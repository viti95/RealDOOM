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
// DESCRIPTION:  the automap code
//

#include <stdio.h>
#include <alloca.h>

#include "z_zone.h"
#include "doomdef.h"
#include "st_stuff.h"
#include "p_local.h"
#include "w_wad.h"

#include "dutils.h"
#include "i_system.h"

// Needs access to LFB.
#include "v_video.h"

// State.
#include "doomstat.h"
#include "r_state.h"

// Data.
#include "dstrings.h"

#include "am_map.h"


// For use if I do walls with outsides/insides
#define REDS		(256-5*16)
#define REDRANGE	16
#define BLUES		(256-4*16+8)
#define BLUERANGE	8
#define GREENS		(7*16)
#define GREENRANGE	16
#define GRAYS		(6*16)
#define GRAYSRANGE	16
#define BROWNS		(4*16)
#define BROWNRANGE	16
#define YELLOWS		(256-32+7)
#define YELLOWRANGE	1
#define BLACK		0
#define WHITE		(256-47)

// Automap colors
#define BACKGROUND	BLACK
#define YOURCOLORS	WHITE
#define YOURRANGE	0
#define WALLCOLORS	REDS
#define WALLRANGE	REDRANGE
#define TSWALLCOLORS	GRAYS
#define TSWALLRANGE	GRAYSRANGE
#define FDWALLCOLORS	BROWNS
#define FDWALLRANGE	BROWNRANGE
#define CDWALLCOLORS	YELLOWS
#define CDWALLRANGE	YELLOWRANGE
#define THINGCOLORS	GREENS
#define THINGRANGE	GREENRANGE
#define SECRETWALLCOLORS WALLCOLORS
#define SECRETWALLRANGE WALLRANGE
#define GRIDCOLORS	(GRAYS + GRAYSRANGE/2)
#define GRIDRANGE	0
#define XHAIRCOLORS	GRAYS

// drawing stuff
#define	FB		0

#define AM_PANDOWNKEY	KEY_DOWNARROW
#define AM_PANUPKEY	KEY_UPARROW
#define AM_PANRIGHTKEY	KEY_RIGHTARROW
#define AM_PANLEFTKEY	KEY_LEFTARROW
#define AM_ZOOMINKEY	'='
#define AM_ZOOMOUTKEY	'-'
#define AM_STARTKEY	KEY_TAB
#define AM_ENDKEY	KEY_TAB
#define AM_GOBIGKEY	'0'
#define AM_FOLLOWKEY	'f'
#define AM_GRIDKEY	'g'
#define AM_MARKKEY	'm'
#define AM_CLEARMARKKEY	'c'

#define AM_NUMMARKPOINTS 10

// scale on entry


#define INITSCALEMTOF (.2*FRACUNIT)


// how much the automap moves window per tic in frame-buffer coordinates
// moves 140 pixels in 1 second
#define F_PANINC	4L
// how much zoom-in per tic
// goes to 2x in 1 second
#define M_ZOOMIN        ((int32_t) (1.02*FRACUNIT))
// how much zoom-out per tic
// pulls out to 0.5x in 1 second
#define M_ZOOMOUT       ((int32_t) (FRACUNIT/1.02))

// translates between frame-buffer and map distances

#define FTOM(x) FixedMul(((x)<<16),scale_ftom)
#define FTOM16(x) FixedMul1632(x,scale_ftom)
#define MTOF(x) (FixedMul((x),scale_mtof)>>16)
#define MTOF(x) (FixedMul((x),scale_mtof)>>16)

// translates between frame-buffer and map coordinates
#define CXMTOF(x)  (f_x + MTOF((x)-m_x))
#define CYMTOF(y)  (f_y + (f_h - MTOF((y)-m_y)))

// the following is crap
#define LINE_NEVERSEE ML_DONTDRAW

typedef struct
{
	fixed_t x, y;
} fpoint_t;

typedef struct
{
    fpoint_t a, b;
} fline_t;

typedef struct
{
    fixed_t		x,y;
} mpoint_t;

typedef struct
{
    mpoint_t a, b;
} mline_t;

typedef struct
{
    fixed_t slp, islp;
} islope_t;



