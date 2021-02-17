/*
 * cmdn.c  SYSTEM35 N command
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
/* $Id: cmdn.c,v 1.18 2001/04/02 21:00:44 chikama Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "portab.h"
#include "eucsjis.h"
#include "xsystem35.h"
#include "menu.h"

/* NI/NT �ѥѥ�᡼�� */
INPUTNUM_PARAM ni_param;

void commandNB() {
	/* var1 ����Ϥޤ�count�Ĥ��ѿ���
	   var2 ����Ϥޤ�count�Ĥ��ѿ��򥳥ԡ����� */
	int *var1 = getCaliVariable();
	int *var2 = getCaliVariable();
	int cnt   = getCaliValue();
	
	if (var1 == NULL) {
		WARNING("NB dst variable(var1) is NULL\n");
		return;
	}
	
	if (var2 == NULL) {
		WARNING("NB src variable(var2) is NULL\n");
		return;
	}
	
	DEBUG_COMMAND("NB %d,%d,%d:\n", *var1, *var2, cnt);
	while(cnt--) {
		*var1 = *var2; var1++; var2++;
	}
}

void commandNC() {
	/* var1����Ϥޤ�count�Ĥ��ѿ���0�ǥ��ꥢ���� */
	int *var1 = getCaliVariable();
	int cnt   = getCaliValue();
	
	DEBUG_COMMAND("NC %d,%d:\n", *var1, cnt);
	
	while(cnt--) {
		*var1++ = 0;
	}
}

void commandNR() {
	/* var1��var2�Υ롼�Ȥ���� (�ְ㤤)*/
	/* NR 100,D01:         ; < D01�˥롼��100(10)���֤� */
	int var1  = getCaliValue();
	int *var2 = getCaliVariable();
	
	*var2 = (int)sqrt(var1);
	DEBUG_COMMAND("NR %d,%d:\n", var1, *var2);
}

void commandN_ADD() {
	/* var1����Ϥޤ�count�Ĥ��ѿ���num��­�� */
	int *var1 = getCaliVariable();
	int num   = getCaliValue();
	int cnt   = getCaliValue();
	
	DEBUG_COMMAND("N+ %d,%d,%d:\n", *var1, num, cnt);
	while(cnt--) {
		*var1 = (int)((WORD)(*var1 + num)) ; var1++;
		// *var1 += num; var1++;
	}
}

void commandN_SUB() {
	/* var1����Ϥޤ�count�Ĥ��ѿ�����num����� */
	int *var1 = getCaliVariable();
	int num  = getCaliValue();
	int cnt  = getCaliValue();
	
	DEBUG_COMMAND("N- %d,%d,%d:\n", *var1, num, cnt);
	while(cnt--) {
		*var1 = max(0, *var1 - num); var1++;
		// *var1 -= num; var1++;
	}
}

void commandN_MUL() {
	/* var1����Ϥޤ�count�Ĥ��ѿ���num��ݤ��� */
	int *var1 = getCaliVariable();
	int num   = getCaliValue();
	int cnt   = getCaliValue();
	
	DEBUG_COMMAND("N* %d,%d,%d:\n", *var1, num, cnt);
	while(cnt--) {
		*var1 = (int)((WORD)(*var1 * num)); var1++;
		// *var1 *= num; var1++;
	}
}

void commandN_DIV() {
	/* var1����Ϥޤ�count�Ĥ��ѿ���num�ǳ�� */
	int *var1 = getCaliVariable();
	int num   = getCaliValue();
	int cnt   = getCaliValue();
	
	DEBUG_COMMAND("N/ %d,%d,%d:\n", *var1, num, cnt);
	while(cnt--) {
		// *var1 = (unsigned short)(*var1)/num; var1++;
		*var1 /= num; var1++;
	}
}

void commandN_GT() {
	/* var1 ����Ϥޤ�count�Ĥ��ѿ�����num����礭�������1�򡢰ʲ��ʤ��0��
	   var2����Ϥޤ��ѿ�����֤�
	*/
	int *var1 = getCaliVariable();
	int num   = getCaliValue();
	int cnt   = getCaliValue();
	int *var2 = getCaliVariable();
	
	DEBUG_COMMAND("N> %d,%d,%d,%d:\n", *var1, num, cnt, *var2);
	while (cnt--) {
		*var2 = *var1 > num ? 1 : 0; var1++; var2++;
	}
}

