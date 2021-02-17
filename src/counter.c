/*
 * counter.c  内部カウンタ
 *
 * Copyright (C) 1997-1998 Masaki Chikama (Wren) <chikama@kasumi.ipl.mech.nagoya-u.ac.jp>
 *               1998-                           <masaki-c@is.aist-nara.ac.jp>
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
 *
*/
/* $Id: counter.c,v 1.10 2000/11/25 13:09:03 chikama Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include "counter.h"

#include <SDL.h>

#define HICOUNTER_MAX (256 + EXTER_HIGHTCOUNTER_NUM)
//static struct timeval tv_base;
static long tv_base;
static int  counter_init = 0;

static int  counter_init_high[HICOUNTER_MAX];
static long tv_high[HICOUNTER_MAX];
static int  division_high[HICOUNTER_MAX] = { [0 ...( HICOUNTER_MAX-1 )]=1};

int get_counter(int division) {
	long usec2;
	usec2 = SDL_GetTicks();
	return counter_init + (int)(usec2 / division);
}

/* カウンタ〜を valでリセット */
/* 初期化時に一度呼んでおく */
void reset_counter(int val) {
	counter_init = val;
	tv_base      = SDL_GetTicks();
}


int get_high_counter(int num) {
	long usec2;
	long tv_base = tv_high[num -1];
	int division = division_high[num -1];

	usec2 = SDL_GetTicks() /division;
	return counter_init_high[num -1] + usec2;
}

/* 高精度カウンタ〜 thanx tajiri@wizard */
/* カウンタ〜を valでリセット */
/* 初期化時に一度呼んでおく */
void reset_counter_high(int num,int division,int val) {
	if (num == 0) {
		int i;
		for (i = 0; i < 256; i++) {
			counter_init_high[i] = val;
			tv_high[i] = SDL_GetTicks();
			division_high[i] = division;
		}
	} else {
		counter_init_high[num -1] = val;
		tv_high[num -1] = SDL_GetTicks();
		division_high[num -1] = division;
	}
}