//
// The vector graphics for the automap.
//  A line drawing of the player pointing right,
//   starting from the middle.
//
#define R ((8*PLAYERRADIUS)/7)
mline_t player_arrow[] = {
    { { -R+R/8, 0 }, { R, 0 } }, // -----
    { { R, 0 }, { R-R/2, R/4 } },  // ----->
    { { R, 0 }, { R-R/2, -R/4 } },
    { { -R+R/8, 0 }, { -R-R/8, R/4 } }, // >---->
    { { -R+R/8, 0 }, { -R-R/8, -R/4 } },
    { { -R+3*R/8, 0 }, { -R+R/8, R/4 } }, // >>--->
    { { -R+3*R/8, 0 }, { -R+R/8, -R/4 } }
};
#undef R
#define NUMPLYRLINES (sizeof(player_arrow)/sizeof(mline_t))

#define R ((8*PLAYERRADIUS)/7)
mline_t cheat_player_arrow[] = {
    { { -R+R/8, 0 }, { R, 0 } }, // -----
    { { R, 0 }, { R-R/2, R/6 } },  // ----->
    { { R, 0 }, { R-R/2, -R/6 } },
    { { -R+R/8, 0 }, { -R-R/8, R/6 } }, // >----->
    { { -R+R/8, 0 }, { -R-R/8, -R/6 } },
    { { -R+3*R/8, 0 }, { -R+R/8, R/6 } }, // >>----->
    { { -R+3*R/8, 0 }, { -R+R/8, -R/6 } },
    { { -R/2, 0 }, { -R/2, -R/6 } }, // >>-d--->
    { { -R/2, -R/6 }, { -R/2+R/6, -R/6 } },
    { { -R/2+R/6, -R/6 }, { -R/2+R/6, R/4 } },
    { { -R/6, 0 }, { -R/6, -R/6 } }, // >>-dd-->
    { { -R/6, -R/6 }, { 0, -R/6 } },
    { { 0, -R/6 }, { 0, R/4 } },
    { { R/6, R/4 }, { R/6, -R/7 } }, // >>-ddt->
    { { R/6, -R/7 }, { R/6+R/32, -R/7-R/32 } },
    { { R/6+R/32, -R/7-R/32 }, { R/6+R/10, -R/7 } }
};
#undef R
#define NUMCHEATPLYRLINES (sizeof(cheat_player_arrow)/sizeof(mline_t))

#define R (FRACUNIT)
mline_t triangle_guy[] = {
    { { -.867*R, -.5*R }, { .867*R, -.5*R } },
    { { .867*R, -.5*R } , { 0, R } },
    { { 0, R }, { -.867*R, -.5*R } }
};
#undef R

#define R (FRACUNIT)
mline_t thintriangle_guy[] = {
    { { -.5*R, -.7*R }, { R, 0 } },
    { { R, 0 }, { -.5*R, .7*R } },
    { { -.5*R, .7*R }, { -.5*R, -.7*R } }
};
#undef R
#define NUMTHINTRIANGLEGUYLINES (sizeof(thintriangle_guy)/sizeof(mline_t))




static int16_t 	cheating = 0;
static int16_t 	grid = 0;

static int16_t 	leveljuststarted = 1; 	// kluge until AM_LevelInit() is called

boolean    	automapactive = false;
static int16_t 	finit_width = SCREENWIDTH;
static int16_t 	finit_height = SCREENHEIGHT - 32;

// location of window on screen
static int16_t 	f_x;
static int16_t	f_y;

// size of window on screen
static int16_t 	f_w;
static int16_t	f_h;

static byte*	fb; 			// pseudo-frame buffer

static mpoint_t m_paninc; // how far the window pans each tic (map coords)
static fixed_t 	mtof_zoommul; // how far the window zooms in each tic (map coords)
static fixed_t 	ftom_zoommul; // how far the window zooms in each tic (fb coords)

static fixed_t 	m_x, m_y;   // LL x,y where the window is on the map (map coords)
static fixed_t 	m_x2, m_y2; // UR x,y where the window is on the map (map coords)

//
// width/height of window on map (map coords)
//
static fixed_t	m_w;
static fixed_t	m_h;

// based on level size
static fixed_t 	min_x;
static fixed_t	min_y; 
static fixed_t 	max_x;
static fixed_t  max_y;

static fixed_t 	max_w; // max_x-min_x,
static fixed_t  max_h; // max_y-min_y

// based on player size
static fixed_t 	min_scale_mtof; // used to tell when to stop zooming out
static fixed_t 	max_scale_mtof; // used to tell when to stop zooming in

// old stuff for recovery later
static fixed_t old_m_w, old_m_h;
static fixed_t old_m_x, old_m_y;

// old location used by the Follower routine
static mpoint_t f_oldloc;

// used by MTOF to scale from map-to-frame-buffer coords

DECLARE_FIXED_POINT_HIGH (static fixed_t scale_mtof, INITSCALEMTOF);

