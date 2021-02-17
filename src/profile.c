/*
 * profile.c  rcfile analizer
 *
 * Original GICQ Copyright 1998 Sean Gabriel <gabriel@montana.com>
 * ja extension Patch Copyright Takashi Mizuhiki <mizuhiki@cclub.cc.tut.ac.jp>
 * modified for xsystem35 Masaki Chikama (Wren) <masaki-c@is.aist-nata.ac.jp>
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
/* $Id: profile.c,v 1.6 2001/04/02 21:00:44 chikama Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "portab.h"

#include "profile.h"

static char **profile_name = NULL, **profile_value = NULL;
static int profile_values = 0;
static int profile_ready = FALSE;

int load_profile(char *path)
{
	FILE *fp = NULL;
	char *home_dir = NULL, profile_path[PATH_MAX], rc_line[RC_LINE_CHARS_MAX];
	char *p, *q;
	int now_values = 0, is_flag, line = 0;
	
	return 1;
	
	/* ����� path ��ޤ�õ�� */
	if (path) {
		
	} else {
		
	}

	/* $(HOME)/.xsys35rc ��õ�� */
	home_dir = "."; //getenv("HOME");
	if (home_dir) {
		sprintf(profile_path, "%s/%s", home_dir, RC_NAME);
		fp = fopen(profile_path, "r");
	}
	
	if (!fp) {
		/* ���Ĥ���ʤ��ä��顢CWD ���� .gicqjarc ��õ�� */
		strcpy(profile_path, RC_NAME);
		fp = fopen(RC_NAME, "r");
		if (!fp) {
			/* CWD ����⸫�Ĥ���ʤ��ä��顢���顼��ɽ�����ƽ�λ */
			char *error_msg;
			error_msg = malloc(strlen(profile_path) + 12);
			sprintf(error_msg, "fopen() '%s'", profile_path);
			perror(error_msg);
			free(error_msg);
			return 1;
		}
	}
	
	while (1) {
		if (!fgets(rc_line, sizeof(rc_line), fp)) {
			if (feof(fp))
				break;
			else {
				perror("fgets()");
				return 1;
			}
		}
		line++;
		
		/* �ɤ߹�����Ԥ�ѡ������� */
		p = q = rc_line;
		is_flag = FALSE;
		
		while (*q) {
			if (*q == ':')
				is_flag = TRUE;
			
			if (iscntrl(*q) || (is_flag == TRUE)?*q == '\n':isspace(*q))
				q++;
			else
				*p++ = *q++;
		}
		
		*p = '\0';
		
		/* ��Ƭ�� # ���ä����ȡ����Ԥξ��ϵ������ʤ� */
		if (rc_line[0] == '#' || rc_line[0] == '\0')
			continue;
		
		if (!(p = strchr(rc_line, ':'))) {
			fprintf(stderr, "XSYSTEM35: Syntax Error in '%s' line %d.\n", profile_path, line);
			return 1;
		}
		
		*p = '\0';
		
		while (*++p)
			if (!isspace(*p) && !iscntrl(*p))
				break;
		
		/* ANSI �� realloc �� glib �� g_realloc �� ptr �� NULL �ݥ��󥿤� */
		/* �ޤޤ�Ƥ������ε�ư���㤦�褦�� */
		profile_name = realloc(profile_name, sizeof(char *) * (now_values + 1));
		if (!profile_name) {
			perror("realloc()");
			return 1;
		}
		
		profile_value = realloc(profile_value, sizeof(char *) * (now_values + 1));
		if (!profile_value) {
			perror("realloc()");
			return 1;
		}
		
		*(profile_name + now_values) = malloc(strlen(rc_line) + 1);
		strcpy(*(profile_name + now_values), rc_line);
		*(profile_value + now_values) = malloc(strlen(p) + 1);
		strcpy(*(profile_value + now_values), p);
		now_values++;
	}
	
	profile_values = now_values;
	profile_ready = TRUE;
	return 0;
}

char *get_profile(const char *name)
{
	int i;
	
	if (profile_ready == FALSE)
		return NULL;
	
	for (i = 0; i < profile_values; i++)
		if (strcmp(name, *(profile_name + i)) == 0)
			break;
	
	if (i == profile_values)
		return NULL;
	
	return *(profile_value + i);
}

