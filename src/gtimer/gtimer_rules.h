/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gtimer/gtimer_rules.h
 * @brief   GTIMER safety rules header file.
 *
 * @addtogroup GTIMER
 * @{
 */

#ifndef _GTIMER_RULES_H
#define _GTIMER_RULES_H

#if GFX_USE_GTIMER
	#if GFX_ALLOW_MULTITHREAD && !GFX_CAN_POLL_DURING_LOCKS && !GTIMER_USE_THREAD
		#if GFX_DISPLAY_RULE_WARNINGS
			#warning "GTIMER: Your operating system requires GTIMER_USE_THREAD when GFX_ALLOW_MULTITHREAD is TRUE. It has been turned on for you."
		#endif
		#undef GTIMER_USE_THREAD
		#define GTIMER_USE_THREAD	TRUE
	#endif
	#if GTIMER_USE_THREAD && !GFX_ALLOW_MULTITHREAD
		#error "GTIMER: You must specify GFX_ALLOW_MULTITHREAD if GTIMER_USE_THREAD is TRUE"
	#endif
#endif

#endif /* _GTIMER_RULES_H */
/** @} */
