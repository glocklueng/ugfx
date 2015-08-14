/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	src/gwin/gwin_textedit.c
 * @brief	GWIN TextEdit widget header file
 */

#include "gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_TEXTEDIT

#include "gwin_class.h"
#include <string.h>
#include <stdio.h>

// Some settings
 const int CURSOR_EXTRA_HEIGHT = 1;

// Macros to assist in data type conversions
#define gh2obj ((GTexteditObject *)gh)
#define gw2obj ((GTexteditObject *)gw)

// cursorPos is the position of the next character
// textBuffer[cursorPos++] = readKey();

// ToDo: Optimize by using strncpy() instead
static void _shiftTextLeft(char* buffer, size_t bufferSize, size_t index)
{
	// Find the end of the string
	size_t indexTerminator = index;
	while (buffer[indexTerminator] != '\0' && indexTerminator < bufferSize-1) {
		indexTerminator++;
	}

	// Shift
	size_t i = 0;
	for (i = index; i < indexTerminator+1; i++) {
		buffer[i-1] = buffer[i];
	}
	buffer[indexTerminator] = '\0';
}

// ToDo: Optimize by using strncpy() instead
static void _shiftTextRight(char* buffer, size_t bufferSize, size_t index, char fillChar)
{
	// Find the end of the string
	size_t indexTerminator = index;
	while (buffer[indexTerminator] != '\0' && indexTerminator < bufferSize-1) {
		indexTerminator++;
	}

	// Shift
	size_t i = 0;
	for (i = indexTerminator+1; i > index; i--) {
		if (i > bufferSize-1) {
			break;
		}

		buffer[i] = buffer[i-1];
	}

	// Fill the gap
	buffer[index] = fillChar;
}


#if GINPUT_NEED_KEYBOARD
	static void _keyboardEvent(GWidgetObject* gw, GEventKeyboard* pke)
	{
		// Is it a special key?
		if (pke->keystate & GKEYSTATE_SPECIAL) {
			// Arrow keys to move the cursor
			switch ((uint8_t)pke->c[0]) {
			case GKEY_LEFT:
				if (gw2obj->cursorPos > 0) {
					gw2obj->cursorPos--;
				}
				break;

			case GKEY_RIGHT:
				if (gw2obj->cursorPos < strlen(gw2obj->textBuffer)) {
					gw2obj->cursorPos++;
				}
				break;


			default:
				break;
			}
		}

		// Parse the key press
		else if (pke->bytecount >= 1) {
			// Check backspace
			if (pke->c[0] == GKEY_BACKSPACE) {
				if (gw2obj->cursorPos == 0) {
					return;
				}
				_shiftTextLeft(gw2obj->textBuffer, gw2obj->bufferSize, gw2obj->cursorPos--);
			}

			// Add a new character
			else {
				// Shift everything right from the cursor by one character. This includes the '\0'. Then inser the new character.
				_shiftTextRight(gw2obj->textBuffer, gw2obj->bufferSize, gw2obj->cursorPos++, pke->c[0]);
			}

			// Set the new text
			gwinSetText((GHandle)gw, gw2obj->textBuffer, FALSE);
		}

		_gwinUpdate((GHandle)gw);
	}
#endif

static void gwinTexteditDefaultDraw(GWidgetObject* gw, void* param);

static const gwidgetVMT texteditVMT = {
	{
		"TextEdit",					// The class name
		sizeof(GTexteditObject),	// The object size
		_gwidgetDestroy,			// The destroy routine
		_gwidgetRedraw, 			// The redraw routine
		0,							// The after-clear routine
	},
	gwinTexteditDefaultDraw,		// default drawing routine
	#if GINPUT_NEED_MOUSE
		{
			0,						// Process mose down events (NOT USED)
			0,						// Process mouse up events (NOT USED)
			0,						// Process mouse move events (NOT USED)
		},
	#endif
	#if GINPUT_NEED_KEYBOARD
		{
			_keyboardEvent,			// Process keyboard key down events
		},
	#endif
	#if GINPUT_NEED_TOGGLE
		{
			0,						// No toggle role
			0,						// Assign Toggles (NOT USED)
			0,						// Get Toggles (NOT USED)
			0,						// Process toggle off event (NOT USED)
			0,						// Process toggle on event (NOT USED)
		},
	#endif
	#if GINPUT_NEED_DIAL
		{
			0,						// No dial roles
			0,						// Assign Dials (NOT USED)
			0, 						// Get Dials (NOT USED)
			0,						// Procees dial move events (NOT USED)
		},
	#endif
};

GHandle gwinGTexteditCreate(GDisplay* g, GTexteditObject* widget, GWidgetInit* pInit, size_t bufSize)
{
	uint16_t flags = 0;

	// Create the underlying widget
	if (!(widget = (GTexteditObject*)_gwidgetCreate(g, &widget->w, pInit, &texteditVMT))) {
		return 0;
	}

	// Allocate the text buffer
	widget->bufferSize = bufSize;
	widget->textBuffer = gfxAlloc(widget->bufferSize);
	if (widget->textBuffer == 0) {
		return 0;
	}

	// Initialize the text buffer
	size_t i = 0;
	for (i = 0; i < bufSize; i++) {
		widget->textBuffer[i] = '\0';
	}

	widget->cursorPos = 0;
	widget->w.g.flags |= flags;
	gwinSetVisible(&widget->w.g, pInit->g.show);

	return (GHandle)widget;
}

static void gwinTexteditDefaultDraw(GWidgetObject* gw, void* param)
{
	(void)param;

	// Is it a valid handle?
	if (gw->g.vmt != (gwinVMT*)&texteditVMT) {
		return;
	}

	// Retrieve colors
	color_t textColor = (gw->g.flags & GWIN_FLG_SYSENABLED) ? gw->pstyle->enabled.text : gw->pstyle->disabled.text;
	color_t cursorColor = (gw->g.flags & GWIN_FLG_SYSENABLED) ? gw->pstyle->enabled.edge : gw->pstyle->disabled.edge;

	// Render background and string
	gdispGFillStringBox(gw->g.display, gw->g.x, gw->g.y, gw->g.width, gw->g.height, gw->text, gw->g.font, textColor, gw->pstyle->background, justifyLeft);

	// Render cursor (if focused)
	if (gwinGetFocus() == (GHandle)gw || TRUE) {
		// Calculate cursor stuff
		char textBeforeCursor[gw2obj->bufferSize];
		strncpy(textBeforeCursor, gw->text, gw2obj->cursorPos+1);
		textBeforeCursor[gw2obj->cursorPos] = '\0';
		coord_t textWidth = gdispGetStringWidth(textBeforeCursor, gw->g.font);
		coord_t cursorHeight = gdispGetFontMetric(gw->g.font, fontHeight);
		coord_t cursorSpacingTop = (gw->g.height - cursorHeight)/2 - CURSOR_EXTRA_HEIGHT;
		coord_t cursorSpacingBottom = (gw->g.height - cursorHeight)/2 - CURSOR_EXTRA_HEIGHT;

		// Draw cursor
		coord_t lineX0 = gw->g.x + textWidth - 2;
		coord_t lineX1 = gw->g.x + textWidth - 2;
		coord_t lineY0 = gw->g.y + cursorSpacingTop;
		coord_t lineY1 = gw->g.y + gw->g.height - cursorSpacingBottom;
		gdispGDrawLine(gw->g.display, lineX0, lineY0, lineX1, lineY1, cursorColor);
	}
}

#undef gh2obj
#undef gw2obj

#endif // GFX_USE_GWIN && GWIN_NEED_TEXTEDIT