// used by FTOM to scale from frame-buffer-to-map coords (=1/scale_mtof)
static fixed_t scale_ftom;

static MEMREF marknums[10]; // numbers used for marking by the automap
static mpoint_t markpoints[AM_NUMMARKPOINTS]; // where the points are
static int8_t markpointnum = 0; // next point to be assigned

static int8_t followplayer = 1; // specifies whether to follow the player around

static uint8_t cheat_amap_seq[] = {'i', 'd', 'd', 't', 0xff};
static cheatseq_t cheat_amap = { cheat_amap_seq, 0 };

static boolean stopped = true;

extern boolean viewactive;



void
V_MarkRect
( int16_t	x,
  int16_t	y,
  int16_t	width,
  int16_t	height );

 

//
//
//
void AM_activateNewScale(void)
{
 

    m_x += m_w/2;
    m_y += m_h/2;
    m_w = FTOM16(f_w);
    m_h = FTOM16(f_h);
    m_x -= m_w/2;
    m_y -= m_h/2;
    m_x2 = m_x + m_w;
    m_y2 = m_y + m_h;

}

//
//
//
void AM_saveScaleAndLoc(void)
{
    old_m_x = m_x;
    old_m_y = m_y;
    old_m_w = m_w;
    old_m_h = m_h;
}

void AM_restoreScaleAndLoc(void)
{
	mobj_t* playerMo;
	fixed_t_union temp;
	temp.h.fracbits = 0;

    m_w = old_m_w;
    m_h = old_m_h;
    if (!followplayer) {
		m_x = old_m_x;
		m_y = old_m_y;
    } else {
		playerMo = (mobj_t*)Z_LoadBytesFromEMS(players.moRef);
		m_x = playerMo->x - m_w/2;
		m_y = playerMo->y - m_h/2;
    }
    m_x2 = m_x + m_w;
    m_y2 = m_y + m_h;

    // Change the scaling multipliers

	temp.h.intbits = f_w;
    scale_mtof = FixedDiv(temp.w, m_w);
    scale_ftom = FixedDiv(FRACUNIT, scale_mtof);
}

//
// adds a marker at the current location
//
void AM_addMark(void)
{
    markpoints[markpointnum].x = m_x + m_w/2;
    markpoints[markpointnum].y = m_y + m_h/2;
    markpointnum = (markpointnum + 1) % AM_NUMMARKPOINTS;

}

//
// Determines bounding box of all vertices,
// sets global variables controlling zoom range.
//
void AM_findMinMaxBoundaries(void)
{
	int16_t i;
    fixed_t a;
    fixed_t b;
	fixed_t_union temp;
	vertex_t* vertexes = (vertex_t*)Z_LoadBytesFromEMS(vertexesRef);
	min_x = min_y =  MAXLONG;
    max_x = max_y = -MAXLONG;
	temp.h.fracbits = 0;

    for (i=0;i<numvertexes;i++) {

		temp.h.intbits = vertexes[i].x;

		if ((temp.w) < min_x)
			min_x = temp.w;
		else if ((temp.w) > max_x)
			max_x = temp.w;
    
		temp.h.intbits = vertexes[i].y;

		if (temp.w < min_y)
			min_y = temp.w;
		else if (temp.w > max_y)
			max_y = temp.w;

    }
  
    max_w = max_x - min_x;
    max_h = max_y - min_y;

	temp.h.intbits = f_w;
	a = FixedDiv(temp.w, max_w);
	temp.h.intbits = f_h;
	b = FixedDiv(temp.w, max_h);
  
    min_scale_mtof = a < b ? a : b;
	max_scale_mtof = FixedDiv(temp.w, 2*PLAYERRADIUS);

}


//
//
//
void AM_changeWindowLoc(void)
{
    if (m_paninc.x || m_paninc.y)
    {
	followplayer = 0;
	f_oldloc.x = MAXLONG;
    }

    m_x += m_paninc.x;
    m_y += m_paninc.y;

    if (m_x + m_w/2 > max_x)
	m_x = max_x - m_w/2;
    else if (m_x + m_w/2 < min_x)
	m_x = min_x - m_w/2;
  
    if (m_y + m_h/2 > max_y)
	m_y = max_y - m_h/2;
    else if (m_y + m_h/2 < min_y)
	m_y = min_y - m_h/2;

    m_x2 = m_x + m_w;
    m_y2 = m_y + m_h;
}


