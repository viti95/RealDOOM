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
//      Refresh/rendering module, shared data struct definitions.
//

#ifndef __R_DEFS__
#define __R_DEFS__


// Screenwidth.
#include "doomdef.h"

// Some more or less basic data types
// we depend on.
//#include "m_fixed.h"

// We rely on the thinker data struct
// to handle sound origins in sectors.
#include "d_think.h"
// SECTORS do store MObjs anyway.
#include "p_mobj.h"



// Silhouette, needed for clipping Segs (mainly)
// and sprites representing things.
#define SIL_NONE		0
#define SIL_BOTTOM		1
#define SIL_TOP			2
#define SIL_BOTH		3

#define MAXDRAWSEGS		256





//
// INTERNAL MAP TYPES
//  used by play and refresh
//

//
// Your plain vanilla vertex.
// Note: transformed values not buffered locally,
//  like some DOOM-alikes ("wt", "WebView") did.
//
typedef struct
{
    int16_t	x;
    int16_t	y;
    
} vertex_t;


// Forward of LineDefs, for Sectors.
struct line_s;
 
//
// The SECTORS record, at runtime.
// Stores things/mobjs.
//
typedef	struct
{

    short_height_t	floorheight;
    short_height_t	ceilingheight;
    uint8_t	floorpic;
    uint8_t	ceilingpic;
    uint8_t	lightlevel; // seems to max at 255
	uint8_t	special;	// only a few small numbers
	uint8_t	tag;		

    // 0 = untraversed, 1,2 = sndlines -1
    int8_t		soundtraversed;

    // thing that made a sound (or null)
    MEMREF	soundtargetRef;

    // mapblock bounding box for height changes
    int16_t	blockbox[4];

    // origin for any sounds played by the sector
    // corresponds to fixed_t, not easy to change
	int32_t soundorgX;
	int32_t soundorgY;

    // if == validcount, already checked
    int16_t		validcount;

    // list of mobjs in sector
    MEMREF	thinglistRef;

    // thinker_t for reversable actions
	MEMREF	specialdataRef;
    uint8_t		linecount;  // is int8 ok? seems more than 2-3 is rare..

	int16_t linesoffset;	// [linecount] size

} sector_t;




//
// The SideDef.
//

typedef struct
{
    // add this to the calculated texture column
	texsize_t	textureoffset;
    
    // add this to the calculated texture top
	texsize_t	rowoffset;

    // Texture indices.
    // We do not maintain names here. 
    uint8_t	toptexture;
    uint8_t	bottomtexture;
    uint8_t	midtexture;

    // Sector the SideDef is facing.
    int16_t	secnum;
    
} side_t;



//
// Move clipping aid for LineDefs.
//
#define     ST_HORIZONTAL 0
#define     ST_VERTICAL 1
#define     ST_POSITIVE 2
#define     ST_NEGATIVE 3
typedef uint8_t slopetype_t;



typedef struct line_s
{
    // Vertices, from v1 to v2.
	int16_t	v1Offset;
	int16_t	v2Offset;

    // Precalculated v2 - v1 for side checking.
    int16_t	dx;
    int16_t	dy;

    // Animation related.
    // theres only 9 flags here, lets try to pull one out somewhere...
	int16_t	flags;
    uint8_t	special;
	uint8_t	tag;

    // Visual appearance: SideDefs.
    //  sidenum[1] will be -1 if one sided
    int16_t	sidenum[2];			

    // Neat. Another bounding box, for the extent
    //  of the LineDef.
	int16_t	bbox[4];

    // To aid move clipping.
    slopetype_t	slopetype;

    // Front and back sector.
    // Note: redundant? Can be retrieved from SideDefs.
	int16_t	frontsecnum;
	int16_t	backsecnum;

    // if == validcount, already checked
    int16_t		validcount;

    // thinker_t for reversable actions
    //void*	specialdata;		
	MEMREF specialdataRef;
} line_t;




//
// A SubSector.
// References a Sector.
// Basically, this is a list of LineSegs,
//  indicating the visible walls that define
//  (all or some) sides of a convex BSP leaf.
//
typedef struct subsector_s
{
    int16_t	secnum;
    int16_t	numlines;
    int16_t	firstline;
    
} subsector_t;



//
// The LineSeg.
//
typedef struct
{

	int16_t	v1Offset;
	int16_t	v2Offset;

	// todo can this be made 16 bit?
    fixed_t	offset;

    fineangle_t	fineangle;

    int16_t	sidedefOffset;
    int16_t	linedefOffset;

    // Sector references.
    // Could be retrieved from linedef, too.
    // backsector is NULL for one sided lines
    int16_t	frontsecnum;
    int16_t	backsecnum;
    
} seg_t;



//
// BSP node.
//
typedef struct
{
    // Partition line.
    int16_t	x;
    int16_t	y;
    int16_t	dx;
    int16_t	dy;

    // Bounding box for each child.
    int16_t	bbox[2][4];

    // If NF_SUBSECTOR its a subsector.
    uint16_t children[2];
    
} node_t;




