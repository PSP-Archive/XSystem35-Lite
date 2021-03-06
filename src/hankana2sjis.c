/*
 * hankana2sjis.c  半角カタカナ -> 全角平仮名変換
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
/* $Id: hankana2sjis.c,v 1.4 2000/11/25 13:09:06 chikama Exp $ */

// index = 0xa0~
char *hankana2sjis(int index) {
	static char kanatbl[][2] = {
		0x81, 0x40, 0x81, 0x42, 0x81, 0x75, 0x81, 0x76,
		0x81, 0x41, 0x81, 0x45, 0x82, 0xf0, 0x82, 0x9f,
		0x82, 0xa1, 0x82, 0xa3, 0x82, 0xa5, 0x82, 0xa7,
		0x82, 0xe1, 0x82, 0xe3, 0x82, 0xe5, 0x82, 0xc1,
		0x81, 0x5b, 0x82, 0xa0, 0x82, 0xa2, 0x82, 0xa4,
		0x82, 0xa6, 0x82, 0xa8, 0x82, 0xa9, 0x82, 0xab,
		0x82, 0xad, 0x82, 0xaf, 0x82, 0xb1, 0x82, 0xb3,
		0x82, 0xb5, 0x82, 0xb7, 0x82, 0xb9, 0x82, 0xbb,
		0x82, 0xbd, 0x82, 0xbf, 0x82, 0xc2, 0x82, 0xc4,
		0x82, 0xc6, 0x82, 0xc8, 0x82, 0xc9, 0x82, 0xca,
		0x82, 0xcb, 0x82, 0xcc, 0x82, 0xcd, 0x82, 0xd0,
		0x82, 0xd3, 0x82, 0xd6, 0x82, 0xd9, 0x82, 0xdc,
		0x82, 0xdd, 0x82, 0xde, 0x82, 0xdf, 0x82, 0xe0,
		0x82, 0xe2, 0x82, 0xe4, 0x82, 0xe6, 0x82, 0xe7,
		0x82, 0xe8, 0x82, 0xe9, 0x82, 0xea, 0x82, 0xeb,
		0x82, 0xed, 0x82, 0xf1, 0x81, 0x4a, 0x81, 0x4b
	};
	return kanatbl[index - 0xa0];
}

char *num2sjis(int num) {
	static char suji[][3] = {
		0x82,0x4f,0x00, /* 0 */
		0x82,0x50,0x00, /* 1 */
		0x82,0x51,0x00, /* 2 */
		0x82,0x52,0x00, /* 3 */
		0x82,0x53,0x00, /* 4 */
		0x82,0x54,0x00, /* 5 */
		0x82,0x55,0x00, /* 6 */
		0x82,0x56,0x00, /* 7 */
		0x82,0x57,0x00, /* 8 */
		0x82,0x58,0x00, /* 9 */
		0x81,0x40,0x00, /* space */
	};
	return suji[num];
}
