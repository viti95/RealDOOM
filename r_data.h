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
//  Refresh module, data I/O, caching, retrieval of graphics
//  by name.
//

#ifndef __R_DATA__
#define __R_DATA__

#include "r_defs.h"
#include "r_state.h"

// Retrieve column data for span blitting.


extern MEMREF lockedRef;

byte*
R_GetColumn
( int16_t		tex,
  int16_t		col );


#define BAD_TEXTURE 255

// I/O, setting up the stuff.
void R_InitData (void);
void R_PrecacheLevel (void);


// Retrieval.
// Floor/ceiling opaque texture tiles,
// lookup by name. For animation?
uint8_t R_FlatNumForName (int8_t* name);


// Called by P_Ticker for switches and animations,
// returns the texture number for the texture name.
uint8_t R_TextureNumForName(int8_t *name);
//int16_t R_TextureNumForName (int8_t *name);

uint8_t R_CheckTextureNumForName (int8_t *name);
void R_EraseCompositeCache(uint8_t texnum);


#endif
