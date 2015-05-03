/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GTIMER || defined(__DOXYGEN__)

#define GFX_ALLOW_MULTITHREAD	TRUE
#define GTIMER_USE_THREAD		TRUE

// GTimer flags
#define GTIMER_FLG_PERIODIC		0x0001
#define GTIMER_FLG_INFINITE		0x0002
#define GTIMER_FLG_JABBED		0x0004
#define GTIMER_FLG_SCHEDULED	0x0008

/* While this macro may seem unnecessarily complex, we have to handle the situation where
 * 	the system ticks have wrapped.
 * Don't rework this macro to use a ternary operator - the gcc compiler stuffs it up. This
 * 	method is just as efficient due to short-circuit evaluation.
 */
#define TimeIsWithin(x, start, end)	((end >= start && x >= start && x <= end) || (end < start && (x >= start || x <= end)))

#if GFX_ALLOW_MULTITHREAD
	static gfxMutex	mutex;
	#define MUTEX_INIT()		gfxMutexInit(&mutex)
	#define MUTEX_DEINIT()		gfxMutexDestroy(&mutex)
	#define	MUTEX_ENTER()		gfxMutexEnter(&mutex)
	#define MUTEX_EXIT()		gfxMutexExit(&mutex)
#else
	#define MUTEX_INIT()
	#define MUTEX_DEINIT()
	#define	MUTEX_ENTER()
	#define MUTEX_EXIT()
#endif

#if GTIMER_USE_THREAD
	static gfxThreadHandle	hThread = 0;
	static gfxSem			waitsem;
	static DECLARE_THREAD_STACK(waTimerThread, GTIMER_THREAD_WORKAREA_SIZE);
	#define POLLING_ON()
	#define POLLING_OFF()
	#define POLLING_TEST()	(FALSE)
	#define THREAD_STOP()	hThread = 0
	#define BUMP_INIT()		gfxSemInit(&waitsem, 0, 1)
	#define BUMP_DEINIT()	gfxSemDestroy(&waitsem)
	#define BUMP_BUMPI()	gfxSemSignalI(&waitsem)
	#define BUMP_BUMP()		gfxSemSignal(&waitsem)
#else
	static bool_t	allowPolls = FALSE;
	#define POLLING_ON()	allowPolls = TRUE
	#define POLLING_OFF()	allowPolls = FALSE
	#define POLLING_TEST()	(allowPolls)
	#define THREAD_STOP()
	#define BUMP_INIT()		gfxSemInit(&waitsem, 0, 1)
	#define BUMP_DEINIT()	gfxSemDestroy(&waitsem)
	#define BUMP_BUMPI()	nxtTimerout = 0
	#define BUMP_BUMP()		nxtTimerout = 0
#endif

static GTimer			*pTimerHead = 0;

