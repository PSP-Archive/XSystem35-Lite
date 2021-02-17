/*
 * variable.c  �ѿ�����
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
/* $Id: variable.c,v 1.15 2002/09/18 13:16:22 chikama Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "portab.h"
#include "variable.h"
#include "xsystem35.h"

/* �����ƥ��ѿ� */
int *sysVar;
/* �����ѿ��ξ��� */
arrayVarStruct *sysVarAttribute;
/* �������� */
arrayVarBufferStruct *arrayVarBuffer;
/* 64bit�ѿ� */
double longVar[SYSVARLONG_MAX];
/* ʸ�����ѿ� */
static char *strVar;
/* ʸ�����ѿ���°��(����,1�Ĥ�������礭��) */
int strvar_cnt = STRVAR_MAX;
int strvar_len = STRVAR_LEN;


/* ����Хåե��γ��� DC ,page = 1~ */
extern boolean v_allocateArrayBuffer(int page, int size, boolean flg) {
	void *buf;
	
	if (page <= 0 || page > 256)   { return false; }
	if (size <= 0 || size > 65536) { return false; }
	
	if (NULL == (buf = malloc(size * sizeof(int) + 257))) {
		NOMEMERR();
	}
	memset(buf, 0, size * sizeof(int) + 257);
	if (arrayVarBuffer[page - 1].value != NULL) {
		int len = min(size, arrayVarBuffer[page - 1].max);
		memcpy(buf, arrayVarBuffer[page - 1].value, len * sizeof(int));
	}
	
	arrayVarBuffer[page - 1].value    = buf;
	arrayVarBuffer[page - 1].max      = size;
	arrayVarBuffer[page - 1].saveflag = flg;
	
	return true;
}

/*�������ѿ��γ������ DS */
extern boolean v_defineArrayVar(int datavar, int *pointvar, int offset, int page) {
	if (datavar < 0 || datavar > SYSVAR_MAX - 1)                   { return false; }
	if (page    < 0 || page    > ARRAYVAR_PAGEMAX - 1)             { return false; }
	if (offset  < 0 || offset  > arrayVarBuffer[page - 1].max - 1) { return false; }
	
	sysVarAttribute[datavar].pointvar = pointvar;
	sysVarAttribute[datavar].page     = page;
	sysVarAttribute[datavar].offset   = offset;
	return true;
}

/* �����ѿ��γ�����Ʋ�� DR */
extern boolean v_releaseArrayVar(int datavar) {
	sysVarAttribute[datavar].page = 0;
	return true;
}

/* ����ڡ����κ����ѿ��μ��� page = 1~ */
extern int v_getArrayBufferCnt(int page) {
	return arrayVarBuffer[page - 1].max;
}

/* ����ڡ����ϻ����� page = 1~ */
extern boolean v_getArrayBufferStatus(int page) {
	return (arrayVarBuffer[page - 1].value != NULL) ? true : false;
}

/* ʸ�����ѿ��κƽ���� */
extern void v_initStringVars(int cnt,int len) {
	strVar = realloc(strVar, cnt * len);
	if (strVar == NULL) {
		NOMEMERR();
	}
	strvar_cnt = cnt;
	strvar_len = len;
}

/* �ѿ��ν���� */
extern boolean v_initVars() {
	sysVar          = calloc(SYSVAR_MAX, sizeof(int));
	sysVarAttribute	= calloc(SYSVAR_MAX, sizeof(arrayVarStruct));
	arrayVarBuffer  = calloc(ARRAYVAR_PAGEMAX, sizeof(arrayVarBufferStruct));
	strVar          = calloc(STRVAR_MAX, STRVAR_LEN);
	
	if (strVar == NULL || sysVar == NULL || sysVarAttribute == NULL || arrayVarBuffer == NULL) {
		NOMEMERR();
	}
	
	return true;
}

/* ʸ���ѿ��ؤ����� */
char *v_strcpy(int no, const char *str) {
	return strncpy(strVar + no * strvar_len, str, strvar_len - 1);
}

/* ʸ���ѿ��ؤ���³ */
char *v_strcat(int no, const char *str) {
	return strncat(strVar + no * strvar_len, str, strvar_len - 1);
}

/* ʸ���ѿ���Ĺ�� */
size_t v_strlen(int no) {
	return strlen(strVar + no * strvar_len);
}

/* ʸ���ѿ����Τ�� */
char *v_str(int no) {
	return strVar + no * strvar_len;
}

#ifdef DEBUG

void debug_showvariable() {
	int i,j,k;
	int *var;
	FILE *fp = fopen("VARIABLES.TXT","a");
	if (fp == NULL) return;

	fprintf(fp, "Page = %d, index = %x\n", sl_getPage(), sl_getIndex());

	var = &sysVar[0];
	fprintf(fp, "sysVar\n");
	for (i = 0; i < SYSVAR_MAX; i+=10) {
		for (j = 0; j < 10; j++) {
			fprintf(fp, "%d,", *var); var++;
		}
		fprintf(fp, "\n");
	}

	for (i = 0; i < ARRAYVAR_PAGEMAX; i++) {
		if (arrayVarBuffer[i].value != NULL) {
			fprintf(fp, "ArrayPage[%d],max=%d\n",i,arrayVarBuffer[i].max); 
			var = arrayVarBuffer[i].value;
			for (j = 0; j < arrayVarBuffer[i].max; j+=10) {
				for (k = 0; k < 10; k++) {
					fprintf(fp, "%d,", *var); var++;
				}
				fprintf(fp, "\n");
			}
		}
	}

	fclose(fp);
}

#endif
