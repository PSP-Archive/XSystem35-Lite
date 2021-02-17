/*
 * flood.c  �ɤ�Ĥ֤���� 
 *
 * Copyright (C) 1998- TAJIRI Yasuhiro <tajiri@wizard.elec.waseda.ac.jp>
 *               2000-                 <tajiri@venus.dti.ne.jp>
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
/* $Id: flood.c,v 1.4 2006/04/21 16:40:48 chikama Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include "portab.h"
#include "flood.h"

typedef struct shdw {
    struct shdw *next;      /* ���αƤؤΥݥ��� */
    int left, right;        /* ü�� */
    int row, par;           /* ���αƤȿƥ饤����� */
    boolean ok;             /* ͭ��/̵�� �ե饰 */
} shadow;

/* �Ʒ����ѿ� */


static int currentRow;         /* ��������� */
static shadow *seedShadow;  /* ������α� */
static shadow *rowHead;     /* ��ꥹ�Ȥ���Ƭ�ؤΥݥ��� */
static shadow *pendHead;    /* �����å��ꥹ�Ȥ���Ƭ�ؤΥݥ��� */
static shadow *freeHead;    /* ���ꥹ�Ȥ���Ƭ�ؤΥݥ��� */

/* �������򤫤ɤ���Ƚ�ꤷ�ƿ����ѹ��򤹤�ؿ��ݥ��� */
static int (*isInterior)(int, int);

/* ���顼������ */

static jmp_buf errBuf;

/*****************************************************/

/* �ꥹ�Ȥ������� */

static void free_shadows(shadow *s) {
    shadow *t;
    while(s){
        t=s->next;
        free(s);
        s=t;
    }
}

/* ξü���������ꤷ�ƿ������Ƥ��� */
static void newshadow(int sleft, int sright, int srow, int prow) 
{

    shadow *new_list;

    if ((new_list = freeHead) != NULL)
        freeHead = freeHead->next;
    else if ((new_list= malloc(sizeof(shadow))) == NULL) {
        free_shadows(rowHead);
        free_shadows(pendHead);
        longjmp(errBuf, !0);
    }

    new_list->left = sleft;
    new_list->right = sright;
    new_list->row = srow;
    new_list->par = prow;
    new_list->ok = TRUE;
    new_list->next = pendHead;
    pendHead = new_list;
}

/* ��ꥹ�Ȥ��� */

static void make_row(void) {

    shadow *s, *t;

    t = pendHead;
    pendHead = NULL;

    while((s = t) != NULL) {
        t = t->next;
        if (s->ok) {
            if (rowHead == NULL) {
                currentRow = s->row;
                s->next = NULL;
                rowHead = s;
            }
            else if (s->row == currentRow) {
                if (s->left <= rowHead->left) {
                    s->next = rowHead;
                    rowHead = s;
                }
                else {
                    shadow* u;
                    for (u = rowHead; u->next; u = u->next)
                        if (s->left <= u->next->left)
                            break;
                    s->next = u->next;
                    u->next = s;
                }
            }
            else {
                s->next = pendHead;
                pendHead = s;
            }
        }
        else {
            s->next = freeHead;
            freeHead = s;
        }
    }
}

/* �Ƥ���饤��˽Ťʤ�ʤ���ʬ��ȴ���������������ƤȤ��� */

static void clipshadow(int left, int right, int row, shadow *line) 
{

    if (left < (line->left - 1))
        newshadow(left, line->left - 2, row, line->row);
    if (right > (line->right + 1))
        newshadow(line->right + 2, right, row, line->row);
}

/* �饤��˽Ťʤ�Ƥ���/����(̵���Υޡ�����Ĥ���)���� */

static void removeoverlap(shadow *rw) {

    shadow *child;

    for (child = pendHead; child->row != rw->par; child = child->next)
        ;

    clipshadow(child->left, child->right, child->row, rw);
    if (rw->right > (child->right + 1))
        rw->left = child->right + 2;
    else
        rw->ok = FALSE;
    child->ok = FALSE;

}

/* �ҥ饤�󤫤�Ƥ��� */
static void make_shadows(int left, int right) {

    shadow *p;

    if (currentRow > seedShadow->par) {
        newshadow(left, right, currentRow + 1, currentRow);
        clipshadow(left, right, currentRow - 1, seedShadow);
    }
    else if (currentRow < seedShadow->par) {
        newshadow(left, right, currentRow - 1, currentRow);
        clipshadow(left, right, currentRow + 1, seedShadow);
    }
    else {
        newshadow(left, right, currentRow + 1, currentRow);
        newshadow(left, right, currentRow - 1, currentRow);
    }

    for (p = rowHead; p && (p->left <= right); p = p->next)
        if (p->ok)
            removeoverlap(p);
}

/* �Ƥ���ˤ������Ƥ�̤õ���Υ饤��򸫤Ĥ��Ф� */

static void visitshadow(void)
{
    int col, left;

    for (col = seedShadow->left; col <= seedShadow->right; col++) 
    {
        if ((*isInterior)(col, currentRow)) {
            if ((left = col) == seedShadow->left) {
                while ((*isInterior)(--left, currentRow))
                    ;
                left++;
            }
            while ((*isInterior)(++col, currentRow))
                ;
            make_shadows(left, col - 1);
        }
    }
}

/* �ɤ�Ĥ֤� */
static void do_flood(int seedx, int seedy, int (*visit)(int, int)) 
{

    isInterior = visit;
    pendHead = rowHead = freeHead = NULL;

    newshadow(seedx, seedx, seedy, seedy);
    while (pendHead) {
        make_row();
        while (rowHead) {
            seedShadow = rowHead;

            rowHead = rowHead->next;
            if (seedShadow->ok)
                visitshadow();
            seedShadow->next = freeHead;
            freeHead = seedShadow;
        }
    }
    free_shadows(freeHead);
}

/* �����ɴؿ���Ȥä��ɤ�Ĥ֤��ؿ�(flood)��¹Ԥ��� */
int flood(int seed_x, int seed_y, int (*visit)(int, int))
{
	if (setjmp(errBuf) != 0)
		return(false);
	do_flood(seed_x, seed_y, visit);
	return(true);
}
