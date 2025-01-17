//
// Copyright (C) 1993-1996 Id Software, Inc.
// Copyright (C) 2016-2017 Alexey Khokholov (Nuke.YKT)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	The status bar widget code.
//
#include <ctype.h>

#include "doomdef.h"

#include "z_zone.h"
#include "v_video.h"

#include "i_system.h"

#include "w_wad.h"

#include "st_stuff.h"
#include "st_lib.h"
#include "r_local.h"


// in AM_map.c
extern boolean		automapactive; 




void STlib_init(void)
{
}


// ?
void
STlib_initNum
( st_number_t*		n,
  int16_t			x,
  int16_t			y,
  MEMREF*		plRef,
  int16_t*			num,
  boolean*		on,
  int16_t			width )
{
    n->x	= x;
    n->y	= y;
    n->oldnum	= 0;
    n->width	= width;
    n->num	= num;
    n->on	= on;
    n->pRef	= plRef;
}


// 
// A fairly efficient way to draw a number
//  based on differences from the old number.
// Note: worth the trouble?
//
void
STlib_drawNum
( st_number_t*	n,
  boolean	refresh )
{
    int16_t		numdigits = n->width;
    int16_t		num = *n->num;
	patch_t* p0 = (patch_t*) Z_LoadBytesFromEMS(n->pRef[0]);
	int16_t w = (p0->width);
	int16_t h = (p0->height);
	int16_t x = n->x;
    
    int16_t		neg;

	// [crispy] redraw only if necessary
	if (n->oldnum == num && !refresh) {
		return;
	}

    n->oldnum = *n->num;

    neg = num < 0;

    if (neg)
    {
	if (numdigits == 2 && num < -9)
	    num = -9;
	else if (numdigits == 3 && num < -99)
	    num = -99;
	
	num = -num;
    }

    // clear the area
    x = n->x - numdigits*w;

    V_CopyRect(x, n->y - ST_Y, BG, w*numdigits, h, x, n->y, FG);

    // if non-number, do not draw it
    if (num == 1994)
	return;

    x = n->x;

    // in the special case of 0, you draw 0
    if (!num)
	V_DrawPatch(x - w, n->y, FG, (patch_t*)Z_LoadBytesFromEMS(n->pRef[ 0 ]));

    // draw the new number
    while (num && numdigits--)
    {
	x -= w;
	V_DrawPatch(x, n->y, FG, (patch_t*)Z_LoadBytesFromEMS( n->pRef[ num % 10 ]));
	num /= 10;
    }
 
}


//
void
STlib_updateNum
( st_number_t*		n,
  boolean		refresh )
{
    if (*n->on) STlib_drawNum(n, refresh);
}


//
void
STlib_initPercent
( st_percent_t*		p,
  int16_t			x,
  int16_t			y,
  MEMREF*		plRef,
  int16_t*			num,
  boolean*		on,
  MEMREF		percentRef )
{
    STlib_initNum(&p->n, x, y, plRef, num, on, 3);
    p->pRef = percentRef;
}




void
STlib_updatePercent
( st_percent_t*		per,
  int16_t			refresh )
{
    if (refresh && *per->n.on)
	V_DrawPatch(per->n.x, per->n.y, FG, (patch_t*)Z_LoadBytesFromEMS(per->pRef));
	
    
    STlib_updateNum(&per->n, refresh);
}



void
STlib_initMultIcon
( st_multicon_t*	i,
  int16_t			x,
  int16_t			y,
  MEMREF*		ilRef,
  int16_t*			inum,
  boolean*		on )
{
    i->x	= x;
    i->y	= y;
    i->oldinum 	= -1;
    i->inum	= inum;
    i->on	= on;
    i->pRef	= ilRef;
}



void
STlib_updateMultIcon
( st_multicon_t*	mi,
  boolean		refresh )
{
    int16_t			w;
    int16_t			h;
    int16_t			x;
    int16_t			y;
	patch_t*    old;
    if (*mi->on
	&& (mi->oldinum != *mi->inum || refresh)
	&& (*mi->inum!=-1))
    {
	if (mi->oldinum != -1)
	{
		old = (patch_t*)Z_LoadBytesFromEMS(mi->pRef[mi->oldinum]);
	    x = mi->x - (old->leftoffset);
	    y = mi->y - (old->topoffset);
	    w = (old->width);
	    h = (old->height);

	    if (y - ST_Y < 0)
		I_Error("updateMultIcon: y - ST_Y < 0");

	    V_CopyRect(x, y-ST_Y, BG, w, h, x, y, FG);
	}
	V_DrawPatch(mi->x, mi->y, FG, (patch_t*)Z_LoadBytesFromEMS(mi->pRef[*mi->inum]));
	mi->oldinum = *mi->inum;
    }
}



void
STlib_initBinIcon
( st_binicon_t*		b,
  int16_t			x,
  int16_t			y,
  MEMREF		iRef,
  boolean*		val,
  boolean*		on )
{
    b->x	= x;
    b->y	= y;
    b->oldval	= 0;
    b->val	= val;
    b->on	= on;
    b->pRef	= iRef;
}



void
STlib_updateBinIcon
( st_binicon_t*		bi,
  boolean		refresh )
{
    int16_t			x;
    int16_t			y;
    int16_t			w;
    int16_t			h;
	patch_t*	bipatch;
    if (*bi->on
	&& (bi->oldval != *bi->val || refresh)) {
		bipatch = (patch_t*)Z_LoadBytesFromEMS(bi->pRef);
		x = bi->x - (bipatch->leftoffset);
		y = bi->y - (bipatch->topoffset);
		w = (bipatch->width);
		h = (bipatch->height);

		if (y - ST_Y < 0)
			I_Error("updateBinIcon: y - ST_Y < 0");

		if (*bi->val)
			V_DrawPatch(bi->x, bi->y, FG, (patch_t*)Z_LoadBytesFromEMS(bi->pRef));
		else
			V_CopyRect(x, y-ST_Y, BG, w, h, x, y, FG);

		bi->oldval = *bi->val;
    }

}