// posts are runs of non masked source pixels
typedef struct
{
    byte		topdelta;	// -1 is the last post in a column
    byte		length; 	// length data bytes follows
} post_t;

// column_t is a list of 0 or more post_t, (byte)-1 terminated
typedef post_t	column_t;



// PC direct to screen pointers
extern byte*	destview;
extern byte*	destscreen;





//
// OTHER TYPES
//

// This could be wider for >8 bit display.
// Indeed, true color support is posibble
//  precalculating 24bpp lightmap/colormap LUT.
//  from darkening PLAYPAL to all black.
// Could even us emore than 32 levels.
typedef byte	lighttable_t;	




//
// ?
//
typedef struct drawseg_s
{
    int16_t		curlinenum;

    int16_t			x1;
    int16_t			x2;

    fixed_t		scale1;
    fixed_t		scale2;
    int16_t		scalestep;

    // 0=none, 1=bottom, 2=top, 3=both
    int8_t			silhouette;

    // do not clip sprites above this
    int16_t		bsilheight;

    // do not clip sprites below this
    int16_t		tsilheight;
    
    // Pointers to lists for sprite clipping,
    //  all three adjusted so [x1] is first value.
    int16_t*		sprtopclip;		
    int16_t*		sprbottomclip;	
    int16_t*		maskedtexturecol;
    
} drawseg_t;



// Patches.
// A patch holds one or more columns.
// Patches are used for sprites and all masked pictures,
// and we compose textures from the TEXTURE1/2 lists
// of patches.
typedef struct 
{ 
    int16_t		width;		// bounding box size 
    int16_t		height; 
    int16_t		leftoffset;	// pixels to the left of origin 
    int16_t		topoffset;	// pixels below the origin 
	// not 100% sure what it is but changing to 16 bit is a crash. might be pointers.
    int32_t		columnofs[8];	// only [width] used
    // the [0] is &columnofs[width] 
} patch_t;







// A vissprite_t is a thing
//  that will be drawn during a refresh.
// I.e. a sprite object that is partly visible.
typedef struct vissprite_s
{
    // Doubly linked list.
    struct vissprite_s*	prev;
    struct vissprite_s*	next;
    

	int16_t x1;
	int16_t x2;

    // for line side calculation
    fixed_t		gx;
    fixed_t		gy;		

    // global bottom / top for silhouette clipping
    fixed_t		gz;
    fixed_t		gzt;

    // horizontal position of x1
    fixed_t		startfrac;
    
    fixed_t		scale;
    
    // negative if flipped
    fixed_t		xiscale;	

    fixed_t		texturemid;
	int16_t patch;

    // for color translation and shadow draw,
    //  maxbright frames as well
    lighttable_t*	colormap;
   
    int32_t			mobjflags;
    
} vissprite_t;


//	
// Sprites are patches with a special naming convention
//  so they can be recognized by R_InitSprites.
// The base name is NNNNFx or NNNNFxFx, with
//  x indicating the rotation, x = 0, 1-7.
// The sprite and frame specified by a thing_t
//  is range checked at run time.
// A sprite is a patch_t that is assumed to represent
//  a three dimensional object and may have multiple
//  rotations pre drawn.
// Horizontal flipping is used to save space,
//  thus NNNNF2F5 defines a mirrored patch.
// Some sprites will only have one picture used
// for all views: NNNNF0
//
typedef struct
{
    // If false use 0 for any position.
    // Note: as eight entries are available,
    //  we might as well insert the same name eight times.
    boolean	rotate;

    // Lump to use for view angles 0-7.
    int16_t	lump[8];

    // Flip bit (1 = flip) to use for view angles 0-7.
    byte	flip[8];
    
} spriteframe_t;



//
// A sprite definition:
//  a number of animation frames.
//
typedef struct
{
    int8_t			numframes;
    MEMREF		spriteframesRef;

} spritedef_t;


#ifdef EMS_VISPLANES

typedef struct
{
  fixed_t height;
  uint8_t picnum;
  uint8_t lightlevel;
  int16_t minx;
  int16_t maxx;
   
  // page and offset within the data. takes up some space but i think it's better than constant divisions and modulos.
  int8_t visplanepage;
  int8_t visplaneoffset;

} visplaneheader_t;



typedef struct
{
  
  // leave pads for [minx-1]/[maxx+1]
  
  byte		pad1;
  // Here lies the rub for all
  //  dynamic resize/change of resolution.
  byte		top[SCREENWIDTH];
  byte		pad2;
  byte		pad3;
  // See above.
  byte		bottom[SCREENWIDTH];
  byte		pad4;

} visplanebytes_t;

#else

//
// Now what is a visplane, anyway?
// 
typedef struct
{
  fixed_t height;
  uint8_t picnum;
  uint8_t lightlevel;
  int16_t minx;
  int16_t maxx;
  
  // leave pads for [minx-1]/[maxx+1]
  
  byte		pad1;
  // Here lies the rub for all
  //  dynamic resize/change of resolution.
  byte		top[SCREENWIDTH];
  byte		pad2;
  byte		pad3;
  // See above.
  byte		bottom[SCREENWIDTH];
  byte		pad4;

} visplane_t;

#endif


#endif
