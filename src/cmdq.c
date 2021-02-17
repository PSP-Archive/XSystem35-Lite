/*
 * cmdq.c  SYSTEM35 Q command
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
/* $Id: cmdq.c,v 1.12 2003/01/25 01:34:50 chikama Exp $ */

#include <stdio.h>
#include "portab.h"
#include "xsystem35.h"
#include "savedata.h"

#define WARN_SAVEERR(cmd, st) \
if (st > 200) fprintf(stderr, "WARNING: Fail to save (cmd=%s, stat=%d)\n", cmd, st)

void commandQD() {
	/* �ѿ��ΰ�ʤɤΥǡ����򥻡��֤��롣���������֡�*/
	int num   = getCaliValue();

	if (num <= 0) {
		sysVar[0] = 255;
	} else {
#if !defined(PSP)
		sysVar[0] = save_saveAll(num - 1);
#endif
	}
	
	WARN_SAVEERR("QD", sysVar[0]);
	
	DEBUG_COMMAND("QD %d:\n",num);
}

void commandQP() {
	/* �ѿ��ΰ�ʤɤΥǡ�������������֤��롣(�����ѿ���) */
	int num    = getCaliValue();
	int *point = getCaliVariable();
	int page   = preVarPage;
	int index  = preVarIndex;
	int cnt    = getCaliValue();

	if (num <= 0) {
		sysVar[0] = 255;
	} else {
#if !defined(PSP)
		sysVar[0] = save_savePartial(num - 1, page, index, cnt);
#endif
	}
	
	WARN_SAVEERR("QP", sysVar[0]);
	
	DEBUG_COMMAND("QP %d,%p,%d:\n", num, point, cnt);
}

void commandQC() {
	/* �����֥ե������num2���ΰ褫��num1���ΰ�إ��ԡ� */
	int num1 = getCaliValue();
	int num2 = getCaliValue();

	if (num1 <= 0 || num2 <= 0) {
		sysVar[0] = 255;
	} else {
#if !defined(PSP)
		sysVar[0] = save_copyAll(num1 - 1, num2 - 1);
#endif
	}
	
	WARN_SAVEERR("QC", sysVar[0]);
	
	DEBUG_COMMAND("QC %d,%d:\n",num1,num2);
}

void commandQE() {
	int type       = sys_getc();
	char *filename = sys_getString(':');
	int *var, _var = 0, cnt;

	switch(type) {
	case 0:
		var = getCaliVariable();
		cnt = getCaliValue();
#if !defined(PSP)
		sysVar[0] = save_save_var_with_file(filename, var, cnt);
#endif
		break;
	case 1:
		_var = getCaliValue();
		cnt  = getCaliValue();
#if !defined(PSP)
		sysVar[0] = save_save_str_with_file(filename, _var, cnt);
#endif
		break;
	default:
		WARNING("Unknown QE command %d\n", type); return;
	}
	
	WARN_SAVEERR("QE", sysVar[0]);
	
	DEBUG_COMMAND("QE %d,%s,%d,%d:\n", type, filename, _var, cnt);
}
