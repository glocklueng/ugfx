/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gos/gos.h
 * @brief   GOS - Operating System Support header file
 *
 * @addtogroup GOS
 *
 * @brief	Module to build a uniform abstraction layer between uGFX and the underlying system
 *
 * @note	Some of the routines specified below may be implemented simply as
 * 			a macro to the real operating system call.
 * @{
 */

#ifndef _GOS_H
#define _GOS_H

#if defined(__DOXYGEN__)
	/*===========================================================================*/
	/* Type definitions                                                          */
	/*===========================================================================*/

	/**
	 * @name	Various integer sizes
	 * @note	Your platform may use slightly different definitions to these
	 * @{
	 */
	typedef unsigned char	bool_t;
	typedef char			int8_t;
	typedef unsigned char	uint8_t;
	typedef short			int16_t;
	typedef unsigned short	uint16_t;
	typedef long			int32_t;
	typedef unsigned long	uint32_t;
	/** @} */

	/**
	 * @name	Various platform (and operating system) dependent types
	 * @note	Your platform may use slightly different definitions to these
	 * @{
	 */
	typedef unsigned long	size_t;
	typedef unsigned long	delaytime_t;
	typedef unsigned long	systemticks_t;
	/** @} */
	/**
	 * @name	More platform (and operating system) dependent types
	 * @pre		These types are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 *
	 * @note	Your platform may use slightly different definitions to these
	 * @{
	 */
	typedef short			semcount_t;
	typedef int				threadreturn_t;
	typedef int				threadpriority_t;
	/** @} */

	/**
	 * @brief	Declare a thread function
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 *
	 * @param[in] fnName	The name of the function
	 * @param[in] param 	A custom parameter that is passed to the function
	 */
	#define DECLARE_THREAD_FUNCTION(fnName, param)	threadreturn_t fnName(void *param)

	/**
	 * @brief	Declare a thread stack
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 *
	 * @param[in] name 		The name of the stack
	 * @param[in] sz 		The size of the stack
	 */
	#define DECLARE_THREAD_STACK(name, sz)			uint8_t name[sz];

	/**
	 * @name	Various platform (and operating system) constants
	 * @note	Your platform may use slightly different definitions to these
	 * @{
	 */
	#define FALSE						0
	#define TRUE						1
	#define TIME_IMMEDIATE				0
	#define TIME_INFINITE				((delaytime_t)-1)
	/** @} */
	/**
	 * @name	More constants
	 * @pre		These constants are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 * @note	Your platform may use slightly different definitions to these
	 * @{
	 */
	#define MAX_SEMAPHORE_COUNT			((semcount_t)(((unsigned long)((semcount_t)(-1))) >> 1))
	#define LOW_PRIORITY				0
	#define NORMAL_PRIORITY				1
	#define HIGH_PRIORITY				2
	/** @} */

	/**
	 * @brief	A semaphore
	 * @note	Your operating system will have a proper definition for this structure
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 */
	typedef struct {} gfxSem;

	/**
	 * @brief	A mutex
	 * @note	Your operating system will have a proper definition for this structure
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 */
	typedef struct {} gfxMutex;

	/**
	 * @brief	A thread handle
	 * @note	Your operating system will have a proper definition for this.
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 */
	typedef void * gfxThreadHandle;

	/*===========================================================================*/
	/* Function declarations.                                                    */
	/*===========================================================================*/

	/**
	 * @brief	Halt the GFX application due to an error.
	 *
	 * @param[in] msg	An optional debug message to show (Can be NULL)
	 *
	 * @api
	 */
	C_FN void gfxHalt(const char *msg);

	/**
	 * @brief	Exit the GFX application.
	 *
	 * @api
	 */
	C_FN void gfxExit(void);

	/**
	 * @brief	Allocate memory
	 * @return	A pointer to the memory allocated or NULL if there is no more memory available
	 *
	 * @param[in] sz	The size in bytes of the area to allocate
	 *
	 * @api
	 */
	C_FN void *gfxAlloc(size_t sz);

	/**
	 * @brief	Re-allocate memory
	 * @return	A pointer to the new memory area or NULL if there is no more memory available
	 *
	 * @param[in] ptr		The old memory area to be increased/decreased in size
	 * @param[in] oldsz		The size in bytes of the old memory area
	 * @param[in] newsz		The size in bytes of the new memory area
	 *
	 * @note		Some operating systems don't use the oldsz parameter as they implicitly know the size of
	 * 				old memory area. The parameter must always be supplied however for API compatibility.
	 * @note		gfxRealloc() can make the area smaller or larger but may have to return a different pointer.
	 * 				If this occurs the new area contains a copy of the data from the old area. The old memory
	 * 				pointer should not be used after this routine as the original area may have been freed.
	 * @note		If there is insufficient memory to create the new memory region, NULL is returned and the
	 * 				old memory area is left unchanged.
	 *
	 * @api
	 */
	C_FN void *gfxRealloc(void *ptr, size_t oldsz, size_t newsz);

	/**
	 * @brief	Free memory
	 *
	 * @param[in] ptr	The memory to free
	 *
	 * @api
	 */
	C_FN void gfxFree(void *ptr);

	/**
	 * @brief	Yield the current thread
	 * @details	Give up the rest of the current time slice for this thread in order to give other threads
	 * 			a chance to run.
	 * @note	This is defined regardless of if GFX_ALLOW_MULTITHREAD is TRUE. On non-multithreading
	 * 			compiles this function will perform system polling and so the semantic meaning is
	 * 			the same regardless of if GFX_ALLOW_MULTITHREAD is set or not.
	 *
	 * @api
	 */
	C_FN void gfxYield(void);

	/**
	 * @brief	Put the current thread to sleep for the specified period in milliseconds
	 *
	 * @param[in] ms	The number milliseconds to sleep
	 *
	 * @note		Specifying TIME_IMMEDIATE will yield the current thread but return
	 * 				on the next time slice.
	 * @note		Specifying TIME_INFINITE will sleep forever.
	 *
	 * @api
	 */
	C_FN void gfxSleepMilliseconds(delaytime_t ms);

	/**
	 * @brief	Put the current thread to sleep for the specified period in microseconds
	 *
	 * @param[in] us	The number microseconds to sleep
	 *
	 * @note		Specifying TIME_IMMEDIATE will return immediately (no sleeping)
	 * @note		Specifying TIME_INFINITE will sleep forever.
	 *
	 * @api
	 */
	C_FN void gfxSleepMicroseconds(delaytime_t us);

	/**
	 * @brief	Get the current operating system tick time
	 * @return	The current tick time
	 *
	 * @note	A "tick" is an arbitrary period of time that the operating
	 * 			system uses to mark time.
	 * @note	The absolute value of this call is relatively meaningless. Its usefulness
	 * 			is in calculating periods between two calls to this function.
	 * @note	As the value from this function can wrap it is important that any periods are calculated
	 * 			as t2 - t1 and then compared to the desired period rather than comparing
	 * 			t1 + period to t2
	 *
	 * @api
	 */
	C_FN systemticks_t gfxSystemTicks(void);

	/**
	 * @brief	Convert a given number of millseconds to a number of operating system ticks
	 * @return	The period in system ticks.
	 *
	 * @note	A "tick" is an arbitrary period of time that the operating
	 * 			system uses to mark time.
	 *
	 * @param[in] ms	The number of millseconds
	 *
	 * @api
	 */
	C_FN systemticks_t gfxMillisecondsToTicks(delaytime_t ms);

	/**
	 * @brief	Lock the operating system to protect a sequence of code
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 *
	 * @note	Calling this will lock out all other threads from executing even at interrupt level
	 * 			within the GFX system. On hardware this may be implemented as a disabling of interrupts,
	 * 			however in an operating system which hides real interrupt level code it may simply use a
	 * 			mutex lock.
	 * @note	The thread MUST NOT block whilst the system is locked. It must execute in this state for
	 * 			as short a period as possible as this can seriously affect interrupt latency on some
	 * 			platforms.
	 * @note	While locked only interrupt level (iclass) GFX routines may be called.
	 *
	 * @api
	 */
	C_FN void gfxSystemLock(void);

	/**
	 * @brief	Unlock the operating system previous locked by gfxSystemLock()
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 *
	 * @api
	 */
	C_FN void gfxSystemUnlock(void);

	/**
	 * @brief	Initialise a mutex to protect a region of code from other threads.
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 *
	 * @param[in]	pmutex	A pointer to the mutex
	 *
	 * @note	Whilst a counting semaphore with a limit of 1 can be used for similiar purposes
	 * 			on many operating systems using a seperate mutex structure is more efficient.
	 *
	 * @api
	 */
	C_FN void gfxMutexInit(gfxMutex *pmutex);

	/**
	 * @brief	Destroy a Mutex.
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 *
	 * @param[in]	pmutex	A pointer to the mutex
	 *
	 * @api
	 */
	C_FN void gfxMutexDestroy(gfxMutex *pmutex);

	/**
	 * @brief	Enter the critical code region protected by the mutex.
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 * @details	Blocks until there is no other thread in the critical region.
	 *
	 * @param[in]	pmutex	A pointer to the mutex
	 *
	 * @api
	 */
	C_FN void gfxMutexEnter(gfxMutex *pmutex);

	/**
	 * @brief	Exit the critical code region protected by the mutex.
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 * @details	May cause another thread waiting on the mutex to now be placed into the run queue.
	 *
	 * @param[in]	pmutex	A pointer to the mutex
	 *
	 * @api
	 */
	C_FN void gfxMutexExit(gfxMutex *pmutex);

	/**
	 * @brief	Initialise a Counted Semaphore
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 *
	 * @param[in] psem		A pointer to the semaphore
	 * @param[in] val		The initial value of the semaphore
	 * @param[in] limit		The maxmimum value of the semaphore
	 *
	 * @note	Operations defined for counted semaphores:
	 * 				Signal: The semaphore counter is increased and if the result is non-positive then a waiting thread
	 * 						 is queued for execution. Note that once the thread reaches "limit", further signals are
	 * 						 ignored.
	 * 				Wait: The semaphore counter is decreased and if the result becomes negative the thread is queued
	 * 						in the semaphore and suspended.
	 *
	 * @api
	 */
	C_FN void gfxSemInit(gfxSem *psem, semcount_t val, semcount_t limit);

	/**
	 * @brief	Destroy a Counted Semaphore
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 *
	 * @param[in] psem		A pointer to the semaphore
	 *
	 * @note	Any threads waiting on the semaphore will be released
	 *
	 * @api
	 */
	C_FN void gfxSemDestroy(gfxSem *psem);

	/**
	 * @brief	Wait on a semaphore
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 * @details	The semaphore counter is decreased and if the result becomes negative the thread waits for it to become
	 * 				non-negative again
	 * @return	FALSE if the wait timeout occurred otherwise TRUE
	 *
	 * @param[in] psem		A pointer to the semaphore
	 * @param[in] ms		The maximum time to wait for the semaphore
	 *
	 * @api
	 */
	C_FN bool_t gfxSemWait(gfxSem *psem, delaytime_t ms);

	/**
	 * @brief	Test if a wait on a semaphore can be satisfied immediately
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 * @details	Equivalent to @p gfxSemWait(psem, TIME_IMMEDIATE) except it can be called at interrupt level
	 * @return	FALSE if the wait would occur occurred otherwise TRUE
	 *
	 * @param[in] psem		A pointer to the semaphore
	 *
	 * @iclass
	 * @api
	 */
	C_FN bool_t gfxSemWaitI(gfxSem *psem);

	/**
	 * @brief	Signal a semaphore
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 * @details	The semaphore counter is increased and if the result is non-positive then a waiting thread
	 * 						 is queued for execution. Note that once the thread reaches "limit", further signals are
	 * 						 ignored.
	 *
	 * @param[in] psem		A pointer to the semaphore
	 *
	 * @api
	 */
	C_FN void gfxSemSignal(gfxSem *psem);

	/**
	 * @brief	Signal a semaphore
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 * @details	The semaphore counter is increased and if the result is non-positive then a waiting thread
	 * 						 is queued for execution. Note that once the thread reaches "limit", further signals are
	 * 						 ignored.
	 *
	 * @param[in] psem		A pointer to the semaphore
	 *
	 * @iclass
	 * @api
	 */
	C_FN void gfxSemSignalI(gfxSem *psem);

	/**
	 * @brief	Start a new thread.
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 * @return	Returns a thread handle if the thread was started, NULL on an error
	 *
	 * @param[in]	stackarea	A pointer to the area for the new threads stack or NULL to dynamically allocate it
	 * @param[in]	stacksz		The size of the thread stack. 0 means the default operating system size although this
	 * 							is only valid when stackarea is dynamically allocated.
	 * @param[in]	prio		The priority of the new thread
	 * @param[in]	fn			The function the new thread will run
	 * @param[in]	param		A parameter to pass the thread function.
	 *
	 * @api
	 */
	C_FN gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param);

	/**
	 * @brief	Wait for a thread to finish.
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 * @return	Returns the thread exit code.
	 *
	 * @param[in]	thread		The Thread Handle
	 *
	 * @note		This will also close the thread handle as it is no longer useful
	 * 				once the thread has ended.
	 * @api
	 */
	C_FN threadreturn_t gfxThreadWait(gfxThreadHandle thread);

	/**
	 * @brief	Get the current thread handle.
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 * @return	A thread handle
	 *
	 * @api
	 */
	C_FN gfxThreadHandle gfxThreadMe(void);

	/**
	 * @brief	Close the thread handle.
	 * @pre		This are only defined if GFX_ALLOW_MULTITHREAD is TRUE
	 *
	 * @param[in]	thread		The Thread Handle
	 *
	 * @note	This does not affect the thread, it just closes our handle to the thread.
	 *
	 * @api
	 */
	C_FN void gfxThreadClose(gfxThreadHandle thread);

