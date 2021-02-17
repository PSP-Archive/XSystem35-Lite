/*
 * audio.c  audio acesss wrapper
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
*/
/* $Id: audio.c,v 1.17 2003/01/25 01:34:50 chikama Exp $ */


#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "audio.h"


/* extern variable */
static char*  audio_dev_dsp;
static char*  audio_dev_mixer;

/* static variable */
static boolean mode_onlyone = FALSE; /* if true, only probe selected device */

#ifdef DEFAULT_AUDIO_MODE
static int mode = DEFAULT_AUDIO_MODE;
#else 
static int mode = AUDIO_PCM_ANY;
#endif

int audio_init(audiodevice_t *a) {
	switch(mode) {
	case AUDIO_PCM_ANY:

#ifdef ENABLE_SDL
	case AUDIO_PCM_SDL:
		// if (sdlaudio_initilize(a) == 0) break;
		// if (audiomode_strict) break;
#endif

	default:
		return -1;
	}
	return OK;
}

void audio_set_output_device(char c) {
	switch(c) {
	case '0':
		/* no audio */
		mode = AUDIO_PCM_DMY;
		break;
	default:
		mode = AUDIO_PCM_ANY;
	}
	
	mode_onlyone = TRUE;
}

void audio_set_pcm_devicename(char *name) {
	if (0 == strcmp("none", name)) {
		mode = AUDIO_PCM_DMY;
	} else {
		audio_dev_dsp = strdup(name);
	}
}

void audio_set_mixer_devicename(char *name) {
        audio_dev_mixer = strdup(name);
}