static systemticks_t	nxtTimeout = TIME_INFINITE;
static systemticks_t	lastTime = 0;

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#if GTIMER_USE_THREAD
	static DECLARE_THREAD_FUNCTION(GTimerThreadHandler, param) {
		systemticks_t	tm;
		GTimer			*pt;
		GTimerFunction	fn;

		while(hThread) {
			/* Wait for work to do. */
			// BUG BUG BUG - CONVERT BACK TO MILLISECONDS
			gfxSemWait(&waitsem, nxtTimeout);

#else
	void _gtimerPoll(void) {
		systemticks_t	tm;
		GTimer			*pt;
		void			*param;
		GTimerFunction	fn;

		if (nxtTimeout == TIME_INFINITE)
			return;

		tm = gfxSystemTicks();
		if (tm - lastTime < nxtTimeout || !POLLING_TEST())
			return;

#endif

restartTimerChecks:

			MUTEX_ENTER();

			// Get the current time and the default next timeout
			tm = gfxSystemTicks();
			nxtTimeout = TIME_INFINITE;

			// Scan through the timer list looking for something that has expired or been jabbed
			if (pTimerHead) {
				pt = pTimerHead;
				do {
					// Has this timer expired or been jabbed?
					if ((pt->flags & GTIMER_FLG_JABBED) || (!(pt->flags & GTIMER_FLG_INFINITE) && TimeIsWithin(pt->when, lastTime, tm))) {

						// Is this timer periodic?
						if ((pt->flags & GTIMER_FLG_PERIODIC) && pt->period != TIME_IMMEDIATE) {
							// Yes - Update ready for the next period
							if (!(pt->flags & GTIMER_FLG_INFINITE)) {
								// We may have skipped a period.
								// We use this complicated formulae rather than a loop
								//	because the gcc compiler stuffs up the loop so that it
								//	either loops forever or doesn't get executed at all.
								pt->when += ((tm + pt->period - pt->when) / pt->period) * pt->period;
							}

							// We are definitely no longer jabbed
							pt->flags &= ~GTIMER_FLG_JABBED;

						} else {
							// No - get us off the timers list
							if (pt->next == pt->prev)
								pTimerHead = 0;
							else {
								pt->next->prev = pt->prev;
								pt->prev->next = pt->next;
								if (pTimerHead == pt)
									pTimerHead = pt->next;
							}
							pt->flags = 0;
						}

						// Call the callback function.
						// Save all the details first so we can safely call it outside the mutex.
						// We don't want recursive polling so make sure that is off over the length of the call
						fn = pt->fn;
						param = pt->param;
						nxtTimeout = TIME_INFINITE;
						MUTEX_EXIT();
						POLLING_OFF();
						fn(param);
						POLLING_ON();

						// We no longer hold the mutex, the callback function may have taken a while
						// and our list may have been altered so start again!
						goto restartTimerChecks;
					}

					// Find when we next need to wake up
					if (!(pt->flags & GTIMER_FLG_INFINITE) && pt->when - tm < nxtTimeout)
						nxtTimeout = pt->when - tm;
					pt = pt->next;
				} while(pt != pTimerHead);
			}

			// Ready for the next loop
			lastTime = tm;
			MUTEX_EXIT();
	#if GTIMER_USE_THREAD
		}

		return 0;
	#endif
	}

void _gtimerInit(void)
{
	BUMP_INIT();
	MUTEX_INIT();
	POLLING_ON();
}

void _gtimerDeinit(void)
{
	POLLING_OFF();
	THREAD_STOP();
	MUTEX_DEINIT();
	BUMP_DEINIT();
}

void gtimerInit(GTimer* pt)
{
	pt->flags = 0;
}

void gtimerDeinit(GTimer* pt)
{
	gtimerStop(pt);
}

void gtimerStart(GTimer *pt, GTimerFunction fn, void *param, bool_t periodic, delaytime_t millisec) {
	MUTEX_ENTER();
	
	#if GTIMER_USE_THREAD
		// Start our thread if not already going
		if (!hThread) {
			hThread = (gfxThreadHandle)1;				// A dummy value just to ensure the thread doesn't prematurely exit.
			hThread = gfxThreadCreate(waTimerThread, sizeof(waTimerThread), GTIMER_THREAD_PRIORITY, GTimerThreadHandler, 0);
			if (hThread) {gfxThreadClose(hThread);}		// We never really need to access the handle again
		}
	#endif

	// Is this already scheduled?
	if (pt->flags & GTIMER_FLG_SCHEDULED) {
		// Cancel it!
		if (pt->next == pt->prev)
			pTimerHead = 0;
		else {
			pt->next->prev = pt->prev;
			pt->prev->next = pt->next;
			if (pTimerHead == pt)
				pTimerHead = pt->next;
		}
	}
	
	// Set up the timer structure
	pt->fn = fn;
	pt->param = param;
	pt->flags = GTIMER_FLG_SCHEDULED;
	if (periodic)
		pt->flags |= GTIMER_FLG_PERIODIC;
	if (millisec == TIME_INFINITE) {
		pt->flags |= GTIMER_FLG_INFINITE;
		pt->period = TIME_INFINITE;
	} else {
		pt->period = gfxMillisecondsToTicks(millisec);
		pt->when = gfxSystemTicks() + pt->period;
	}

	// Just pop it on the end of the queue
	if (pTimerHead) {
		pt->next = pTimerHead;
		pt->prev = pTimerHead->prev;
		pt->prev->next = pt;
		pt->next->prev = pt;
	} else
		pt->next = pt->prev = pTimerHead = pt;

	// Bump the thread
	if (!(pt->flags & GTIMER_FLG_INFINITE))
		BUMP_BUMP();
	MUTEX_EXIT();
}

void gtimerStop(GTimer *pt) {
	MUTEX_ENTER();
	if (pt->flags & GTIMER_FLG_SCHEDULED) {
		// Cancel it!
		if (pt->next == pt)
			pTimerHead = 0;
		else {
			pt->next->prev = pt->prev;
			pt->prev->next = pt->next;
			if (pTimerHead == pt)
				pTimerHead = pt->next;
		}
		// Make sure we know the structure is dead!
		pt->flags = 0;
	}
	MUTEX_EXIT();
}

bool_t gtimerIsActive(GTimer *pt) {
	return (pt->flags & GTIMER_FLG_SCHEDULED) ? TRUE : FALSE;
}

void gtimerJab(GTimer *pt) {
	MUTEX_ENTER();
	
	// Jab it!
	pt->flags |= GTIMER_FLG_JABBED;

	// Bump the thread/poller
	BUMP_BUMP();

	MUTEX_EXIT();
}

void gtimerJabI(GTimer *pt) {
	// Jab it!
	pt->flags |= GTIMER_FLG_JABBED;

	// Bump the thread/poller
	BUMP_BUMPI();
}

#endif /* GFX_USE_GTIMER */
