/*
 * cmd2F.c  SYSTEM35 0x2f,xx command
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
/* $Id: cmd2F.c,v 1.35 2006/04/21 16:39:02 chikama Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "portab.h"
#include "xsystem35.h"
#include "savedata.h"
#include "menu.h"
#include "selection.h"
#include "message.h"
#include "music_client.h"
#include "eucsjis.h"
#include "ags.h"
#include "graphicsdevice.h"
#include "ald_manager.h"
#include "LittleEndian.h"

/* 選択 Window OPEN 時 callback */
static int cb_sel_init_page = 0;
static int cb_sel_init_address = 0;

extern INPUTSTRING_PARAM mi_param;

extern void commandH();
extern void commandHH();

typedef struct {
	int page;
	int index;
} fncall_table;

#define FCTBL_MAX 1024
static fncall_table fnctbl[FCTBL_MAX];

void commands2F00() {
	/* テキストカラーをスタックからポップして設定する */
	int data[3];
	
	sl_popData(data, 3);
	if (data[0] == TxxTEXTCOLOR) {
		if (data[1] == 0) {
			nact->msg.MsgFontColor = data[2];
		} else {
			nact->sel.MsgFontColor = data[2];
		}
	}
	
	DEBUG_COMMAND("TOC:\n");
}

void commands2F01() {
	/* テキストフォントサイズをスタックからポップして設定する */
	int data[3];
	
	sl_popData(data, 3);
	if (data[0] == TxxTEXTSIZE) {
		if (data[1] == 0) {
			nact->msg.MsgFontSize = data[2];
		} else {
			nact->sel.MsgFontSize = data[2];
		}
	}
	
	DEBUG_COMMAND("TOS:\n");
}

void commands2F02() {
	/* 現在のテキストカラーをスタックにプッシュする */
	int exp = getCaliValue();
	int data[] = {TxxTEXTCOLOR,0,0};
	
	data[1] = exp;
	data[2] = (exp == 0 ? nact->msg.MsgFontColor : nact->sel.MsgFontColor);
	sl_pushData(data, 3);
	
	DEBUG_COMMAND("TPC %d\n", exp);
}

void commands2F03() {
	/* 現在のテキストフォントサイズをスタックにプッシュする */
	int exp = getCaliValue();
	int data[] = {TxxTEXTSIZE, 0, 0};
	
	data[1] = exp;
	data[2] = (exp == 0 ? nact->msg.MsgFontSize : nact->sel.MsgFontSize);
	sl_pushData(data, 3);
	
	DEBUG_COMMAND("TPS %d:%s\n", exp, "");
}

void commands2F04() {
	/* テキスト表示位置をスタックからポップして設定する */
	int data[3];
	
	sl_popData(data, 3);
	if (data[0] == TxxTEXTLOC) {
		msg_setMessageLocation(data[1], data[2]);
	}
	
	DEBUG_COMMAND("TOP:\n");
}

void commands2F05() {
	/* 現在のテキスト表示位置をスタックにプッシュする */
	int data[] = {TxxTEXTLOC, 0, 0};
	MyPoint loc;
	
	msg_getMessageLocation(&loc);
	data[1] = loc.x;
	data[2] = loc.y;
	sl_pushData(data, 3);
	
	DEBUG_COMMAND("TPP:\n");
}

void commands2F08() {
	/* アンチエイリアシング付きテキスト描画のフラグ設定 */
	int exp = getCaliValue();
	
	ags_setAntialiasedStringMode(exp == 1);
	
	DEBUG_COMMAND("TAA %d:\n", exp);
}

void commands2F09() {
	/* アンチエイリアシング付きテキスト描画のフラグ取得 */
	int *var = getCaliVariable();

	*var = ags_getAntialiasedStringMode() ? 1 : 0;
	
	DEBUG_COMMAND("TAB %d:\n", *var);
}

void commands2F0A() {
	/* Wavデータを読み込む */
	int eCh = getCaliValue();
	int eLinkNum = getCaliValue();

	mus_wav_load(eCh, eLinkNum);
	
	DEBUG_COMMAND("wavLoad %d, %d:\n", eCh, eLinkNum);
}

void commands2F0B() {
	/* Wavを再生する */
	int eCh = getCaliValue();
	int eLoopFlag = getCaliValue();

	mus_wav_play(eCh, eLoopFlag);
	
	DEBUG_COMMAND("wavPlay %d, %d\n", eCh, eLoopFlag);
}

void commands2F0C() {
	int eCh = getCaliValue();
	
	mus_wav_stop(eCh);
	
	DEBUG_COMMAND("wavStop %d\n", eCh);
}

void commands2F0D() {
	int eCh = getCaliValue();
	
	mus_wav_unload(eCh);
	
	DEBUG_COMMAND("wavUnload %d:\n", eCh);
}

void commands2F0E() {
	int eCh = getCaliValue();
	int *vResult = getCaliVariable();
	int pos = mus_wav_get_playposition(eCh);
	
	*vResult = ((pos == 0 || pos == 65535) ? 0 : 1);
	
	DEBUG_COMMAND("wavIsPlay %d, %d:\n", eCh, *vResult);
}

void commands2F0F() {
	int eCh       = getCaliValue();
	int eTime     = getCaliValue();
	int eVolume   = getCaliValue();
	int eStopFlag = getCaliValue();

	mus_wav_fadeout_start(eCh, eTime, eVolume, eStopFlag);
	
	DEBUG_COMMAND("wavFade %d, %d, %d, %d:\n", eCh, eTime, eVolume, eStopFlag);
}