//
//
//
void AM_initVariables(void)
{
    static event_t st_notify = { ev_keyup, AM_MSGENTERED };
	mobj_t* playerMo; 


    automapactive = true;
    fb = screen0;

    f_oldloc.x = MAXLONG;

    m_paninc.x = m_paninc.y = 0;
    ftom_zoommul = FRACUNIT;
    mtof_zoommul = FRACUNIT;

    m_w = FTOM16(f_w);
    m_h = FTOM16(f_h);

  
	playerMo = (mobj_t*)Z_LoadBytesFromEMS(players.moRef);
	m_x = playerMo->x - m_w/2;
    m_y = playerMo->y - m_h/2;
    AM_changeWindowLoc();

    // for saving & restoring
    old_m_x = m_x;
    old_m_y = m_y;
    old_m_w = m_w;
    old_m_h = m_h;

    // inform the status bar of the change
    ST_Responder(&st_notify);

}

//
// 
//
void AM_loadPics(void)
{
	int8_t i;
	int8_t namebuf[9];
  
    for (i=0;i<10;i++) {
		sprintf(namebuf, "AMMNUM%d", i);
		marknums[i] = W_CacheLumpNameEMS(namebuf, PU_STATIC);
    }

}

void AM_unloadPics(void)
{
	int8_t i;
  
    for (i=0;i<10;i++)
	Z_ChangeTagEMSNew(marknums[i], PU_CACHE);

}

void AM_clearMarks(void)
{
	int8_t i;

    for (i=0;i<AM_NUMMARKPOINTS;i++)
	markpoints[i].x = -1; // means empty
    markpointnum = 0;
}

//
// should be called at the start of every level
// right now, i figure it out myself
//
void AM_LevelInit(void)
{
    leveljuststarted = 0;

    f_x = f_y = 0;
    f_w = finit_width;
    f_h = finit_height;

    AM_clearMarks();

    AM_findMinMaxBoundaries();
    scale_mtof = FixedDiv(min_scale_mtof, (int32_t) (0.7*FRACUNIT));
	if (scale_mtof > max_scale_mtof) {
		scale_mtof = min_scale_mtof;
	}
    scale_ftom = FixedDiv(FRACUNIT, scale_mtof);
}




//
//
//
void AM_Stop (void)
{
    static event_t st_notify = { 0, ev_keyup, AM_MSGEXITED };

    AM_unloadPics();
    automapactive = false;
    ST_Responder(&st_notify);
    stopped = true;
}

//
//
//
void AM_Start (void)
{
    static int8_t lastlevel = -1, lastepisode = -1;

    if (!stopped) AM_Stop();
    stopped = false;
    if (lastlevel != gamemap || lastepisode != gameepisode)
    {
	AM_LevelInit();
	lastlevel = gamemap;
	lastepisode = gameepisode;
    }
    AM_initVariables();
    AM_loadPics();
}

//
// set the window scale to the maximum size
//
void AM_minOutWindowScale(void)
{
    scale_mtof = min_scale_mtof;
    scale_ftom = FixedDiv(FRACUNIT, scale_mtof);
    AM_activateNewScale();
}

//
// set the window scale to the minimum size
//
void AM_maxOutWindowScale(void)
{
    scale_mtof = max_scale_mtof;
    scale_ftom = FixedDiv(FRACUNIT, scale_mtof);
    AM_activateNewScale();
}


