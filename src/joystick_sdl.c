/*
 * joystick_sdl.c  joystick interface for SDL 1.1 and over
 *
 * Copyright (C) 2000- Fumihiko Murata <fmurata@p1.tcnet.ne.jp>
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
/* $Id: joystick_sdl.c,v 1.3 2000/11/25 13:09:08 chikama Exp $ */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>

#include "portab.h"
#include "joystick.h"
#include "sdl_core.h"

static boolean joy_enable=TRUE;
static SDL_Joystick *js=NULL;

void joy_set_devicename(char *name) {

}

int joy_open(void) {
#ifdef USE_JOYSTICK
	js=SDL_JoystickOpen(0);
	if(js){
		SDL_JoystickEventState(ENABLE_SDL);
		joy_enable=TRUE;
		return 1;
	}
#endif
	joy_enable=FALSE;
	return -1;
}

void joy_close(void) {
	SDL_JoystickClose(js);
}

int joy_getinfo(void) {
	//if (joy_enable) {
	//	return sdl_getjoyinfo();
	//}
	return 0;
}

int joy_getaxis(int axis){
#ifdef USE_JOYSTICK
	if(joy_enable)
		return SDL_JoystickGetAxis(js, axis);
	else
#endif
		return 0;
}