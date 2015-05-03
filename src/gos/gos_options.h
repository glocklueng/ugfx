/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gos/gos_options.h
 * @brief   GOS - Operating System options header file.
 *
 * @addtogroup GOS
 * @{
 */

#ifndef _GOS_OPTIONS_H
#define _GOS_OPTIONS_H

/**
 * @name    The operating system to use. One (and only one) of these must be defined.
 * @{
 */
	/**
	 * @brief   Use ChibiOS
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_CHIBIOS
		#define GFX_USE_OS_CHIBIOS		FALSE
	#endif
	/**
	 * @brief   Use FreeRTOS
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_FREERTOS
		#define GFX_USE_OS_FREERTOS		FALSE
	#endif
	/**
	 * @brief   Use Win32
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_WIN32
		#define GFX_USE_OS_WIN32		FALSE
	#endif
	/**
	 * @brief   Use a linux based system running X11
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_LINUX
		#define GFX_USE_OS_LINUX		FALSE
	#endif
	/**
	 * @brief   Use a Mac OS-X based system
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_OSX
		#define GFX_USE_OS_OSX			FALSE
	#endif
	/**
	 * @brief   Use a Raw 32 bit CPU based system
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_RAW32
		#define GFX_USE_OS_RAW32		FALSE
	#endif
	/**
	 * @brief   Use a eCos
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_ECOS
		#define GFX_USE_OS_ECOS			FALSE
	#endif
	/**
	 * @brief   Use Arduino
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_ARDUINO
		#define GFX_USE_OS_ARDUINO			FALSE
	#endif
/**
 * @}
 *
 * @name    GOS Optional Parameters
 * @{
 */
 	/**
 	 * @brief	Should uGFX avoid initializing the operating system
 	 * @details	Defaults to FALSE
 	 * @note	This is not relevant to all operating systems eg Win32 never initializes the
 	 * 			operating system as uGFX runs as an application outside the boot process.
 	 * @note	Operating system initialization is not necessarily implemented for all
 	 * 			operating systems yet even when it is relevant. These operating systems
 	 * 			will display a compile warning reminding you to initialize the operating
 	 * 			system in your application code. Note that on these operating systems the
 	 * 			demo applications will not work without modification.
 	 */
 	#ifndef GFX_NO_OS_INIT
 		#define GFX_NO_OS_INIT			FALSE
 	#endif
 	/**
 	 * @brief	Should uGFX stuff be added to the FreeRTOS+Tracer
 	 * @details	Defaults to FALSE
 	 */
 	#ifndef GFX_FREERTOS_USE_TRACE
 		#define GFX_FREERTOS_USE_TRACE	FALSE
 	#endif
 	/**
 	 * @brief	How much RAM (in bytes) should uGFX use for the heap
 	 * @details	Defaults to 0
 	 * @note	If 0 then the safe heap implementation is not used. Either
 	 * 			the C library or the operating system heap manager is used
 	 * 			instead.
 	 * @note	No C runtime library routines will be used for the safe heap.
 	 * @note	A new routine @p gfxAddHeapBlock() is added allowing the user
 	 * 			to add extra memory blocks to the heap.
 	 * @note	The safe heap is safe even in multithread environments. The safe
 	 * 			heap is required as many C library implementations crash when
 	 * 			called from a different stack such as when the threading implementation
 	 * 			included with uGFX is used. Some operating systems also just don't
 	 * 			have a memory allocator included.
 	 */
	#ifndef GFX_OS_SAFEHEAP_SIZE
		#define GFX_OS_SAFEHEAP_SIZE		0
	#endif
 	/**
 	 * @brief	Use the C Library memory allocator
 	 * @details	This option is ignored if the safe heap is used
 	 */
	#ifndef GFX_OS_CLIB_HEAP
		#define GFX_OS_CLIB_HEAP			FALSE
	#endif
/** @} */

#endif /* _GOS_OPTIONS_H */
/** @} */