void commandN_LT() {
	/* var1����Ϥޤ�count�Ĥ��ѿ�����num��꾮�������1�򡢰ʾ�ʤ��0��
	   var2����Ϥޤ��ѿ�����֤�
	 */
	int *var1 = getCaliVariable();
	int num   = getCaliValue();
	int cnt   = getCaliValue();
	int *var2 = getCaliVariable();
	
	DEBUG_COMMAND("N< %p,%d,%d,%p:\n", var1, num, cnt, var2);
	while(cnt--) {
		*var2 = *var1 < num ? 1 : 0; var1++; var2++;
	}
}

void commandN_EQ() {
	/* var1����Ϥޤ�count�Ĥ��ѿ�����num�����������1���������ʤ����0��
	   var2����Ϥޤ��ѿ�����֤�
	 */
	int *var1 = getCaliVariable();
	int num   = getCaliValue();
	int cnt   = getCaliValue();
	int *var2 = getCaliVariable();
	
	DEBUG_COMMAND("N= %p,%d,%d,%p:\n", var1, num, cnt, var2);
	while(cnt--) {
		*var2 = *var1 == num ? 1 : 0; var1++; var2++;
	}
}

void commandN_NE() {
	/* var1����Ϥޤ�count�Ĥ��ѿ���0,1��ȿž���� */
	int *var1 = getCaliVariable();
	int cnt   = getCaliValue();
	
	DEBUG_COMMAND("N\\ %p,%d:\n", var1, cnt);
	while(cnt--) {
		*var1 ^= 1; var1++;
	}
}

void commandN_AND() {
	/* var1,var2��count�Ĥ��ѿ���AND��Ȥ� */
	int *var1 = getCaliVariable();
	int cnt   = getCaliValue();
	int *var2 = getCaliVariable();
	
	DEBUG_COMMAND("N& %p,%d,%p:\n", var1, cnt, var2);
	while(cnt--) {
		*var2 &= *var1; var1++; var2++;
	}
}

void commandN_OR() {
	/* var1,var2��count�Ĥ��ѿ���OR��Ȥ� */
	int *var1 = getCaliVariable();
	int cnt   = getCaliValue();
	int *var2 = getCaliVariable();
	
	DEBUG_COMMAND("N| %p,%d,%p:\n", var1, cnt, var2);
	while(cnt--) {
		*var2 |= *var1; var1++; var2++;
	}
}

void commandN_XOR() {
	/* var1,var2��count�Ĥ��ѿ���XOR��Ȥ� */
	int *var1 = getCaliVariable();
	int cnt   = getCaliValue();
	int *var2 = getCaliVariable();
	
	DEBUG_COMMAND("N^ %p,%d,%p:\n", var1, cnt, var2);
	while(cnt--) {
		*var2 ^= *var1; var1++; var2++;
	}
}

void commandN_NOT() {
	/* �ӥå�ȿž���� */
	int *var = getCaliVariable();
	int cnt  = getCaliValue();
	
	DEBUG_COMMAND("N~ %p,%d\n", var, cnt);
	while(cnt--) {
		*var ^= 0xffff; var++;
	}
}

void commandNO() { /* T2 */
	int p1 = sys_getc();
	int *dst_var = getCaliVariable();
	int *src_var = getCaliVariable();
	int cnt      = getCaliValue();
	int i, tmp = 0;
	
	if (p1 == 0) {
		/* �ѿ��¤Ӥ�ӥå���˰��̤��롣 */
		for (i = 0; i < cnt; i++) {
			tmp |= ((*src_var & 1) << (15 - (i%16))); src_var++;
			if ((i%16) == 15 && i < (cnt-1)) {
				*dst_var = tmp; dst_var++; tmp = 0;
			}
		}
		*dst_var = tmp;
	} else if (p1 == 1) {
		/* �ӥå�����ѿ��¤Ӥ�Ÿ�����롣*/
		for (i = 0; i < cnt; i++) {
			if ((i%16) == 0) {
				tmp = *src_var++;
			}
			*dst_var = (tmp & (1 << (15 - (i%16)))) ? 1 : 0; dst_var++;
		}
	} else {
		WARNING("Unknown NO %d command\n", p1);
	}
	
	DEBUG_COMMAND("NO %d,%p,%p,%d:\n", p1, src_var, dst_var, cnt);
}