//
// Handle events (user inputs) in automap mode
//
boolean
AM_Responder
( event_t*	ev )
{

	boolean rc;
    static int8_t bigstate=0;
	static int8_t buffer[20];
	int8_t text[100];

    rc = false;

    if (!automapactive)
    {
	if (ev->type == ev_keydown && ev->data1 == AM_STARTKEY)
	{
	    AM_Start ();
	    viewactive = false;
	    rc = true;
	}
    }

    else if (ev->type == ev_keydown)
    {

	rc = true;
	switch(ev->data1)
	{
	  case AM_PANRIGHTKEY: // pan right
	    if (!followplayer) m_paninc.x = FTOM(F_PANINC);
	    else rc = false;
	    break;
	  case AM_PANLEFTKEY: // pan left
	    if (!followplayer) m_paninc.x = -FTOM(F_PANINC);
	    else rc = false;
	    break;
	  case AM_PANUPKEY: // pan up
	    if (!followplayer) m_paninc.y = FTOM(F_PANINC);
	    else rc = false;
	    break;
	  case AM_PANDOWNKEY: // pan down
	    if (!followplayer) m_paninc.y = -FTOM(F_PANINC);
	    else rc = false;
	    break;
	  case AM_ZOOMOUTKEY: // zoom out
	    mtof_zoommul = M_ZOOMOUT;
	    ftom_zoommul = M_ZOOMIN;
	    break;
	  case AM_ZOOMINKEY: // zoom in
	    mtof_zoommul = M_ZOOMIN;
	    ftom_zoommul = M_ZOOMOUT;
	    break;
	  case AM_ENDKEY:
	    bigstate = 0;
	    viewactive = true;
	    AM_Stop ();
	    break;
	  case AM_GOBIGKEY:
	    bigstate = !bigstate;
	    if (bigstate)
	    {
		AM_saveScaleAndLoc();
		AM_minOutWindowScale();
	    }
	    else AM_restoreScaleAndLoc();
	    break;
	  case AM_FOLLOWKEY:
	    followplayer = !followplayer;
	    f_oldloc.x = MAXLONG;
		players.message = followplayer ? AMSTR_FOLLOWON : AMSTR_FOLLOWOFF;
	    break;
	  case AM_GRIDKEY:
	    grid = !grid;
		players.message = grid ? AMSTR_GRIDON : AMSTR_GRIDOFF;
	    break;
	  case AM_MARKKEY:
		getStringByIndex(AMSTR_MARKEDSPOT, text);
	    sprintf(buffer, "%s %d", text, markpointnum);
		players.messagestring = buffer;
	    AM_addMark();
	    break;
	  case AM_CLEARMARKKEY:
	    AM_clearMarks();
		players.message = AMSTR_MARKSCLEARED;
	    break;
	  default:
	    rc = false;
	}
	if ( cht_CheckCheat(&cheat_amap, ev->data1))
	{
	    rc = false;
	    cheating = (cheating+1) % 3;
	}
    }

    else if (ev->type == ev_keyup)
    {
	rc = false;
	switch (ev->data1)
	{
	  case AM_PANRIGHTKEY:
	    if (!followplayer) m_paninc.x = 0;
	    break;
	  case AM_PANLEFTKEY:
	    if (!followplayer) m_paninc.x = 0;
	    break;
	  case AM_PANUPKEY:
	    if (!followplayer) m_paninc.y = 0;
	    break;
	  case AM_PANDOWNKEY:
	    if (!followplayer) m_paninc.y = 0;
	    break;
	  case AM_ZOOMOUTKEY:
	  case AM_ZOOMINKEY:
	    mtof_zoommul = FRACUNIT;
	    ftom_zoommul = FRACUNIT;
	    break;
	}
    }

    return rc;

}


//
// Zooming
//
void AM_changeWindowScale(void)
{

    // Change the scaling multipliers
    scale_mtof = FixedMul(scale_mtof, mtof_zoommul);
    scale_ftom = FixedDiv(FRACUNIT, scale_mtof);

    if (scale_mtof < min_scale_mtof)
	AM_minOutWindowScale();
    else if (scale_mtof > max_scale_mtof)
	AM_maxOutWindowScale();
    else
	AM_activateNewScale();
}


//
//
//
void AM_doFollowPlayer(void) {

	mobj_t* playerMo;

	playerMo = (mobj_t*)Z_LoadBytesFromEMS(players.moRef);

    if (f_oldloc.x != playerMo->x || f_oldloc.y != playerMo->y)
    {
	m_x = FTOM(MTOF(playerMo->x)) - m_w/2;
	m_y = FTOM(MTOF(playerMo->y)) - m_h/2;
	m_x2 = m_x + m_w;
	m_y2 = m_y + m_h;
	f_oldloc.x = playerMo->x;
	f_oldloc.y = playerMo->y;


    }

}

 

//
// Updates on Game Tick
//
void AM_Ticker (void)
{

    if (!automapactive)
	return;

    
    if (followplayer)
	AM_doFollowPlayer();

    // Change the zoom if necessary
    if (ftom_zoommul != FRACUNIT)
	AM_changeWindowScale();

    // Change x,y location
    if (m_paninc.x || m_paninc.y)
	AM_changeWindowLoc();

    // Update light level
    // AM_updateLightLev();

}


//
// Clear automap frame buffer.
//
void AM_clearFB(int16_t color)
{
    memset(fb, color, f_w*f_h);
}


//
// Automap clipping of lines.
//
// Based on Cohen-Sutherland clipping algorithm but with a slightly
// faster reject and precalculated slopes.  If the speed is needed,
// use a hash algorithm to handle  the common cases.
//

#define  LEFT	1
#define  RIGHT	2
#define  BOTTOM	4
#define  TOP	8