void commands2F10() {
	int eCh = getCaliValue();
	int *vResult = getCaliVariable();
	
	*vResult = (mus_wav_fadeout_get_state(eCh) ? 1 : 0);
	
	DEBUG_COMMAND("wavIsFade %d, %d:\n", eCh, *vResult);
}

void commands2F11() {
	int eCh = getCaliValue();

	mus_wav_fadeout_stop(eCh);
	
	DEBUG_COMMAND("wavStopFade %d:\n", eCh);
}

void commands2F12() {
	char *sText = sys_getString(0);
	
	DEBUG_COMMAND("trace %s:\n", sText);

}

void commands2F13() {
	int eCh = getCaliValue();
	int eX = getCaliValue();
	int eY = getCaliValue();
	int eZ = getCaliValue();

	DEBUG_COMMAND_YET("wav3DSetPos %d, %d, %d, %d:\n", eCh, eX, eY, eZ);
}

void commands2F14() {
	DEBUG_COMMAND_YET("wav3DCommit:\n");
}

void commands2F15() {
	int eCh = getCaliValue();
	int *eX = getCaliVariable();
	int *eY = getCaliVariable();
	int *eZ = getCaliVariable();

	*eX = 0;
	*eY = 0;
	*eZ = 0;
	DEBUG_COMMAND_YET("wav3DGetPos %d, %d, %d, %d:\n", eCh, *eX, *eY, *eZ);
}

void commands2F16() {
	int eX = getCaliValue();
	int eY = getCaliValue();
	int eZ = getCaliValue();
	
	DEBUG_COMMAND_YET("wav3DSetPosL %d, %d, %d:\n", eX, eY, eZ);
}

void commands2F17() {
	int *vX = getCaliVariable();
	int *vY = getCaliVariable();
	int *vZ = getCaliVariable();
	
	*vX = 0;
	*vY = 0;
	*vZ = 0;
	DEBUG_COMMAND_YET("wav3DGetPosL %d, %d, %d:%s\n", *vX, *vY, *vZ);
}

void commands2F18() {
	int eCh = getCaliValue();
	int eTime = getCaliValue();
	int eX = getCaliValue();
	int eY = getCaliValue();
	int eZ = getCaliValue();

	DEBUG_COMMAND_YET("wav3DFadePos %d, %d, %d, %d, %d:\n", eCh, eTime, eX, eY, eZ);
}

void commands2F19() {
	int eCh = getCaliValue();
	int *vResult = getCaliVariable();

	*vResult = 0;
	DEBUG_COMMAND_YET("wav3DIsFadePos %d, %d:\n", eCh, *vResult);
}

void commands2F1A() {
	int eCh = getCaliValue();

	DEBUG_COMMAND_YET("wav3DStopFadePos %d:\n", eCh);
}

void commands2F1B() {
	int eTime = getCaliValue();
	int eX = getCaliValue();
	int eY = getCaliValue();
	int eZ = getCaliValue();

	DEBUG_COMMAND_YET("wav3DFadePosL %d, %d, %d, %d:\n", eTime, eX, eY, eZ);
}

void commands2F1C() {
	int *vResult = getCaliVariable();
	
	*vResult = 0;
	DEBUG_COMMAND_YET("wav3DIsFadePosL %d:\n", *vResult);
}

void commands2F1D() {
	DEBUG_COMMAND_YET("wav3DStopFadePosL");
}

void commands2F1E() {
	int eLinkNum = getCaliValue();
	int eLoop = getCaliValue();

	if (eLinkNum > 0) {
		mus_pcm_start(eLinkNum , eLoop);
	}
	
	DEBUG_COMMAND("sndPlay %d, %d:\n", eLinkNum, eLoop);
}

void commands2F1F() {
	mus_pcm_stop(0);
	
	DEBUG_COMMAND("sndStop:\n");
}

void commands2F20() {
	int *vResult = getCaliVariable();
	
	mus_pcm_get_playposition(vResult);
	
	DEBUG_COMMAND("sndIsPlay %d:\n", *vResult);
}

void commands2F21() {
	char *str = sys_getString(0);
	
	sys_addMsg(str);
}

void commands2F23() {
	int x = getCaliValue();
	int y = getCaliValue();
	char *vFileName = sys_getString(0);
	
	sysVar[0] = cg_load_with_filename(vFileName, x, y);
	
	DEBUG_COMMAND("LC(new) %d, %d, %s:\n", x, y, vFileName);
}

void commands2F24() {
	int type       = sys_getc();
	char *file_name = sys_getString(0);
	int *var = NULL, _var = 0;
	int num = 0;

	switch(type) {
	case 0:
		var = getCaliVariable();
		num  = getCaliValue();
#if !defined(PSP)
		sysVar[0] = save_load_var_with_file(file_name, var, num); 
#endif
		break;
	case 1:
		_var = getCaliValue();
		num  = getCaliValue();
#if !defined(PSP)
		sysVar[0] = save_load_str_with_file(file_name, _var, num);
#endif
		break;
	}
	
	DEBUG_COMMAND("LE(new) %d, %s, %d, %d:\n", type, file_name, var, num);
}

void commands2F25() {
	int file_name = getCaliValue();
	char *title = sys_getString(0);
	char *filter = sys_getString(0);

	DEBUG_COMMAND_YET("LXG %d, %s, %s:\n", file_name, title, filter);
}

