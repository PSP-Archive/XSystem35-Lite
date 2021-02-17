/*
 * selection.h  ����
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
/* $Id: selection.h,v 1.14 2002/05/07 21:43:53 chikama Exp $ */

#ifndef __SELECTION__
#define __SELECTION__

#include "portab.h"
#include "windowframe.h"

extern void sel_init();
extern void sel_reduce(int no);
extern int  sel_getnumberof();
extern char *sel_gettext(int no);
extern void sel_goto(int no, int flag);
extern void sel_returengoto(int no, int flag);
extern void sel_setDefaultElement(int type);
extern void sel_setCallback(int type, int page, int adr);
extern int  sel_getLastElement();
extern int  sel_getRegistoredElementNumber();
extern int  sel_getRegistoredElementWidth();
extern int  sel_getRegistoredElement_strlen();
extern void sel_setFontSize(int size);
extern void sel_addElement(const char *str);
extern void sel_addRetValue(int val);
extern void sel_fixElement();
extern void sel_select();

struct _selection {
	/* Window�Ȥμ��� */
	int WindowFrameType;
	int FrameCgNoTop;
	int FrameCgNoMid;
	int FrameCgNoBot;
	int Framedot;
	
        /* ��å������ե���Ȥ��礭�� */
	int MsgFontSize;
	
	/* �Ƽ￧ */
	int MsgFontColor;
	int WinFrameColor;
	int WinBackgroundColor;
	int WinElementEncloseColor;
	int SelectedElementColor;
	int WinBackgroundTransparent;
	int EncloseType;
	
	/* ���򤷤����ȥ�å������ΰ���������뤫 */
	boolean ClearMsgWindow;
	
	/* ����Window���礭�����ѹ� */
	boolean WinResizeWidth;
	boolean WinResizeHeight;
	
	/* ����� Window */
	int              winno;  
	Bcom_WindowInfo *win; // ���߻������ window
	Bcom_WindowInfo  wininfo[SELWINMAX];
	
        /* ��������Ͽ�� */
	boolean in_setting;
};
typedef struct _selection sel_t;

#endif /* __SELECTION__ */