boolean
AM_clipMline
( mline_t*	ml,
  fline_t*	fl )
{
    
    register	outcode1 = 0;
    register	outcode2 = 0;
    register	outside;
    
    fpoint_t	tmp;
    fixed_t		dx;
    fixed_t		dy;

    
#define DOOUTCODE(oc, mx, my) \
    (oc) = 0; \
    if ((my) < 0) (oc) |= TOP; \
    else if ((my) >= f_h) (oc) |= BOTTOM; \
    if ((mx) < 0) (oc) |= LEFT; \
    else if ((mx) >= f_w) (oc) |= RIGHT;

    
    // do trivial rejects and outcodes
    if (ml->a.y > m_y2)
	outcode1 = TOP;
    else if (ml->a.y < m_y)
	outcode1 = BOTTOM;

    if (ml->b.y > m_y2)
	outcode2 = TOP;
    else if (ml->b.y < m_y)
	outcode2 = BOTTOM;
    
    if (outcode1 & outcode2)
	return false; // trivially outside

    if (ml->a.x < m_x)
	outcode1 |= LEFT;
    else if (ml->a.x > m_x2)
	outcode1 |= RIGHT;
    
    if (ml->b.x < m_x)
	outcode2 |= LEFT;
    else if (ml->b.x > m_x2)
	outcode2 |= RIGHT;
    
    if (outcode1 & outcode2)
	return false; // trivially outside

    // transform to frame-buffer coordinates.
    fl->a.x = CXMTOF(ml->a.x);
    fl->a.y = CYMTOF(ml->a.y);
    fl->b.x = CXMTOF(ml->b.x);
    fl->b.y = CYMTOF(ml->b.y);

    DOOUTCODE(outcode1, fl->a.x, fl->a.y);
    DOOUTCODE(outcode2, fl->b.x, fl->b.y);

    if (outcode1 & outcode2)
	return false;

    while (outcode1 | outcode2)
    {
	// may be partially inside box
	// find an outside point
	if (outcode1)
	    outside = outcode1;
	else
	    outside = outcode2;
	
	// clip to each side
	if (outside & TOP)
	{
	    dy = fl->a.y - fl->b.y;
	    dx = fl->b.x - fl->a.x;
	    tmp.x = fl->a.x + (dx*(fl->a.y))/dy;
	    tmp.y = 0;
	}
	else if (outside & BOTTOM)
	{
	    dy = fl->a.y - fl->b.y;
	    dx = fl->b.x - fl->a.x;
	    tmp.x = fl->a.x + (dx*(fl->a.y-f_h))/dy;
	    tmp.y = f_h-1;
	}
	else if (outside & RIGHT)
	{
	    dy = fl->b.y - fl->a.y;
	    dx = fl->b.x - fl->a.x;
	    tmp.y = fl->a.y + (dy*(f_w-1 - fl->a.x))/dx;
	    tmp.x = f_w-1;
	}
	else if (outside & LEFT)
	{
	    dy = fl->b.y - fl->a.y;
	    dx = fl->b.x - fl->a.x;
	    tmp.y = fl->a.y + (dy*(-fl->a.x))/dx;
	    tmp.x = 0;
	}

	if (outside == outcode1)
	{
	    fl->a = tmp;
	    DOOUTCODE(outcode1, fl->a.x, fl->a.y);
	}
	else
	{
	    fl->b = tmp;
	    DOOUTCODE(outcode2, fl->b.x, fl->b.y);
	}
	
	if (outcode1 & outcode2)
	    return false; // trivially outside
    }

    return true;
}
#undef DOOUTCODE


//
// Classic Bresenham w/ whatever optimizations needed for speed
//
void
AM_drawFline
( fline_t*	fl,
  uint8_t		color )
{
    register int32_t x;
    register int32_t y;
    register int32_t dx;
    register int32_t dy;
    register int32_t sx;
    register int32_t sy;
    register int32_t ax;
    register int32_t ay;
    register int32_t d;
    
	 

#define PUTDOT(xx,yy,cc) fb[(yy)*f_w+(xx)]=(cc)

    dx = fl->b.x - fl->a.x;
    ax = 2 * (dx<0 ? -dx : dx);
    sx = dx<0 ? -1 : 1;

    dy = fl->b.y - fl->a.y;
    ay = 2 * (dy<0 ? -dy : dy);
    sy = dy<0 ? -1 : 1;

    x = fl->a.x;
    y = fl->a.y;

    if (ax > ay)
    {
	d = ay - ax/2;
	while (1)
	{
	    PUTDOT(x,y,color);
	    if (x == fl->b.x) return;
	    if (d>=0)
	    {
		y += sy;
		d -= ax;
	    }
	    x += sx;
	    d += ay;
	}
    }
    else
    {
	d = ax - ay/2;
	while (1)
	{
	    PUTDOT(x, y, color);
	    if (y == fl->b.y) return;
	    if (d >= 0)
	    {
		x += sx;
		d -= ay;
	    }
	    y += sy;
	    d += ax;
	}
    }
}