void commands2F26() {
	int dst_no  = getCaliValue();
	int max_len = getCaliValue();
	char *title  = sys_getString(0);
	char *t1, *t2, *t3;
	
	t1 = sjis2lang(title);
	t2 = sjis2lang(v_str(dst_no -1));
	
	mi_param.title = t1;
	mi_param.oldstring = t2;
	mi_param.max = max_len;
	
	menu_inputstring(&mi_param);
	if (mi_param.newstring == NULL) {
		v_strcpy(dst_no -1, NULL);
		free(t1); free(t2);
		return;
	}
	
	t3 = lang2sjis(mi_param.newstring);
	
	/* 全角文字以外は不可 */
	if (!sjis_has_hankaku(t3)) {
		v_strcpy(dst_no -1, t3);
	}
	free(t1);
	free(t2);
	free(t3);
	
	DEBUG_COMMAND("MI(new) %d, %d, %s:\n", dst_no, max_len, title);
}

void commands2F27() {
	int   num    = getCaliValue();
	char *string = sys_getString(0);

	if (num > 0) {
	        v_strcpy(num - 1, string);
	} else {
        	WARNING("MS: num <= 0\n");
	}
	
	DEBUG_COMMAND("MS(new) %d, %s:\n", num, string);
}

void commands2F28() {
	char *title = sys_getString(0);
	
	strncpy(nact->game_title_name, title, 30);
	ags_setWindowTitle(title);
	
	DEBUG_COMMAND("MT(new) %s:\n",title);
}

/* defined in cmdn.c */
extern INPUTNUM_PARAM ni_param;
void commands2F29() {
	char *title = sys_getString(0);
	char *t;
	
	if (ni_param.title != NULL) {
		free(ni_param.title);
	}
	t = sjis2lang(title);
	ni_param.title = t;
	
	DEBUG_COMMAND("NT(new) %s:\n", title);
}

void commands2F2A() {
	int type = sys_getc();
	char *file_name = sys_getString(0);
	int *var, _var = 0, cnt;

	switch(type) {
	case 0:
		var = getCaliVariable();
		cnt = getCaliValue();
#if !defined(PSP)
		sysVar[0] = save_save_var_with_file(file_name, var, cnt);
#endif
		break;
	case 1:
		_var = getCaliValue();
		cnt  = getCaliValue();
#if !defined(PSP)
		sysVar[0] = save_save_str_with_file(file_name, _var, cnt);
#endif
		break;
	default:
		WARNING("Unknown QE command\n"); return;
	}

	DEBUG_COMMAND("QE(new) %d, %s, %d, %d:\n", type, file_name, _var, cnt);
}

void commands2F2B() {
	int type = sys_getc();
	char *work_dir = sys_getString(0);
	char *file_name = sys_getString(0);

	DEBUG_COMMAND_YET("UP(new) %d, %s, %s:\n", type, work_dir, file_name);
}

void commands2F2D() {
	int eCh = getCaliValue();
	int eTime = getCaliValue();
	
	/* 該当する eCh が演奏中の場合 eTime だけ wait をいれる */
	mus_wav_waittime(eCh, eTime);
	
	DEBUG_COMMAND("wavWaitTime %d, %d:\n", eCh, eTime);
}

void commands2F2E() {
	int eCh = getCaliValue();
	int *vTime = getCaliVariable();
	
	*vTime = mus_wav_get_playposition(eCh);
	
	DEBUG_COMMAND("wavGetPlayPos %d, %d:\n", eCh, *vTime);
}

void commands2F2F() {
	int eCh = getCaliValue();
	
	/* 該当する eCh が演奏中の場合、終了まで待つ  */
	/* ただし、無限ループの場合は１回目終了時まで */
	mus_wav_waitend(eCh);
	
	DEBUG_COMMAND("wavWaitEnd %d:\n", eCh);
}

void commands2F30() {
	int eCh = getCaliValue();
	int *vTime = getCaliVariable();
	
	*vTime = mus_wav_wavtime(eCh);
	
	DEBUG_COMMAND("wavGetWavTime %d, %d:\n", eCh, *vTime);
}

void commands2F31() {
	int fPage = sys_getw();
	int fIndex = sys_getaddress();
	
	sel_setCallback(1, fPage, fIndex);
	DEBUG_COMMAND("menuSetCbkSelect page=%d, index=%x:\n", fPage, fIndex);
}

void commands2F32() {
	int fPage = sys_getw();
	int fIndex = sys_getaddress();
	
	sel_setCallback(2, fPage, fIndex);
	DEBUG_COMMAND("menuSetCbkCancel page=%d, index=%x:\n", fPage, fIndex);
}

void commands2F33() {
	sel_setCallback(1, 0, 0);
	DEBUG_COMMAND("menuClearCbkSelect:\n");
}

void commands2F34() {
	sel_setCallback(2, 0, 0);
	DEBUG_COMMAND("menuClearCbkCancel:\n");
}

void commands2F35() {
	int eCh = getCaliValue();
	int eMode = getCaliValue();
	
	DEBUG_COMMAND_YET("wav3DSetMode %d, %d:\n", eCh, eMode);
}

void commands2F36() {
	int eDestX     = getCaliValue();
	int eDestY     = getCaliValue();
	int eDestWidth  = getCaliValue();
	int eDestHeight = getCaliValue();
	int eSrcX      = getCaliValue();
	int eSrcY      = getCaliValue();
	int eSrcWidth  = getCaliValue();
	int eSrcHeight = getCaliValue();
	int eMode      = getCaliValue();
	
	ags_scaledCopyArea(eSrcX, eSrcY, eSrcWidth, eSrcHeight,
			   eDestX, eDestY, eDestWidth, eDestHeight, 0);
	ags_updateArea(eDestX, eDestY, eDestWidth, eDestHeight);
	
	DEBUG_COMMAND("grCopyStrerch %d, %d, %d, %d, %d, %d, %d, %d, %d:\n",
		      eDestX, eDestY, eDestWidth, eDestHeight,
		      eSrcX, eSrcY, eSrcWidth, eSrcHeight, eMode);
}

