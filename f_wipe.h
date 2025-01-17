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
//	Mission start screen wipe/melt, special effects.
//


#ifndef __F_WIPE_H__
#define __F_WIPE_H__

//
//                       SCREEN WIPE PACKAGE
//

enum
{
    // simple gradual pixel change for 8-bit only
    wipe_ColorXForm,
    
    // weird screen melt
    wipe_Melt,	

    wipe_NUMWIPES
};

int16_t
wipe_StartScreen
( int16_t		x,
  int16_t		y,
  int16_t		width,
  int16_t		height );


int16_t
wipe_EndScreen
( int16_t		x,
  int16_t		y,
  int16_t		width,
  int16_t		height );


int16_t
wipe_ScreenWipe
( int16_t		wipeno,
  int16_t		x,
  int16_t		y,
  int16_t		width,
  int16_t		height,
  int16_t		ticks );

#endif
