/*
 * sdl_cursor.c  SDL cursor 
 *
 * Copyright (C) 2000-     Fumihiko Murata       <fmurata@p1.tcnet.ne.jp>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
*/
/* $Id: sdl_cursor.c,v 1.4 2001/03/30 19:16:38 chikama Exp $ */

#include "config.h"

#include <SDL/SDL.h>

#include "portab.h"
#include "system.h"
#include "cursor.h"
#include "sdl_private.h"

/* マウスカーソルフォントイメージ*/
#include "bitmaps/cursor_arrow.xpm"
#include "bitmaps/cursor_busy.xpm"
#include "bitmaps/cursor_no.xpm"
#include "bitmaps/cursor_move.xpm"
#include "bitmaps/cursor_cross.xpm"
#include "bitmaps/cursor_size_h.xpm"
#include "bitmaps/cursor_size_l.xpm"
#include "bitmaps/cursor_size_r.xpm"
#include "bitmaps/cursor_size_v.xpm"
#include "bitmaps/cursor_uparrow.xpm"
#include "bitmaps/cursor_ibeam.xpm"

static SDL_Cursor      *cursor[256];

/* Stolen from the SDL mailing list */
/* Creates a new mouse cursor from an XPM */

static SDL_Cursor *init_system_cursor(const char *image[]) {
	int i, row, col;
	Uint8 data[4*32];
	Uint8 mask[4*32];
	int hot_x, hot_y;
	
	i = -1;
	for (row = 0; row < 32; row++) {
		for (col = 0; col < 32; col++) {
			if (col % 8) {
				data[i] <<= 1;
				mask[i] <<= 1;
			} else {
				i++;
				data[i] = mask[i] = 0;
			}
			switch (image[4 + row][col]) {
			case 'X':
				data[i] |= 0x01;
				mask[i] |= 0x01;
				break;
			case '.':
				mask[i] |= 0x01;
				break;
			case ' ':
				break;
			}
		}
	}
	sscanf(image[4 + row], "%d,%d", &hot_x, &hot_y);
	return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

/* mouse cursur の初期化 */
void sdl_cursor_init(void) {
	cursor[IDC_ARROW] = init_system_cursor(cursor_arrow);
	cursor[IDC_CROSS] = init_system_cursor(cursor_cross);
	cursor[IDC_IBEAM] = init_system_cursor(cursor_ibeam);
	cursor[IDC_NO]    = init_system_cursor(cursor_no);
	cursor[IDC_SIZE]  = init_system_cursor(cursor_move);
	cursor[IDC_SIZEALL]  = init_system_cursor(cursor_move);
	cursor[IDC_SIZENESW] = init_system_cursor(cursor_size_r);
	cursor[IDC_SIZENS]   = init_system_cursor(cursor_size_v);
	cursor[IDC_SIZENWSE] = init_system_cursor(cursor_size_l);
	cursor[IDC_SIZEWE]   = init_system_cursor(cursor_size_h);
	cursor[IDC_UPARROW]  = init_system_cursor(cursor_uparrow);
	cursor[IDC_WAIT]     = init_system_cursor(cursor_busy);
}

boolean sdl_cursorNew(BYTE* data, int no, CursorImage *cursorImage, TCursorDirEntry *cursordirentry) {
	int    xormasklen, andmasklen, xornum;
	int    i, j;
	int    h = 0;
	
	BYTE   *buf1, *buf2, *buf3, *buf4;
	
	xornum = (cursordirentry->bWidth * cursordirentry->bHeight);
	xormasklen = (xornum * cursorImage->icHeader.biBitCount) / 8;
	NOTICE("Cursor:  xormasklen==%d,  xornum==%d\n", xormasklen, xornum);
	
	andmasklen = xornum / 8;
	cursorImage->xormasklen = xormasklen;
	cursorImage->andmasklen = andmasklen;
	
	buf1 = (BYTE*)malloc(sizeof(BYTE)* xornum);
	buf2 = (BYTE*)malloc(sizeof(BYTE)* xornum);
	buf3 = (BYTE*)malloc(sizeof(BYTE)* xornum);
	buf4 = (BYTE*)malloc(sizeof(BYTE)* xornum);
	
	memcpy(buf1, data, MIN(xormasklen, xornum));
	data += xormasklen;
	
	memcpy(buf2, data, MIN(andmasklen, xornum));
	data += andmasklen;
	
#define height cursordirentry->bHeight
#define width  cursordirentry->bWidth
	
	for (j = 0; j < height; j++) {
		for (i = 0; i < width * cursorImage->icHeader.biBitCount /8; i++) {
			buf3[h] = buf1[(height-j-1)*height*cursorImage->icHeader.biBitCount/8+i];
			buf4[h] = 0xff ^ buf2[(height-j-1)*height*cursorImage->icHeader.biBitCount/8+i];
			h++;
		}
	}
	
	cursor[no] = SDL_CreateCursor(buf3, buf4, 32, 32, cursordirentry->wxHotspot, cursordirentry->wyHotspot);
	
	free(buf1);
	free(buf2);
	free(buf3);
	free(buf4);
	
#undef height
#undef width
	
	return TRUE;
}

/* マウスの位置の移動 */
void sdl_setCursorLocation(int x, int y) {
	if (ms_active) SDL_WarpMouse(x, y);
}

/* マウスカーソルの形状の設定 */
void sdl_setCursorType(int type) {
	if (cursor[type] != NULL) {
		SDL_SetCursor(cursor[type]);
	}
}