void commands2F37() {
	int eX      = getCaliValue();
	int eY      = getCaliValue();
	int eWidth  = getCaliValue();
	int eHeight = getCaliValue();
	int eType   = getCaliValue();
	
	DEBUG_COMMAND_YET("grFilterRect %d, %d, %d, %d, %d:\n",
			  eX, eY, eWidth, eHeight, eType);
}

void commands2F38() {
	DEBUG_COMMAND("iptClearWheelCount:\n");
}

void commands2F39() {
	int *vForward = getCaliVariable();
	int *vBack = getCaliVariable();
	
	*vForward = 0;
	*vBack = 0;
	DEBUG_COMMAND("iptGetWheelCount %d, %d:\n", *vForward, *vBack);
}

void commands2F3A() {
	int *vSize = getCaliVariable();

	*vSize = nact->sel.MsgFontSize;
	DEBUG_COMMAND("menuGetFontSize %d:\n", *vSize);
}

void commands2F3B() {
	int *vSize = getCaliVariable();
	
	*vSize = nact->msg.MsgFontSize;
	DEBUG_COMMAND("msgGetFontSize %d:\n", *vSize);
}	

void commands2F3C() {
	int eNum = getCaliValue();
	int ePos = getCaliValue();
	int *vResult = getCaliVariable();
	
	if (eNum <= 0 || v_strlen(eNum -1) < ePos) {
		*vResult = 0;
	} else {
		BYTE b1, b2 = 0;
		b1 = *(v_str(eNum - 1) + ePos);
		if (ePos == 0) {
			*vResult = CHECKSJIS1BYTE(b1) ? 2 : 1;
		} else {
			b2 = *(v_str(eNum - 1) + ePos -1);
			if (CHECKSJIS1BYTE(b1)) {
				*vResult = 2;
			} else {
				*vResult = CHECKSJIS1BYTE(b2) ? 2 : 1;
			}
		}
	}
	
	DEBUG_COMMAND("strGetCharType %d, %d, %d:\n", eNum, ePos, *vResult);
}

void commands2F3D() {
	int eNum = getCaliValue();
	int *vResult = getCaliVariable();
	
	*vResult = v_strlen(eNum -1);

	DEBUG_COMMAND("strGetLengthASCII %d, %d:\n", eNum, *vResult);
}

void commands2F3E() {
	nact->is_message_locked = TRUE;
	DEBUG_COMMAND("sysWinMsgLock:\n");
}

void commands2F3F() {
	nact->is_message_locked = FALSE;
	DEBUG_COMMAND("sysWinMsgUnlock:\n");
}

void commands2F40() {
	int *vAry = getCaliVariable();
	int eCount = getCaliValue();
	int eNum = getCaliValue();
	int *vResult = getCaliVariable();
	int cnt = 0;
	
	DEBUG_COMMAND("aryCmpCount %d, %d, %d, %d:\n", *vAry, eCount, eNum, *vResult);
	
	while(eCount--) {
		if (*vAry == eNum) cnt++;
		vAry++;
	}
	*vResult = cnt;
}

void commands2F41() {
	int *vAry = getCaliVariable();
	int eNumof = getCaliValue();
	int eNum = getCaliValue();
	int eTransNum = getCaliValue();
	int eMaxTrans = getCaliValue();
	int *vResult = getCaliVariable();
	int cnt = 0;
	
	DEBUG_COMMAND("aryCmpTrans %d, %d, %d, %d, %d, %d:\n", 
		      *vAry, eNumof, eNum, eTransNum, eMaxTrans, *vResult);
	
	while(eNumof--) {
		if (*vAry == eNum) {
			*vAry = eTransNum;
			cnt++;
			if (cnt >= eMaxTrans) break;
		}
		vAry++;
	}
	*vResult = cnt;
}

void commands2F42() {
	int eDestX    = getCaliValue();
	int eDestY    = getCaliValue();
	int eWidth    = getCaliValue();
	int eHeight   = getCaliValue();
	int eDestRate = getCaliValue();
	int eSrcX     = getCaliValue();
	int eSrcY     = getCaliValue();
	int eSrcRate  = getCaliValue();
	int eType     = getCaliValue();

	DEBUG_COMMAND_YET("grBlendColorRect %d, %d, %d, %d, %d, %d, %d, %d, %d:\n",
			  eDestX, eDestY, eWidth, eHeight, eDestRate,
			  eSrcX, eSrcY, eSrcRate, eType);
}

void commands2F43() {
	int eX = getCaliValue();
	int eY = getCaliValue();
	int eLength = getCaliValue();
	int eColor  = getCaliValue();

	DEBUG_COMMAND_YET("grDrawFillCircle %d, %d, %d, %d:\n", eX, eY, eLength, eColor);
}

void commands2F44() {
	int num1 = getCaliValue();
	int fig  = getCaliValue();
	int num2 = getCaliValue();
	char s[256];

	if (fig) {
		char *ss="%%%dd";
		char sss[256];
		sprintf(sss, ss, fig);
		sprintf(s, sss, num2);
	} else {
		sprintf(s, "%d", num2);
	}
	v_strcpy(num1 - 1, s);
	
	DEBUG_COMMAND("MHH %d, %d, %d:\n", num1, fig, num2);
}

