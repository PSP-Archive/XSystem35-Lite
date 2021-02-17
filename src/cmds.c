/*
 * cmds.c  SYSTEM35 S command
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
/* $Id: cmds.c,v 1.33 2002/12/31 04:11:19 chikama Exp $ */

#include <stdio.h>
#include "portab.h"
#include "xsystem35.h"
#include "music_client.h"

/* �Ѥˤ���� �۾亮�ʥꥪ�к� */
static boolean dummy_pcm_in_play = FALSE;
/* Ʈ���Ի�II �۾亮�ʥꥪ�к� */
boolean dummy_pcm_su_flag = FALSE;
/* ���� cdrom �� loop ��� */ 
static int next_cdrom_loopcnt = 0;

void commandSS() {
	/* ���ڱ��դ򳫻Ϥ���ʣãĤΤߡ�*/
	int num = getCaliValue();
	static int pre = 0;
	
	DEBUG_COMMAND("SS %d:\n",num);
	
	if (num == 0) {
		mus_cdrom_stop();
	} else {
		if (pre != num) {
			mus_cdrom_stop();
			mus_cdrom_start(num + 1, next_cdrom_loopcnt);
		}
	}
	
	next_cdrom_loopcnt = 0;
	pre = num;
}

void commandSC() {
	/* �ãĤΥץ쥤��Υ������������� */
	int *var = getCaliVariable();
	cd_time info;
	
	mus_cdrom_get_playposition(&info);
	if (info.t == 999) {
		*var++ = 999;
	} else {
		*var++ = info.t - 1;
	}
	*var++ = info.m;
	*var++ = info.s;
	*var++ = info.f;
	
	DEBUG_COMMAND("SC %p:\n",var);
}

void commandSD() {
	/* ���ڱ��դ򳫻Ϥ���ʥǥե���Ȥϣãġݣģ���*/
	int num1 = getCaliValue();
	int num2 = getCaliValue();
	
	DEBUG_COMMAND_YET("SD %d,%d:\n",num1,num2);
}

void commandSR() {
	/* ���ڱ��վ�����֤� (CD,MIDI)
	   num   = �ǥХ����λ��� (0=CD , 1=MIDI)
	   var   = ���ն��ֹ� (0�λ��ϱ��������)
	   var+1 = �롼�ײ��
	   var+2 = ���հ���
	   var+3 = �ե����ɥ��󡿥�������
	*/
	/* Hushaby ���� SR,c,P:���ѹ����� */
	int num, *var;
	int c = sl_getcAt(sl_getIndex());
	if (c < 0x40) {
		num = sl_getc();
	} else {
		num  = getCaliValue();
	}
	var = getCaliVariable();
	
	if (num == 0) {
		cd_time info;
		mus_cdrom_get_playposition(&info);
		if (info.t == 999) {
			*var = 0;
		} else {
			*var = info.t - 1;
		}
	} else {
		midiplaystate st;
		mus_midi_get_playposition(&st);
		*var = st.play_no;
	}
	
	DEBUG_COMMAND("SR %d,%p:\n",num, var);
}

void commandSL() {
	/* ���β��ڤΥ롼�ײ������ꤹ�� */
	int num = getCaliValue();
	
	next_cdrom_loopcnt = num;
	
	DEBUG_COMMAND("SL %d:\n",num);
}

void commandSI() {
	/* ���ꤷ����������³���֤� var �˼��� */
	int type = sys_getc();
	int *var = getCaliVariable();
	
	if (type == 0) {        /* MIDI */
		*var = mus_midi_get_state()  == TRUE ? 1 : 0;
	} else if (type == 1) { /* PCM */
		*var = mus_pcm_get_state()   == TRUE ? 1 : 0;
	} else if (type == 2) { /* CD */
		*var = mus_cdrom_get_state() == TRUE ? 1 : 0;
	}
	
	DEBUG_COMMAND("SI %d,%d:\n",type,*var);
}

void commandSG() {
	/* MIDI���� */
	static int loopcnt = 0;
	int sw  = sys_getc();
	int num, fnum, *var;
	midiplaystate st;
	
	switch(sw) {
	case 0:
		/* ������Σͣɣģɤ���ߤ��� */
		num = getCaliValue();
		mus_midi_stop();
		DEBUG_COMMAND("SG0 %d:\n", num);
		break;
	case 1:
		/* �ͣɣģɤ���դ��� */
		num = getCaliValue();
		if (num == 0) {
			mus_midi_stop();
		} else {
			mus_midi_stop();
			mus_midi_start(num, loopcnt);
		}
		DEBUG_COMMAND("SG1 %d:\n", num);
		break;
	case 2:
		/* �ͣɣģɱ��հ��֤�1/100��ñ�̤Ǽ������� */
		var = getCaliVariable();
		mus_midi_get_playposition(&st);
		*var = st.loc_ms / 10;
		DEBUG_COMMAND("SG2 %p:\n", var);
		break;
	case 3:
		num = getCaliValue();
		if (num == 0) {
			/* ������Σͣɣģɤ�����ߤ��� */
			mus_midi_pause();
		} else {
			/* ��������Σͣɣģɤΰ����ߤ������� */
			mus_midi_unpause();
		}
		DEBUG_COMMAND("SG3 %d:\n", num);
		break;
	case 4:
		num = getCaliValue();
		/* ����SG1���ޥ�ɤǤ�MIDI���դη����֤�������� */
		loopcnt = num;
		DEBUG_COMMAND("SG4 %d:\n", num);
		break;
	case 5:
		fnum = getCaliValue() & 0x7f;
		num  = getCaliValue();
		mus_midi_set_flag(0, fnum, num);
		
		DEBUG_COMMAND("SG5 %d,%d:\n", fnum, num);
		break;
	case 6:
		fnum = getCaliValue() & 0x7f;
		num  = getCaliValue();
		mus_midi_set_flag(1, fnum, num);
		
		DEBUG_COMMAND("SG6 %d,%d:\n", fnum, num);
		break;
	case 7:
		fnum = getCaliValue() & 0x7f;
		var  = getCaliVariable();
		*var = mus_midi_get_flag(0, fnum);
		
		DEBUG_COMMAND("SG7 %d,%d:\n", fnum, *var);
		break;
	case 8: {
		fnum = getCaliValue() & 0x7f;
		var  = getCaliVariable();
		*var = mus_midi_get_flag(1, fnum);
		DEBUG_COMMAND("SG8 %d,%p:\n", fnum, var);
		break;
	}
	default:
		SYSERROR("Unknown SG command %d\n", sw);
		break;
	}
}

