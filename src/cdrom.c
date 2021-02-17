/*
 * cdrom.c  CD-ROM control wrapper
 *
 * Copyright (C) 2000-  Masaki Chikama (Wren) <masaki-c@is.aist-nara.ac.jp>
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
/* $Id: cdrom.c,v 1.20 2002/08/18 09:35:29 chikama Exp $ */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "portab.h"
#include "cdrom.h"
#include "system.h"

#if defined(ENABLE_CDROM_LINUX)
extern cdromdevice_t cdrom_linux;
#define DEV_PLAY_MODE &cdrom_linux

#elif defined(ENABLE_CDROM_BSD)
extern cdromdevice_t cdrom_bsd;
#define DEV_PLAY_MODE &cdrom_bsd

#elif defined(ENALE_CDROM_IRIX)
extern cdromdevice_t cdrom_irix;
#define DEV_PLAY_MODE &cdrom_irix

#else

extern cdromdevice_t cdrom_empty;
#define DEV_PLAY_MODE &cdrom_empty
#endif

#ifdef ENABLE_CDROM_MP3
extern cdromdevice_t cdrom_mp3;
#endif


/*
  temporary cdrom device name
   
  default ... /dev/cdrom
  
  Irix(O2) ... /dev/scsi/sc1d4l0 
  FreeBSD  ... /dev/acd0a
   etc...
*/
static char *dev = CDROM_DEVICE;


/*
  初期化
    dev: cdromdevice に関する情報を格納する場所
    RET: 成功  0
         失敗 -1
*/
int cd_init(cdromdevice_t *cd) {

	return NG;
}

void cd_set_devicename(char *name) {

}