void commands2F45() {
	int fPage = sys_getw();
	int fIndex = sys_getaddress();
	
	cb_sel_init_page    = fPage;
	cb_sel_init_address = fIndex;
	
	DEBUG_COMMAND("menuSetCbkInit page=%d, index=%x:\n", fPage, fIndex);
}

void commands2F46() {
	cb_sel_init_page = 0;
	DEBUG_COMMAND("menuClearCbkInit:\n");
}

void commands2F47() {
	if (cb_sel_init_page != 0) {
		sl_jmpNear(sl_getIndex());
		sl_callFar2(cb_sel_init_page -1, cb_sel_init_address);
	}
	DEBUG_COMMAND("new ]:\n");
}

void commands2F48() {
	char *sText = sys_getString(0);
	
	DEBUG_COMMAND_YET("sysOpenShell %s:\n", sText);
}

void commands2F49() {
	char *sTitle = sys_getString(0);
	char *sUrl = sys_getString(0);
	
	DEBUG_COMMAND_YET("sysAddWebMenu %s, %s:\n", sTitle, sUrl);
}

void commands2F4A() {
	int eTime = getCaliValue();
	
	ags_setCursorMoveTime(eTime);
	DEBUG_COMMAND("iptSetMoveCursorTime %d:\n", eTime);
}

void commands2F4B() {
	int *vTime = getCaliVariable();
	
	*vTime = ags_getCursorMoveTime();
	DEBUG_COMMAND("iptGetMoveCursorTime %d:\n", *vTime);
}

void commands2F4C() {
	int eX      = getCaliValue();
	int eY      = getCaliValue();
	int eSrcX   = getCaliValue();
	int eSrcY   = getCaliValue();
	int eWidth  = getCaliValue();
	int eHeight = getCaliValue();

	MyRectangle r;
	MyPoint p;

	r.x = eSrcX; r.y = eSrcY; r.width = eWidth; r.height = eHeight;
	p.x = eX, p.y = eY;
	
	UpdateArea(&r, &p);
	
	DEBUG_COMMAND("grBlt %d, %d, %d, %d, %d, %d:\n",
		      eX, eY, eSrcX, eSrcY, eWidth, eHeight);
}

void commands2F4D() {
	int eNum = getCaliValue();
	char *sText = sys_getString(0);

	DEBUG_COMMAND_YET("LXWT %d, %s:\n", eNum, sText);
}

void commands2F4E() {
	int eNum = getCaliValue();
	int eTextNum = getCaliValue();

	DEBUG_COMMAND_YET("LXWS %d, %d:\n", eNum, eTextNum);
}

void commands2F4F() {
	int eNum = getCaliValue();
	int eType = getCaliValue();

	DEBUG_COMMAND_YET("LXWE %d, %d:\n", eNum, eType);
}

void commands2F50() {
	int eFile = getCaliValue();
	int nFlg = sys_getc();
	int eNum = getCaliValue();

	DEBUG_COMMAND_YET("LXWH %d, %d, %d:\n", eFile, nFlg, eNum);
}

void commands2F51() {
	int eFile = getCaliValue();
	int nFlg  = sys_getc();
	int eNum  = getCaliValue();

	DEBUG_COMMAND_YET("LXWHH %d, %d, %d:\n", eFile, nFlg, eNum);
}

void commands2F52() {
	int eNum = getCaliValue();
//	struct utsname un;
	char s[256];
	
	if (eNum <= 0) return;
/*
	if (uname(&un) < 0) return;
	NOTICE("sysname %s nodename %s release %s version %s machine %s\n",
	       un.sysname, un.nodename, un.release, un.version, un.machine);
	sprintf(s,"%s %s %s",
		un.sysname, un.release, un.machine);
	v_strcpy(eNum -1, s);
	
	DEBUG_COMMAND("sysGetOsName %d: %s\n", eNum, s);
*/
}

void commands2F53() {
	int eFlag = getCaliValue();
	
	nact->patch_ec = eFlag;
	
	DEBUG_COMMAND("patchEC %d:\n", eFlag);
}

void commands2F54() {
	int eX = getCaliValue();
	int eY = getCaliValue();
	int eWidth = getCaliValue();
	int eHeight = getCaliValue();
	
	DEBUG_COMMAND_YET("mathSetClipWindow %d, %d, %d, %d:\n", eX, eY, eWidth, eHeight);
}

void commands2F55() {
	int *vDx = getCaliVariable();
	int *vDy = getCaliVariable();
	int *vSx = getCaliVariable();
	int *vSy = getCaliVariable();
	int *vWidth  = getCaliVariable();
	int *vHeight = getCaliVariable();
	
	DEBUG_COMMAND_YET("mathClip %d, %d, %d, %d, %d, %d:\n",
			  *vDx, *vDy, *vSx, *vSy, *vWidth, *vHeight);
}

void commands2F56() {
	int file_name = getCaliValue();
	char *title = sys_getString(0);
	char *folder = sys_getString(0);

	DEBUG_COMMAND_YET("LXF %d, %s, %s:\n", file_name, title, folder);
}

void commands2F57() {
	char *sTitle = sys_getString(0);
	int eStrVar = getCaliValue();
	int eLength = getCaliValue();
	int *vResult = getCaliVariable();

	char *t1, *t2, *t3;
	INPUTSTRING_PARAM p;
	
	t1 = sjis2lang(sTitle);
	t2 = sjis2lang(v_str(eStrVar -1));
	p.title = t1;
	p.oldstring = t2;
	p.max = eLength;
	
	menu_inputstring(&p);
	if (p.newstring == NULL) {
		*vResult = 65535;
	} else {
		t3 = lang2sjis(p.newstring);
		if (!sjis_has_hankaku(t3)) {
			v_strcpy(eStrVar -1, t3);
			*vResult = sjis_count_char(t3);
		} else {
			*vResult = 65535;
		}
		free(t3);
	}
	free(t1);
	free(t2);
	DEBUG_COMMAND("strInputDlg %s, %d, %d, %d:\n",
		      sTitle, eStrVar, eLength, *vResult);
}