void commandSP() {
	/* �Уãͥǡ�������դ��� */
	int no = getCaliValue();
	int loop = getCaliValue();

	DEBUG_COMMAND("SP %d,%d:\n",no,loop);
	
	if (!mus_pcm_get_state()) {
		dummy_pcm_in_play = TRUE;
	}
	
	/* ???? */
	if (no == 0) {
		mus_pcm_stop(0);
	} else {
		mus_pcm_start(no, loop);
	}
}

void commandST() {
	/* �Уãͥǡ����α��դ���ߤ��롣 */
	int time = getCaliValue();
	
	DEBUG_COMMAND("ST %d:\n",time);
	
	if (!mus_pcm_get_state()) { 
		dummy_pcm_in_play = FALSE;
	}
	mus_pcm_stop(time);
}

void commandSU() {
	/* �Уãͤα��վ��֤��ѿ� var1 , var2 ���֤� */
	int *var1 = getCaliVariable();
	int *var2 = getCaliVariable();

	if (!mus_pcm_get_state()) {
		*var1 = dummy_pcm_in_play ? 1 : 0;
		*var2 = 0;
		if (dummy_pcm_in_play) dummy_pcm_in_play = FALSE;
	} else {
		*var1 = mus_pcm_get_playposition(var2);
		/* XXX for panyon_new */
	        if (*var2 == 0){
			*var1 = dummy_pcm_in_play ? TRUE : FALSE;
            		dummy_pcm_in_play = dummy_pcm_in_play ? FALSE : TRUE;
        	}	
	}
	if (dummy_pcm_su_flag) {
		*var1 = *var2 = 0;
	}
	
	DEBUG_COMMAND("SU %d,%d:\n",*var1, *var2);
}

void commandSQ() {
	/* �����̡��ΣУãͥǡ�����������Ʊ��դ��� */
	int noL  = getCaliValue();
	int noR  = getCaliValue();
	int loop = getCaliValue();
	
	DEBUG_COMMAND("SQ %d,%d,%d:\n", noL, noR, loop);
		     
	if (!mus_pcm_get_state()) {
		dummy_pcm_in_play = TRUE;
	}
	
	if( noL<1 || noR<1 ) {
		mus_pcm_stop(0);
		return;
	}
	mus_pcm_mix(noL, noR, loop);
	return;
}

void commandSO() {
	// �УãͥǥХ����Υ��ݡ��Ⱦ�������
	int *var = getCaliVariable();
	
	DEBUG_COMMAND_YET("SO %p:\n",var);
}

void commandSW() {
	/* ����ǡ������������ս���뤫�����å����롥*/
	int *var    = getCaliVariable();
	int channel = getCaliValue();
	int Srate   = getCaliValue();
	int bit     = getCaliValue();
	
	if (mus_pcm_get_state()) {
		int rate = Srate == 11 ? 11025 : Srate == 22 ? 22050 : Srate == 44 ? 44100 : 8000;
		boolean able;
		int ret;
		
		ret = mus_pcm_check_ability(bit, rate, channel, &able);
		if (ret < 0) {
			*var = 0;
		} else {
			*var = (able ? 2 : 1);
		}
	} else {
		*var = 0;
	}
	
	DEBUG_COMMAND("SW %p,%d,%d,%d:\n",var, channel, Srate, bit);
}

void commandSM() {
	/* �Уãͥǡ���������˾褻�롣*/
	int no = getCaliValue();
	DEBUG_COMMAND("SM %d:\n",no);
	
	mus_pcm_load(no);
}

void commandSX() {
	int device = sys_getc();
	int sw     = sys_getc();

	switch(sw) {
	case 1: {
		/* �ե����� */
		int time   = getCaliValue();
		int volume = getCaliValue();
		int stop   = getCaliValue();
		mus_mixer_fadeout_start(device, time, volume, stop);
		DEBUG_COMMAND("SX %d,%d,%d,%d,%d:\n", device, sw, time, volume, stop);
		break;
	}
	case 2: {
		/* �ե����ɽ�λ��ǧ */
		int *var   = getCaliVariable();
		boolean st;
		st = mus_mixer_fadeout_get_state(device);
		*var = (st ? 0 : 1);
		DEBUG_COMMAND("SX %d,%d:\n", device, sw);
		break;
	}
	case 3: {
		/*  �ե����ɶ�����λ */
		mus_mixer_fadeout_stop(device);
		DEBUG_COMMAND("SX %d,%d:\n", device, sw);
		break;
	}
	case 4: {
		/* �ܥ�塼����� */
		int *var   = getCaliVariable();
		*var = mus_mixer_get_level(device);
		DEBUG_COMMAND("SX %d,%d:\n", device, sw);
		break;
	}
	default:
		SYSERROR("Unknown SX command\n");
	}
}