//
// Clip lines, draw visible part sof lines.
//
void
AM_drawMline
( mline_t*	ml,
  uint8_t		color )
{
    static fline_t fl;

    if (AM_clipMline(ml, &fl))
	AM_drawFline(&fl, color); // draws it on frame buffer using fb coords
}



//
// Draws flat (floor/ceiling tile) aligned grid lines.
//
void AM_drawGrid(uint8_t color)
{
    fixed_t x, y;
    fixed_t start, end;
    mline_t ml;

    // Figure out start of vertical gridlines
    start = m_x;
	
	if ((start - bmaporgx) % (0x800000)) {
		start += (0x800000) - ((start - bmaporgx) % (0x800000));
	}
    end = m_x + m_w;

    // draw vertical gridlines
    ml.a.y = m_y;
    ml.b.y = m_y+m_h;
    for (x=start; x<end; x+=(0x800000)) {
		ml.a.x = x;
		ml.b.x = x;
		AM_drawMline(&ml, color);
    }

    // Figure out start of horizontal gridlines
    start = m_y;
	if ((start - bmaporgy) % (0x800000)) {
		start += (0x800000) - ((start - bmaporgy) % (0x800000));
	}
    end = m_y + m_h;

    // draw horizontal gridlines
    ml.a.x = m_x;
    ml.b.x = m_x + m_w;
    for (y=start; y<end; y+=(0x800000)) {
		ml.a.y = y;
		ml.b.y = y;
		AM_drawMline(&ml, color);
    }

}

//
// Determines visible lines, draws them.
// This is LineDef based, not LineSeg based.
//
void AM_drawWalls(void)
{
	uint16_t i;
    static mline_t l;
	line_t* lines;
	int16_t linev1Offset;
	int16_t linev2Offset;
	int16_t lineflags;
	int16_t linefrontsecnum;
	int16_t linebacksecnum;
	int16_t linespecial;
	sector_t* sectors;
	boolean floorheightnonequal;
	boolean ceilingheightnonequal;
	fixed_t_union temp;
	vertex_t* vertexes;
	temp.h.fracbits = 0;

    for (i=0;i<numlines;i++) {
		lines = (line_t*)Z_LoadBytesFromEMS(linesRef);
		linev1Offset = lines[i].v1Offset;
		linev2Offset = lines[i].v2Offset;
		lineflags = lines[i].flags;
		linebacksecnum = lines[i].backsecnum;
		linefrontsecnum = lines[i].frontsecnum;
		linespecial = lines[i].special;

		vertexes = (vertex_t*)Z_LoadBytesFromEMS(vertexesRef);
		temp.h.intbits = vertexes[linev1Offset].x;
		l.a.x = temp.w;
		temp.h.intbits = vertexes[linev1Offset].y;
		l.a.y = temp.w;
		temp.h.intbits = vertexes[linev2Offset].x;
		l.b.x = temp.w;
		temp.h.intbits = vertexes[linev2Offset].y;
		l.b.y = temp.w;

		if (cheating || (lineflags & ML_MAPPED)) {
			if ((lineflags & LINE_NEVERSEE) && !cheating) {
				continue;
			} if (linebacksecnum == SECNUM_NULL) {
				AM_drawMline(&l, WALLCOLORS);
			} else {
				sectors = (sector_t*)Z_LoadBytesFromEMS(sectorsRef);
				floorheightnonequal = sectors[linebacksecnum].floorheight != sectors[linefrontsecnum].floorheight;
				ceilingheightnonequal = sectors[linebacksecnum].ceilingheight != sectors[linefrontsecnum].ceilingheight;
				if (linespecial == 39) { // teleporters
					AM_drawMline(&l, WALLCOLORS+WALLRANGE/2);
				} else if (lineflags & ML_SECRET){ // secret door
					if (cheating) { 
						AM_drawMline(&l, SECRETWALLCOLORS); 
					} else {
						AM_drawMline(&l, WALLCOLORS);
					}
				} else if (floorheightnonequal) {
					AM_drawMline(&l, FDWALLCOLORS); // floor level change
				}
				else if (ceilingheightnonequal) {
					AM_drawMline(&l, CDWALLCOLORS); // ceiling level change
				} else if (cheating) {
					AM_drawMline(&l, TSWALLCOLORS);
				}
			}
		} else if (players.powers[pw_allmap]) {
			if (!(lineflags & LINE_NEVERSEE)) {
				AM_drawMline(&l, GRAYS + 3);
			}
		}
    }
}