/**
 * All the above was just for the doxygen documentation. All the implementation of the above
 * (without any of the documentation overheads) is in the files below.
 */
#elif GFX_USE_OS_RAWRTOS
 	#include "src/gos/gos_rawrtos.h"
#elif GFX_USE_OS_CHIBIOS
	#include "src/gos/gos_chibios.h"
#elif GFX_USE_OS_FREERTOS
	#include "src/gos/gos_freertos.h"
#elif GFX_USE_OS_WIN32
	#include "src/gos/gos_win32.h"
#elif GFX_USE_OS_LINUX
	#include "src/gos/gos_linux.h"
#elif GFX_USE_OS_OSX
	#include "src/gos/gos_osx.h"
#elif GFX_USE_OS_RAW32
	#include "src/gos/gos_raw32.h"
#elif GFX_USE_OS_ECOS
	#include "src/gos/gos_ecos.h"
#elif GFX_USE_OS_ARDUINO
	#include "src/gos/gos_arduino.h"
#else
	#error "Your operating system is not supported yet"
#endif

/**
 * In some cases the operating system will use one of the pre-canned implementations below.
 */

#if GFX_OS_POLLS
	C_FN void gfxPollOff(void);
	C_FN void gfxPollOn(void);
	C_FN void gfxPoll(void);
