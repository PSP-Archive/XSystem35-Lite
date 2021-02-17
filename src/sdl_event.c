/*
 * sdl_darw.c  SDL event handler
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
/* $Id: sdl_event.c,v 1.5 2001/12/16 17:12:56 chikama Exp $ */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "portab.h"
#include "system.h"
#include "counter.h"
#include "nact.h"
#include "sdl_private.h"
#include "key.h"
#include "menu.h"
#include "imput.h"
#include "joystick.h"
#include "sdl_input.c"

static void sdl_getEvent(void);
static void keyEventProsess(SDL_KeyboardEvent *e, boolean bool);
static int  check_button(void);


/* pointer の状態 */
static int mousex, mousey, mouseb;
boolean RawKeyInfo[256];
/* SDL Joystick */
static int joyinfo=0;

static int mouse_to_rawkey(int button) {
	switch(button) {
	case SDL_BUTTON_LEFT:
		return KEY_MOUSE_LEFT;
	case SDL_BUTTON_MIDDLE:
		return KEY_MOUSE_MIDDLE;
	case SDL_BUTTON_RIGHT:
		return KEY_MOUSE_RIGHT;
	}
	return 0;
}


static SDLKey transJoystickButton(Uint8 button)
{
#if defined(PSP)    
    SDLKey button_map[] = { SDLK_ESCAPE, /* TRIANGLE */
                            SDLK_LMETA,  /* CIRCLE   */
                            SDLK_RMETA,  /* CROSS    */
                            SDLK_TAB,    /* SQUARE   */
                            SDLK_UNKNOWN,/* LTRIGGER */
                            SDLK_UNKNOWN,/* RTRIGGER */
                            SDLK_DOWN,   /* DOWN     */
                            SDLK_LEFT,   /* LEFT     */
                            SDLK_UP,     /* UP       */
                            SDLK_RIGHT,  /* RIGHT    */
                            SDLK_UNKNOWN,/* SELECT   */
                            SDLK_q,      /* START    */
                            SDLK_UNKNOWN,/* HOME     */ /* kernel mode only */
                            SDLK_UNKNOWN,/* HOLD     */};
    return button_map[button];
#endif
    return SDLK_UNKNOWN;
}

/* Event処理 */
static void sdl_getEvent(void) {
	SDL_Event e;
	boolean m2b = FALSE, msg_skip = FALSE;
	int i;

#if defined(USE_JOYSTICK)
	{
		static long lasttime = 0;
		long nowtime;
		int x, y, mx, my;
		mx = joy_getaxis(0) / 3200;
		my = joy_getaxis(1) / 3200;
		nowtime = SDL_GetTicks();
		if((mx!=0 || my!=0) && lasttime + 5 < nowtime){
			SDL_GetMouseState(&x, &y);
			SDL_WarpMouse(x+mx, y+my);
			lasttime = nowtime;
		}
	}
#endif
	
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_ACTIVEEVENT:
			if (e.active.state & SDL_APPMOUSEFOCUS) {
				ms_active = e.active.gain;
#if 0
				if (sdl_fs_on) {
					if (ms_active) {
						SDL_WM_GrabInput(SDL_GRAB_ON);
					} else {
						SDL_WM_GrabInput(SDL_GRAB_OFF);
					}
				}
#endif
			}
			if (e.active.state & SDL_APPINPUTFOCUS) {
			}
			
			break;
#if defined(USE_JOYSTICK)
		case SDL_JOYBUTTONDOWN:
            e.key.keysym.sym = transJoystickButton(e.jbutton.button);
            if(e.key.keysym.sym == SDLK_UNKNOWN){
                break;
			} else if(e.key.keysym.sym == SDLK_LMETA){
				mouseb |= (1<<SDL_BUTTON_LEFT);
				RawKeyInfo[mouse_to_rawkey(SDL_BUTTON_LEFT)] = TRUE;
			} else if(e.key.keysym.sym == SDLK_RMETA){
				mouseb |= (1<<SDL_BUTTON_RIGHT);
				RawKeyInfo[mouse_to_rawkey(SDL_BUTTON_RIGHT)] = TRUE;
			} else {
				keyEventProsess(&e.key, TRUE);
			}
			break;

		case SDL_JOYBUTTONUP:
            e.key.keysym.sym = transJoystickButton(e.jbutton.button);
            if(e.key.keysym.sym == SDLK_UNKNOWN){
                break;
			} else if(e.key.keysym.sym == SDLK_LMETA){
				mouseb |= mouseb &= (0xffffffff ^ (1 << SDL_BUTTON_LEFT));
				RawKeyInfo[mouse_to_rawkey(SDL_BUTTON_LEFT)] = FALSE;
			} else if(e.key.keysym.sym == SDLK_RMETA){
				mouseb |= mouseb &= (0xffffffff ^ (1 << SDL_BUTTON_RIGHT));
				RawKeyInfo[mouse_to_rawkey(SDL_BUTTON_RIGHT)] = FALSE;
				m2b = TRUE;
			} else {
				keyEventProsess(&e.key, FALSE);
				if (e.key.keysym.sym == SDLK_F1) msg_skip = TRUE;
				if (e.key.keysym.sym == SDLK_F4) {
					SDL_WM_ToggleFullScreen(sdl_display);
					sdl_fs_on = !sdl_fs_on;
				}
			}
			break;
#endif
				
		case SDL_KEYDOWN:
			keyEventProsess(&e.key, TRUE);
			break;

		case SDL_KEYUP:
			keyEventProsess(&e.key, FALSE);
			if (e.key.keysym.sym == SDLK_F1) msg_skip = TRUE;
			if (e.key.keysym.sym == SDLK_F4) {
				SDL_WM_ToggleFullScreen(sdl_display);
				sdl_fs_on = !sdl_fs_on;
			}
			break;
		case SDL_MOUSEMOTION:
			mousex = e.motion.x;
			mousey = e.motion.y;
			break;
		case SDL_MOUSEBUTTONDOWN:
			mouseb |= (1 << e.button.button);
			RawKeyInfo[mouse_to_rawkey(e.button.button)] = TRUE;
#if 0
			if (e.button.button == 2) {
				keywait_flag=TRUE;
			}
#endif
			break;
		case SDL_MOUSEBUTTONUP:
			mouseb &= (0xffffffff ^ (1 << e.button.button));
			RawKeyInfo[mouse_to_rawkey(e.button.button)] = FALSE;
			if (e.button.button == 2) {
				m2b = TRUE;
			}
			break;

		case SDL_QUIT:
			sys_exit(0);
			break;

		default:
			printf("ev %x\n", e.type);
			break;
		}
	}
	
	if (m2b) {
		menu_open();
	}
	
	if (msg_skip) set_skipMode(!get_skipMode());
}