//
// Rotation in 2D.
// Used to rotate player arrow line character.
//
void
AM_rotate
( fixed_t*	x,
  fixed_t*	y,
  fineangle_t	a )
{
    fixed_t tmpx;
    tmpx =
	FixedMul(*x,finecosine(a))
	- FixedMul(*y,finesine(a));
    
    *y   =
	FixedMul(*x,finesine(a))
	+ FixedMul(*y,finecosine(a));

    *x = tmpx;
}

void
AM_drawLineCharacter
( mline_t*	lineguy,
  int16_t		lineguylines,
  fixed_t	scale,
  fineangle_t	angle,
  uint8_t		color,
  fixed_t	x,
  fixed_t	y )
{
    int16_t		i;
    mline_t	l;

    for (i=0;i<lineguylines;i++)
    {
	l.a.x = lineguy[i].a.x;
	l.a.y = lineguy[i].a.y;

	if (scale)
	{
	    l.a.x = FixedMul(scale, l.a.x);
	    l.a.y = FixedMul(scale, l.a.y);
	}

	if (angle)
	    AM_rotate(&l.a.x, &l.a.y, angle);

	l.a.x += x;
	l.a.y += y;

	l.b.x = lineguy[i].b.x;
	l.b.y = lineguy[i].b.y;

	if (scale)
	{
	    l.b.x = FixedMul(scale, l.b.x);
	    l.b.y = FixedMul(scale, l.b.y);
	}

	if (angle)
	    AM_rotate(&l.b.x, &l.b.y, angle);
	
	l.b.x += x;
	l.b.y += y;

	AM_drawMline(&l, color);
    }
}

void AM_drawPlayers(void)
{
	static uint8_t their_colors[] = { GREENS, GRAYS, BROWNS, REDS };
	uint8_t color;
	mobj_t* playerMo;
	playerMo = (mobj_t*)Z_LoadBytesFromEMS(players.moRef);
	if (cheating)
		AM_drawLineCharacter(cheat_player_arrow, NUMCHEATPLYRLINES, 0, playerMo->angle>>ANGLETOFINESHIFT, WHITE, playerMo->x, playerMo->y);
	else
		AM_drawLineCharacter(player_arrow, NUMPLYRLINES, 0, playerMo->angle>>ANGLETOFINESHIFT, WHITE, playerMo->x, playerMo->y);



}

void
AM_drawThings
( uint8_t	colors)
{
    uint16_t		i;
    mobj_t*	t;
	MEMREF tRef;
	sector_t* sectors;
	for (i=0;i<numsectors;i++) {
		sectors = (sector_t*)Z_LoadBytesFromEMS(sectorsRef);
		tRef = sectors[i].thinglistRef;
		while (tRef) {
			t = (mobj_t*) Z_LoadBytesFromEMS(tRef);
			
			AM_drawLineCharacter (thintriangle_guy, NUMTHINTRIANGLEGUYLINES,
			 0x100000L, t->angle>>ANGLETOFINESHIFT, colors, t->x, t->y);
			tRef = t->snextRef;
		}
    }
}

void AM_drawMarks(void)
{
	int8_t i, w, h;
	int16_t fx, fy;

    for (i=0;i<AM_NUMMARKPOINTS;i++)
    {
	if (markpoints[i].x != -1)
	{
	    //      w = (marknums[i]->width);
	    //      h = (marknums[i]->height);
	    w = 5; // because something's wrong with the wad, i guess
	    h = 6; // because something's wrong with the wad, i guess
	    fx = CXMTOF(markpoints[i].x);
	    fy = CYMTOF(markpoints[i].y);
		if (fx >= f_x && fx <= f_w - w && fy >= f_y && fy <= f_h - h) {
			V_DrawPatch(fx, fy, FB, (patch_t*)Z_LoadBytesFromEMS(marknums[i]));
		}
	}
    }

}

void AM_drawCrosshair(uint8_t color)
{
    fb[(f_w*(f_h+1))/2] = color; // single point for now

}

void AM_Drawer (void)
{
    if (!automapactive) return;

    AM_clearFB(BACKGROUND);
    if (grid)
	AM_drawGrid(GRIDCOLORS);
    AM_drawWalls();
    AM_drawPlayers();
    if (cheating==2)
	AM_drawThings(THINGCOLORS);
    AM_drawCrosshair(XHAIRCOLORS);

    AM_drawMarks();

    V_MarkRect(f_x, f_y, f_w, f_h);

}
