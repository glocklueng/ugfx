/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	src/gwin/gwin_textedit.h
 * @brief	GWIN textedit widget header file
 *
 * @defgroup TextEdit TextEdit
 * @ingroup Widgets
 *
 * @details		A GWIN TextEdit widget allows user input.
 *
 * @pre			GFX_USE_GDISP must be set to TRUE in your gfxconf.h
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GDISP_NEED_TEXT must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_TEXTEDIT must be set to TRUE in your gfxconf.h
 * @pre			The fonts you want to use must be enabled in your gfxconf.h
 *
 * @{
 */

#ifndef _GWIN_TEXTEDIT_H
#define _GWIN_TEXTEDIT_H

// This file is included within "src/gwin/gwin_widget.h"

// A TextEdit widget
typedef struct GTexteditObject {
	GWidgetObject	w;
} GTexteditObject;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief				Create a TextEdit widget.
 * @details				A TextEdit widget is a rectangular box which allows the user to input data through a keyboard.
 *						The keyboard can either be a physical one or a virtual on-screen keyboard as the keyboard driver
 *						is abstracted through the GINPUT module.
 *
 * @param[in] g			The GDisplay on which the textedit should be displayed
 * @param[in] widget	The TextEdit structure to initialise. If this is NULL, the structure is dynamically allocated.
 * @param[in] pInit		The initialisation parameters to use.
 *
 * @return				NULL if there is no resultat drawing area, otherwise the widget handle.
 *
 * @api
 */
GHandle gwinGTexteditCreate(GDisplay* g, GTexteditObject* widget, GWidgetInit* pInit);
#define gwinTexteditCreate(w, pInit)			gwinGTexteditCreate(GDISP, w, pInit)

#ifdef __cplusplus
}
#endif

#endif // _GWIN_TEXTEDIT_H
/** @} */