#endif

#if GFX_OS_NEED_EXIT
	C_FN void gfxHalt(const char *msg);
	C_FN void gfxExit(void);
#endif

#if GFX_OS_SAFEHEAP_SIZE != 0
	C_FN void gfxAddHeapBlock(void *ptr, size_t sz);
#endif

#if GFX_OS_SAFEHEAP_SIZE != 0 || GFX_OS_CLIB_HEAP
	C_FN void *gfxAlloc(size_t sz);
	C_FN void *gfxRealloc(void *ptr, size_t oldsz, size_t sz);
	C_FN void gfxFree(void *ptr);
#endif

#if	GFX_OS_SLEEP_YIELD
	C_FN void gfxSleepMilliseconds(delaytime_t ms);
	C_FN void gfxSleepMicroseconds(delaytime_t ms);
#endif

#if GFX_OS_NEED_THREADS && GFX_ALLOW_MULTITHREAD
	C_FN void gfxSystemLock(void);
	C_FN void gfxSystemUnlock(void);
	C_FN void gfxMutexInit(gfxMutex *pmutex);
	C_FN void gfxMutexEnter(gfxMutex *pmutex);
	C_FN void gfxMutexExit(gfxMutex *pmutex);
	C_FN void gfxSemInit(gfxSem *psem, semcount_t val, semcount_t limit);
	C_FN bool_t gfxSemWait(gfxSem *psem, delaytime_t ms);
	C_FN bool_t gfxSemWaitI(gfxSem *psem);
	C_FN void gfxSemSignal(gfxSem *psem);
	C_FN void gfxSemSignalI(gfxSem *psem);
	static void _gosThreadsInit(void) {
		Qinit(&readyQ);
		current = &mainthread;
		current->next = 0;
		current->size = sizeof(thread);
		current->flags = FLG_THD_MAIN;
		current->fn = 0;
		current->param = 0;

		#if AUTO_DETECT_MASK
			{
				uint32_t	i;
				char **		pout;
				char **		pin;
				size_t		diff;
				char *		framebase;

				// Allocate a buffer to store our test data
				pframeinfo = gfxAlloc(sizeof(saveloc)*2);

				// Get details of the stack frame from within a function
				get_stack_state_in_fn();

				// Get details of the stack frame outside the function
				get_stack_state();

				/* Work out the frame entries to relocate by treating the jump buffer as an array of pointers */
				stackdirup =  pframeinfo[1].localptr > pframeinfo[0].localptr;
				pout = (char **)pframeinfo[0].cxt;
				pin =  (char **)pframeinfo[1].cxt;
				diff = pframeinfo[0].localptr - pframeinfo[1].localptr;
				framebase = pframeinfo[0].localptr;
				jmpmask1 = jmpmask2 = 0;
				for (i = 0; i < sizeof(jmp_buf)/sizeof(char *); i++, pout++, pin++) {
					if ((size_t)(*pout - *pin) == diff) {
						if (i < 32)
							jmpmask1 |= 1 << i;
						else
							jmpmask2 |= 1 << (i-32);

						if (stackdirup) {
							if (framebase > *pout)
								framebase = *pout;
						} else {
							if (framebase < *pout)
								framebase = *pout;
						}
					}
				}
				stackbase = stackdirup ? (pframeinfo[0].localptr - framebase) : (framebase - pframeinfo[0].localptr);

				// Clean up
				gfxFree(pframeinfo);
			}
		#endif
	}

	gfxThreadHandle gfxThreadMe(void) {
		return (gfxThreadHandle)current;
	}

	void gfxYield(void) {
		if (!_setjmp(current->cxt)) {
			// Add us back to the Queue
			Qadd(&readyQ, current);

			// Check if there are dead processes to deallocate
			while ((current = Qpop(&deadQ)))
				gfxFree(current);

			// Run the next process
			current = Qpop(&readyQ);
			_longjmp(current->cxt, 1);
		}
	}

	// This routine is not currently public - but it could be.
	void gfxThreadExit(threadreturn_t ret) {
		// Save the results
		current->param = (void *)ret;
		current->flags |= FLG_THD_DEAD;

		// Add us to the dead list if we need deallocation as we can't free ourselves.
		// If someone is waiting on the thread they will do the cleanup.
		if ((current->flags & (FLG_THD_ALLOC|FLG_THD_WAIT)) == FLG_THD_ALLOC)
			Qadd(&deadQ, current);

		// Switch to the next task
		current = Qpop(&readyQ);
		if (!current)
			gfxExit();		// Oops - this should never happen!
		_longjmp(current->cxt, 1);
	}

	gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param) {
		thread *	t;
		(void)		prio;

		// Ensure we have a minimum stack size
		if (stacksz < sizeof(thread)+64) {
			stacksz = sizeof(thread)+64;
			stackarea = 0;
		}

		if (stackarea) {
			t = (thread *)stackarea;
			t->flags = 0;
		} else {
			t = (thread *)gfxAlloc(stacksz);
			if (!t)
				return 0;
			t->flags = FLG_THD_ALLOC;
		}
		t->size = stacksz;
		t->fn = fn;
		t->param = param;
		if (_setjmp(t->cxt)) {
			// This is the new thread - call the function!
			gfxThreadExit(current->fn(current->param));

			// We never get here
			return 0;
		}

		// Move the stack frame and relocate the context data
		{
			char **	s;
			char *	nf;
			int		diff;
			uint32_t	i;

			// Copy the stack frame
			#if AUTO_DETECT_MASK
				if (STACK_DIR_UP) {					// Stack grows up
					nf = (char *)(t) + sizeof(thread) + stackbase;
					memcpy(t+1, (char *)&t - stackbase, stackbase+sizeof(char *));
				} else {							// Stack grows down
					nf = (char *)(t) + stacksz - (stackbase + sizeof(char *));
					memcpy(nf, &t, stackbase+sizeof(char *));
				}
			#elif STACK_DIR_UP
				// Stack grows up
				nf = (char *)(t) + sizeof(thread) + stackbase;
				memcpy(t+1, (char *)&t - stackbase, stackbase+sizeof(char *));
			#else
				// Stack grows down
				nf = (char *)(t) + size - (stackbase + sizeof(char *));
				memcpy(nf, &t, stackbase+sizeof(char *));
			#endif

			// Relocate the context data
			s = (char **)(t->cxt);
			diff = nf - (char *)&t;

			// Relocate the elements we know need to be relocated
			for (i = 1; i && i < MASK1; i <<= 1, s++) {
				if ((MASK1 & i))
					*s += diff;
			}
			#ifdef MASK2
				for (i = 1; i && i < MASK2; i <<= 1, s++) {
					if ((MASK1 & i))
						*s += diff;
				}
			#endif
		}

		// Add this thread to the ready queue
		Qadd(&readyQ, t);
		return t;
	}

	threadreturn_t gfxThreadWait(gfxThreadHandle th) {
		thread *		t;

		t = th;
		if (t == current)
			return -1;

		// Mark that we are waiting
		t->flags |= FLG_THD_WAIT;

		// Wait for the thread to die
		while(!(t->flags & FLG_THD_DEAD))
			gfxYield();

		// Unmark
		t->flags &= ~FLG_THD_WAIT;

		// Clean up resources if needed
		if (t->flags & FLG_THD_ALLOC)
			gfxFree(t);

		// Return the status left by the dead process
		return (threadreturn_t)t->param;
	}

#endif

#endif /* _GOS_H */
/** @} */