void commandNDC() {
	/* w64n��num�򥳥ԡ����� */
	int w64n = getCaliValue();
	int num  = getCaliValue();
	longVar[w64n] = num;
	
	DEBUG_COMMAND("NDC %d,%d:\n", w64n, num);
}

void commandNDD() {
	/* var��w64n�򥳥ԡ�����*/
	int *var = getCaliVariable();
	int w64n = getCaliValue();
	*var = (int)longVar[w64n];
	
	DEBUG_COMMAND("NDD %p,%d:\n", var, w64n);
}

void commandNDM() {
	// ����w64n��ʸ�����ΰ�str��ʸ����Ȥ���ȿ��
	int str  = getCaliValue();
	int w64n = getCaliValue();
	
	DEBUG_COMMAND_YET("NDM %d,%d:\n", str, w64n);
}

void commandNDA() { 
	// ʸ�����ΰ�str����ͤȤ���w64n��ȿ��
	int str  = getCaliValue();
	int w64n = getCaliValue();
	
	DEBUG_COMMAND_YET("NDA %d,%d:\n", str, w64n);
}

void commandNDH() {
	// ����w64n����̤�ɽ���ʥѥ�᡼���ΰ�̣�ϣȥ��ޥ�ɤ˽���
	int str  = getCaliValue();
	int w64n = getCaliValue();
	
	DEBUG_COMMAND_YET("NDH %d,%d:\n", str, w64n);
}

void commandND_ADD() {
	/* w64n2��w64n3��­����w64n1������ */
	int w64n1 = getCaliValue();
	int w64n2 = getCaliValue();
	int w64n3 = getCaliValue();
	
	longVar[w64n1] = longVar[w64n2] + longVar[w64n3];
	
	DEBUG_COMMAND("ND+ %d,%d,%d:\n", w64n1, w64n2);
}

void commandND_SUB() {
	/* w64n2����w64n3�������w64n1������ */
	int w64n1 = getCaliValue();
	int w64n2 = getCaliValue();
	int w64n3 = getCaliValue();
	
	longVar[w64n1] = longVar[w64n2] - longVar[w64n3];
	
	DEBUG_COMMAND("ND- %d,%d,%d:\n", w64n1, w64n2, w64n3);
}

void commandND_MUL() {
	/* w64n2��w64n3��ݤ���w64n1������ */
	int w64n1 = getCaliValue();
	int w64n2 = getCaliValue();
	int w64n3 = getCaliValue();
	
	longVar[w64n1] = longVar[w64n2] * longVar[w64n3];
	
	DEBUG_COMMAND("ND* %d,%d,%d:\n", w64n1, w64n2, w64n3);
}

void commandND_DIV() {
	// w64n2��w64n3�ǳ�ä�w64n1������
	int w64n1 = getCaliValue();
	int w64n2 = getCaliValue();
	int w64n3 = getCaliValue();
	
	longVar[w64n1] = longVar[w64n2] / longVar[w64n3];
	
	DEBUG_COMMAND("ND/ %d,%d,%d:\n", w64n1, w64n2, w64n3);
}

void commandNI() { /* From Panyo */
	/* �������� */
	int *var  = getCaliVariable();
	int def   = getCaliValue();
	int _min  = getCaliValue();
	int _max  = getCaliValue();
	
	ni_param.def = def;
	ni_param.max = _max;
	ni_param.min = _min;
	
	menu_inputnumber(&ni_param);
	
	if (ni_param.value < 0) {
		sysVar[0] = 255;
	} else {
		*var = ni_param.value;
		sysVar[0] = 0;
	}
	
	DEBUG_COMMAND("NI %p,%d,%d,%d:\n", var, def, _min, _max);
}

void commandNT() { /* From Panyo */
	/* NI���ޥ�ɤ�ɽ�����륿���ȥ�����ꤹ�롣*/
	char *str = sys_getString(':');
	char *t;

	if (ni_param.title != NULL) {
		free(ni_param.title);
	}
	t = sjis2lang(str);
	ni_param.title = t;
	
	DEBUG_COMMAND("NT %p:\n", str);
}

void commandNP() {
	/* ������� */
	int *var1   = getCaliVariable();
	int *var2   = getCaliVariable();
	int count   = getCaliValue();
	int *result = getCaliVariable();
	
	DEBUG_COMMAND("NP %d,%d,%d,%d:\n", *var1, *var2, count, *result);

	while(count--) {
		if (*var1 != *var2) {
			*result = 0;
			return;
		}
		var1++; var2++;
	}
	*result = 1;
}