int sdl_keywait(int msec, boolean cancel) {
	int key=0, n;
	int cnt = get_high_counter(SYSTEMCOUNTER_MSEC);
	
	if (msec < 0) return 0;
	
	while (msec > (get_high_counter(SYSTEMCOUNTER_MSEC) - cnt)) {
		sdl_getEvent();
		key = check_button() | sdl_getKeyInfo() | joy_getinfo();
		if (cancel && key) break;
		n = msec - (get_high_counter(SYSTEMCOUNTER_MSEC) - cnt);
		if (n < 0) break;
		if (n < 10) {
			SDL_Delay(n); //usleep(1000 * n);
		} else {
			SDL_Delay(10); //usleep(10000);
		}
		nact->callback();
	}
	
	return key;
}

/* キー情報の取得 */
static void keyEventProsess(SDL_KeyboardEvent *e, boolean bool) {
	RawKeyInfo[sdl_keytable[e->keysym.sym]] = bool;
}

int sdl_getKeyInfo() {
	int rt;
	
	sdl_getEvent();
	
	rt = ((RawKeyInfo[KEY_UP]     || RawKeyInfo[KEY_PAD_8])       |
	      ((RawKeyInfo[KEY_DOWN]  || RawKeyInfo[KEY_PAD_2]) << 1) |
	      ((RawKeyInfo[KEY_LEFT]  || RawKeyInfo[KEY_PAD_4]) << 2) |
	      ((RawKeyInfo[KEY_RIGHT] || RawKeyInfo[KEY_PAD_6]) << 3) |
	      (RawKeyInfo[KEY_RETURN] << 4) |
	      (RawKeyInfo[KEY_SPACE ] << 5) |
	      (RawKeyInfo[KEY_ESC]    << 6) |
	      (RawKeyInfo[KEY_TAB]    << 7));
	
	return rt;
}

static int check_button(void) {
	int m1, m2;
	
	m1 = mouseb & (1 << 1) ? SYS35KEY_RET : 0;
	m2 = mouseb & (1 << 3) ? SYS35KEY_SPC : 0;
	
	return m1 | m2;
}

int sdl_getMouseInfo(MyPoint *p) {
	sdl_getEvent();
	
	if (!ms_active) {
		if (p) {
			p->x = 0; p->y = 0;
		}
		return 0;
	}
	
	if (p) {
		p->x = mousex - winoffset_x;
		p->y = mousey - winoffset_y;
	}
	return check_button();
}

#if 0
int sdl_getjoyinfo(void) {
	sdl_getEvent();
	return joyinfo;
}
#endif