void commands2F58() {
	int eNum = getCaliValue();
	int *vResult = getCaliVariable();
	
	*vResult = sjis_has_hankaku(v_str(eNum) -1) ? 1 : 0;
	
	DEBUG_COMMAND("strCheckASCII %d, %d:\n", eNum, *vResult);
}

void commands2F59() {
	int eNum = getCaliValue();
	int *vResult = getCaliVariable();
	
	*vResult = sjis_has_zenkaku(v_str(eNum) -1) ? 1 : 0;
	
	DEBUG_COMMAND("strCheckSJIS %d, %d:\n", eNum, *vResult);
}

void commands2F5A() {
	char *sText = sys_getString(0);
	char *t1;
	
	t1 = sjis2lang(sText);
	menu_msgbox_open(t1);
	free(t1);
	
	DEBUG_COMMAND("strMessageBox %s:\n", sText);
}

void commands2F5B() {
	int eNum = getCaliValue();
	char *t1;
	
	t1 = sjis2lang(v_str(eNum) - 1);
	menu_msgbox_open(t1);
	free(t1);
	
	DEBUG_COMMAND("strMessageBoxStr %d:\n", eNum);
}

void commands2F5C() {
	int eDestX = getCaliValue();
	int eDestY = getCaliValue();
	int eSrcX = getCaliValue();
	int eSrcY = getCaliValue();
	int eWidth = getCaliValue();
	int eHeight = getCaliValue();
	int eAlpha = getCaliValue();
	
	ags_copyArea_shadow_withrate(eSrcX, eSrcY, eWidth, eHeight, eDestX, eDestY, eAlpha);
	DEBUG_COMMAND("grCopyUseAMapUseA %d, %d, %d, %d, %d, %d, %d:\n",
		      eDestX, eDestY, eSrcX, eSrcY, eWidth, eHeight, eAlpha);
}

void commands2F5D() {
	int eNum = getCaliValue();
	int eData = getCaliValue();
	
	DEBUG_COMMAND_YET("grSetCEParam %d, %d:\n", eNum, eData);
}

void commands2F5E() {
	int eX = getCaliValue();
	int eY = getCaliValue();
	int eTime = getCaliValue();
	int eFlag = getCaliValue();
	
	DEBUG_COMMAND_YET("grEffectMoveView %d, %d, %d, %d:\n", eX, eY, eTime, eFlag);
}

void commands2F5F() {
	int eSize = getCaliValue();
	
	DEBUG_COMMAND_YET("cgSetCacheSize %d:\n", eSize);
}

void commands2F61() {
	int eChar = getCaliValue();
	int eCGNum = getCaliValue();
	
	DEBUG_COMMAND_YET("gaijiSet %d, %d:\n", eChar, eCGNum);
}

void commands2F62() {
	DEBUG_COMMAND_YET("gaijiClearAll:\n");
}

void commands2F63() {
	int *vSelect = getCaliVariable();
	
	*vSelect = sel_getLastElement();

	DEBUG_COMMAND("menuGetLatestSelect %d:\n", *vSelect);
}

void commands2F64() {
	int eType = getCaliValue();
	int eNum  = getCaliValue();
	int *vResult = getCaliVariable();
	DRIFILETYPE t = DRIFILE_DATA;
	dridata *dfile;
	
	switch(eType) {
	case 0:
		t = DRIFILE_DATA;
		break;
	case 1:
		t = DRIFILE_CG;
		break;
	case 2:
		t = DRIFILE_MIDI;
		break;
	case 3:
		t = DRIFILE_SCO;
		break;
	case 4:
		t = DRIFILE_WAVE;
		break;
	case 5:
		t = DRIFILE_RSC;
		break;
	}
	
	dfile = ald_getdata(t, eNum -1);
	if (dfile == NULL) {
		*vResult = 0;
	} else {
		*vResult = 1;
	}
	
	ald_freedata(dfile);
	
	DEBUG_COMMAND("lnkIsLink %d, %d, %d:\n", eType, eNum, *vResult);
}

void commands2F65() {
	int eType = getCaliValue();
	int eNum  = getCaliValue();
	int *vResult = getCaliVariable();
	
	DRIFILETYPE t = DRIFILE_DATA;
	dridata *dfile;
	
	switch(eType) {
	case 0:
		t = DRIFILE_DATA;
		break;
	case 1:
		t = DRIFILE_CG;
		break;
	case 2:
		t = DRIFILE_MIDI;
		break;
	case 3:
		t = DRIFILE_SCO;
		break;
	case 4:
		t = DRIFILE_WAVE;
		break;
	case 5:
		t = DRIFILE_RSC;
		break;
	}
	
	dfile = ald_getdata(t, eNum -1);
	if (dfile == NULL) {
		*vResult = 0;
	} else {
		*vResult = 1;
	}
	
	ald_freedata(dfile);
	
	DEBUG_COMMAND("lnkIsData %d, %d, %d:\n", eType, eNum, *vResult);
}

void commands2F66() {
	int eNum = getCaliValue();
	int page  = sys_getw();
	int index = sys_getdw();
	
	DEBUG_COMMAND("fncSetTable %d, %d,%d:\n", eNum, page, index);

	if (eNum >= FCTBL_MAX) {
		WARNING("fncSetTable: too many table number %d\n", eNum);
		return;
	}
	
	fnctbl[eNum].page  = page;
	fnctbl[eNum].index = index;
}

