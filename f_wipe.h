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
( int32_t		x,
  int32_t		y,
  int16_t		width,
  int16_t		height );


int16_t
wipe_EndScreen
( int32_t		x,
  int32_t		y,
  int16_t		width,
  int16_t		height );


int16_t
wipe_ScreenWipe
( int32_t		wipeno,
  int32_t		x,
  int32_t		y,
  int16_t		width,
  int16_t		height,
  int32_t		ticks );

#endif