void commands2F67() {
	int eNum = getCaliValue();
	int eStrNum = getCaliValue();
	int *vResult = getCaliVariable();
	int i;

	for (i = 0; i < nact->ain.fncnum; i++) {
		if (0 == strcmp(nact->ain.fnc[i].name, v_str(eStrNum -1))) {
			fnctbl[eNum].page  = nact->ain.fnc[i].page;
			fnctbl[eNum].index = nact->ain.fnc[i].index;
			break;
		}
	}
	
	*vResult = (i == nact->ain.fncnum) ? 0 : !0;
	
	DEBUG_COMMAND("fncSetTableFromStr %d, %d, %d:\n", eNum, eStrNum, *vResult);
}

void commands2F68() {
	int eNum = getCaliValue();
	
	DEBUG_COMMAND("fncClearTable %d:\n", eNum);
	
	if (eNum >= FCTBL_MAX) {
		WARNING("fncClearTable: too many table number %d\n", eNum);
		return;
	}
	
	fnctbl[eNum].page  = 0;
	fnctbl[eNum].index = 0;
}

void commands2F69() {
	int eNum = getCaliValue();
	
	DEBUG_COMMAND("fncCall %d:\n", eNum);
	
	if (eNum >= FCTBL_MAX) {
		WARNING("fncClearTable: too many table number %d\n", eNum);
		return;
	}
	
	if (fnctbl[eNum].page == 0 && fnctbl[eNum].index == 0) {
		return;
	}
	
	sl_callFar2(fnctbl[eNum].page -1, fnctbl[eNum].index);
}

void commands2F6A() {
	int eNum = getCaliValue();

	nact->fnc_return_value = eNum;
	
	DEBUG_COMMAND("fncSetReturnCode %d:\n", eNum);
}

void commands2F6B() {
	int *vNum = getCaliVariable();
	
	*vNum = nact->fnc_return_value;
	
	DEBUG_COMMAND("fncGetReturnCode %d:\n", *vNum);
}

void commands2F6C() {
	int eFlag = getCaliValue();
	
	nact->is_msg_out = eFlag != 0 ? TRUE : FALSE;
	
	DEBUG_COMMAND("msgSetOutputFlag %d:\n", eFlag);
}

void commands2F6D() {
	int eNum = getCaliValue();
	int *vResult = getCaliVariable();

#if !defined(PSP)
	*vResult = save_delete_file(eNum -1);
#endif
	
	DEBUG_COMMAND("saveDeleteFile %d, %d:\n", eNum, *vResult);
}

void commands2F6E() {
	int eFlag = getCaliValue();
	
	DEBUG_COMMAND_YET("wav3DSetUseFlag %d:\n", eFlag);
}

void commands2F6F() {
	int eCh       = getCaliValue();
	int eTime     = getCaliValue();
	int eVolume   = getCaliValue();
	int eStopFlag = getCaliValue();
	
	mus_wav_fadeout_start(eCh, eTime, eVolume, eStopFlag);
	
	DEBUG_COMMAND("wavFadeVolume %d, %d, %d, %d:\n", eCh, eTime, eVolume, eStopFlag);
}

void commands2F70() {
	int eFlag = getCaliValue();
	
	nact->patch_emen = eFlag;
	
	DEBUG_COMMAND("patchEMEN %d:\n", eFlag);
}

void commands2F71() {
	int eFlag = getCaliValue();
	
	DEBUG_COMMAND_YET("wmenuEnableMsgSkip %d:\n", eFlag);
}

void commands2F72() {
	int *vFlag = getCaliVariable();
	
	DEBUG_COMMAND_YET("winGetFlipFlag %p:\n", vFlag);
}

void commands2F73() {
	int *vTrack = getCaliVariable();
	
	*vTrack = mus_cdrom_get_maxtrack();
	
	DEBUG_COMMAND("cdGetMaxTrack %d:\n", *vTrack);
}

void commands2F74() {
	char *str1 = sys_getString(0);
	int *var   = getCaliVariable();
	
	DEBUG_COMMAND_YET("dlgErrorOkCancel %s,%d:\n", str1, *var);
}

void commands2F75() {
	int eNumof = getCaliValue();
	
	sel_reduce(eNumof);
	
	DEBUG_COMMAND("menuReduce %d:\n", eNumof);
}

void commands2F76() {
	int *var   = getCaliVariable();

	*var = sel_getnumberof();
	
	//DEBUG_COMMAND("menuGetNumof %p:\n", var);
	DEBUG_COMMAND("menuGetNumof %d:\n", *var);
}

void commands2F77() {
	int eStrNum    = getCaliValue();
	int eSelectNum = getCaliValue();
	
	v_strcpy(eStrNum -1, sel_gettext(eSelectNum));
	
	DEBUG_COMMAND("menuGetText %d,%d:\n", eStrNum, eSelectNum);
}

void commands2F78() {
	int eSelectNum = getCaliValue();
	int eClearFlag = getCaliValue();

	sel_goto(eSelectNum, eClearFlag);
	
	DEBUG_COMMAND("menuGoto %d,%d:\n", eSelectNum, eClearFlag);
}

void commands2F79() {
	int eSelectNum = getCaliValue();
	int eClearFlag = getCaliValue();
	
	sel_returengoto(eSelectNum, eClearFlag);
	
	DEBUG_COMMAND("menuReturnGoto %d,%d:\n", eSelectNum, eClearFlag);
}

void commands2F7A() {
	DEBUG_COMMAND_YET("menuFreeShelterDIB:\n");
}

void commands2F7B() {
	DEBUG_COMMAND_YET("msgFreeShelterDIB:\n");
}

void commands2F7C() {
	int index = sys_getdw();
	
	sys_addMsg(nact->ain.msg[index]);
	
	DEBUG_COMMAND("2F7C %d:\n", index);
}

void commands2F7D() {
	int index = sys_getdw();
	
	commandH();
	sys_addMsg(nact->ain.msg[index]);
	
	// DEBUG_COMMAND_YET("2F7D %d, %d, %d:\n", index, page, p1);
	DEBUG_COMMAND("2F7D %d:\n", index);
}

void commands2F7E() {
	int index = sys_getdw();
	
	commandHH();
	sys_addMsg(nact->ain.msg[index]);
	
	//DEBUG_COMMAND_YET("2F7E %d, %d, %d:\n", index, p1, p2);
	DEBUG_COMMAND("2F7E %d:\n", index);
}

void commands2F7F() {
	int index = sys_getdw();
	int p1    = sys_getCaliValue();
	
	sys_addMsg(v_str(p1 -1));
	sys_addMsg(nact->ain.msg[index]);
	
	DEBUG_COMMAND("2F7F %d, %d(%s,%s):\n", index, p1, nact->ain.msg[index], v_str(p1 -1));
}

void commands2F80() {
	int page  = sys_getw();
	int index = sys_getdw();
	
	if (NULL == (nact->datatbl_addr = sl_setDataTable(page -1, index))) {
		WARNING("dataSetPointer set failed\n");
	}
	
	DEBUG_COMMAND("dataSetPointer %d,%x\n", page, index);
}

void commands2F81() {
	int *vData = sys_getCaliVariable();
	int eNumof = sys_getCaliValue();
	int i;
	WORD *p = (WORD *)nact->datatbl_addr;

	for (i = 0; i < eNumof; i++) {
		*vData = LittleEndian_getW((BYTE *)p, 0);
		p++;
		vData++;
	}
	
	nact->datatbl_addr = p;
	
	DEBUG_COMMAND("dataGetWORD %p,%d(%p):\n", vData, eNumof, nact->datatbl_addr);
}

void commands2F82() {
	int eStrNum = sys_getCaliValue();
	int eNumof  = sys_getCaliValue();
	int i;
	char *p = (char *)nact->datatbl_addr;
	
	DEBUG_COMMAND("dataGetString %d,%d:\n", eStrNum, eNumof);
	
	for (i = 0; i < eNumof; i++) {
		v_strcpy(eStrNum + i -1, p);
		p += (strlen(p) + 1);
	}

	nact->datatbl_addr = p;
}

void commands2F83() {
	int eNumof  = sys_getCaliValue();
	WORD *p = (WORD *)nact->datatbl_addr;

	p += eNumof;

	nact->datatbl_addr = p;
	
	DEBUG_COMMAND("dataSkipWORD %d:\n", eNumof);
}

void commands2F84() {
	int eNumof  = sys_getCaliValue();
	int i;
	char *p = (char *)nact->datatbl_addr;
	
	for (i = 0; i < eNumof; i++) {
		p += (strlen(p) + 1);
	}
	
	nact->datatbl_addr = p;
	
	DEBUG_COMMAND("dataSkipString %d:\n", eNumof);
}

void commands2F85() {
	int *vResult = sys_getCaliVariable();
	
	*vResult = SYSVAR_MAX;
	
	DEBUG_COMMAND("varGetNumof %p:\n", vResult);
}

void commands2F86() {
	int eFlag = sys_getCaliValue();
	
	nact->patch_g0 = eFlag;
	
	DEBUG_COMMAND("patchG0 %d:\n", eFlag);
}

void commands2F87() {
	int eSubKeyStrNum = sys_getCaliValue();
	int eBaneStrNum   = sys_getCaliValue();
	int eResultStrNum = sys_getCaliValue();
	int *vResult      = sys_getCaliVariable();
	
	*vResult = 1;
	
	DEBUG_COMMAND("regReadString %d,%d,%d,%d:\n",
		      eSubKeyStrNum, eBaneStrNum, eResultStrNum, *vResult);
}

void commands2F88() {
	int eFileNameStrNum = sys_getCaliValue();
	int *vResult        = sys_getCaliVariable();
	
	*vResult = 1;
	
	DEBUG_COMMAND("fileCheckExist %d,%d:\n", eFileNameStrNum, *vResult);
}

void commands2F89() {
	int eYear = sys_getCaliValue();
	int eMonth = sys_getCaliValue();
	int eData = sys_getCaliValue();
	int *vResult = sys_getCaliVariable();
	
	DEBUG_COMMAND_YET("timeCheckCurDate %d,%d,%d,%p:\n", eYear, eMonth, eData, vResult);
}

void commands2F8A() {
	char *s2 = sys_getConstString();
	char *s3 = sys_getConstString();

	DEBUG_COMMAND_YET("dlgManualProtect %s,%s:\n", s2, s3);
}

void commands2F8B() {
	char *s2 = sys_getConstString();
	int c3 = sys_getCaliValue();
	int c4 = sys_getCaliValue();
	char *s6 = sys_getConstString();
	int *c7 = sys_getCaliVariable();

	*c7 = 1;
	
	DEBUG_COMMAND_YET("fileCheckDVD %s,%d,%d,%s,%p:\n", s2, c3, c4, s6, c7);
}

void commands2F8C() {
	DEBUG_COMMAND("sysReset:\n");
	sys_reset();
}
