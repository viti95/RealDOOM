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
//	Game completion, final screen animation.
//

#include <ctype.h>

// Functions.
#include "i_system.h"
#include "z_zone.h"
#include "v_video.h"
#include "w_wad.h"
#include "s_sound.h"

// Data.
#include "dstrings.h"
#include "sounds.h"

#include "doomstat.h"
#include "r_state.h"


#include <alloca.h>


// Stage of animation:
//  0 = text, 1 = art screen, 2 = character cast
int16_t		finalestage;

int16_t		finalecount;

#define	TEXTSPEED	3
#define	TEXTWAIT	250

int16_t	e1text = E1TEXT;
int16_t	e2text = E2TEXT;
int16_t	e3text = E3TEXT;
#if (EXE_VERSION >= EXE_VERSION_ULTIMATE)
int8_t*	e4text = E4TEXT;
#endif

int16_t	c1text = C1TEXT;
int16_t	c2text = C2TEXT;
int16_t	c3text = C3TEXT;
int16_t	c4text = C4TEXT;
int16_t	c5text = C5TEXT;
int16_t	c6text = C6TEXT;

#if (EXE_VERSION >= EXE_VERSION_FINAL)
int16_t	p1text = P1TEXT;
int16_t	p2text = P2TEXT;
int16_t	p3text = P3TEXT;
int16_t	p4text = P4TEXT;
int16_t	p5text = P5TEXT;
int16_t	p6text = P6TEXT;

int16_t	t1text = T1TEXT;
int16_t	t2text = T2TEXT;
int16_t	t3text = T3TEXT;
int16_t	t4text = T4TEXT;
int16_t	t5text = T5TEXT;
int16_t	t6text = T6TEXT;
#endif

int16_t	finaletext;
int8_t*	finaleflat;

void	F_StartCast (void);
void	F_CastTicker (void);
boolean F_CastResponder (event_t *ev);
void	F_CastDrawer (void);

//
// F_StartFinale
//
void F_StartFinale (void)
{
	int16_t finalemusic;

    gameaction = ga_nothing;
    gamestate = GS_FINALE;
    viewactive = false;
    automapactive = false;

    if(commercial)
    {
#if (EXE_VERSION < EXE_VERSION_FINAL)
        // DOOM II and missions packs with E1, M34
	  switch (gamemap)
	  {
	    case 6:
	      finaleflat = "SLIME16";
	      finaletext = c1text;
	      break;
	    case 11:
	      finaleflat = "RROCK14";
	      finaletext = c2text;
	      break;
	    case 20:
	      finaleflat = "RROCK07";
	      finaletext = c3text;
	      break;
	    case 30:
	      finaleflat = "RROCK17";
	      finaletext = c4text;
	      break;
	    case 15:
	      finaleflat = "RROCK13";
	      finaletext = c5text;
	      break;
	    case 31:
	      finaleflat = "RROCK19";
	      finaletext = c6text;
	      break;
	    default:
	      // Ouch.
	      break;
	  }
#else
        if (plutonia)
        {
	  switch (gamemap)
	  {
	    case 6:
	      finaleflat = "SLIME16";
	      finaletext = p1text;
	      break;
	    case 11:
	      finaleflat = "RROCK14";
	      finaletext = p2text;
	      break;
	    case 20:
	      finaleflat = "RROCK07";
	      finaletext = p3text;
	      break;
	    case 30:
	      finaleflat = "RROCK17";
	      finaletext = p4text;
	      break;
	    case 15:
	      finaleflat = "RROCK13";
	      finaletext = p5text;
	      break;
	    case 31:
	      finaleflat = "RROCK19";
	      finaletext = p6text;
	      break;
	    default:
	      // Ouch.
	      break;
	  }
        }
        else if (tnt)
        {
	  switch (gamemap)
	  {
	    case 6:
	      finaleflat = "SLIME16";
	      finaletext = t1text;
	      break;
	    case 11:
	      finaleflat = "RROCK14";
	      finaletext = t2text;
	      break;
	    case 20:
	      finaleflat = "RROCK07";
	      finaletext = t3text;
	      break;
	    case 30:
	      finaleflat = "RROCK17";
	      finaletext = t4text;
	      break;
	    case 15:
	      finaleflat = "RROCK13";
	      finaletext = t5text;
	      break;
	    case 31:
	      finaleflat = "RROCK19";
	      finaletext = t6text;
	      break;
	    default:
	      // Ouch.
	      break;
	  }
        }
        else
        {
	  // DOOM II and missions packs with E1, M34
	  switch (gamemap)
	  {
	    case 6:
	      finaleflat = "SLIME16";
	      finaletext = c1text;
	      break;
	    case 11:
	      finaleflat = "RROCK14";
	      finaletext = c2text;
	      break;
	    case 20:
	      finaleflat = "RROCK07";
	      finaletext = c3text;
	      break;
	    case 30:
	      finaleflat = "RROCK17";
	      finaletext = c4text;
	      break;
	    case 15:
	      finaleflat = "RROCK13";
	      finaletext = c5text;
	      break;
	    case 31:
	      finaleflat = "RROCK19";
	      finaletext = c6text;
	      break;
	    default:
	      // Ouch.
	      break;
	  }
        }
#endif
	finalemusic = mus_read_m;
    }
    else
    {
	// DOOM 1 - E1, E3 or E4, but each nine missions
	switch (gameepisode)
	{
	  case 1:
	    finaleflat = "FLOOR4_8";
	    finaletext = e1text;
	    break;
	  case 2:
	    finaleflat = "SFLR6_1";
	    finaletext = e2text;
	    break;
	  case 3:
	    finaleflat = "MFLR8_4";
	    finaletext = e3text;
	    break;
#if (EXE_VERSION >= EXE_VERSION_ULTIMATE)
	  case 4:
	    finaleflat = "MFLR8_3";
	    finaletext = e4text;
	    break;
#endif
	  default:
	    // Ouch.
	    break;
	}
	finalemusic = mus_victor;
    }
    
    S_ChangeMusic(finalemusic, true);
    finalestage = 0;
    finalecount = 0;
}



boolean F_Responder (event_t *event)
{
    if (finalestage == 2)
	return F_CastResponder (event);
	
    return false;
}


//
// F_Ticker
//
void F_Ticker (void)
{
	int8_t* text;
    // check for skipping
    if ( (commercial)
      && ( finalecount > 50) )
    {
      // go on to the next level
				
		if (players.cmd.buttons)
		{
	if (gamemap == 30)
	  F_StartCast ();
	else
	  gameaction = ga_worlddone;
      }
    }
    
    // advance animation
    finalecount++;
	
    if (finalestage == 2)
    {
	F_CastTicker ();
	return;
    }
	
    if (commercial)
	return;
	
	getStringByIndex(finaletext, text);
    if (!finalestage && finalecount>strlen (text)*TEXTSPEED + TEXTWAIT)
    {
	finalecount = 0;
	finalestage = 1;
	wipegamestate = -1;		// force a wipe
	if (gameepisode == 3)
	    S_StartMusic (mus_bunny);
    }
}



//
// F_TextWrite
//

#include "hu_stuff.h"
extern	MEMREF hu_fontRef[HU_FONTSIZE];


void F_TextWrite (void)
{
    byte*	src;
    byte*	dest;
    
    int16_t		x,y,w;
    int16_t		count;
    int8_t*	ch;
    int16_t		c;
    int16_t		cx;
    int16_t		cy;
	MEMREF	srcRef;
	patch_t* hu_fontC;
    // erase the entire screen to a tiled background
    srcRef = W_CacheLumpNameEMS ( finaleflat , PU_CACHE);
	src = Z_LoadBytesFromEMS(srcRef);
    dest = screen0;
	
    for (y=0 ; y<SCREENHEIGHT ; y++) {
		for (x=0 ; x<SCREENWIDTH/64 ; x++) {
			memcpy (dest, src+((y&63)<<6), 64);
			dest += 64;
		}
	 
    }

    V_MarkRect (0, 0, SCREENWIDTH, SCREENHEIGHT);
    
    // draw some of the text onto the screen
    cx = 10;
    cy = 10;
    getStringByIndex(finaletext, ch);
	
    count = (finalecount - 10)/TEXTSPEED;
    if (count < 0)
	count = 0;
    for ( ; count ; count-- ) {
		c = *ch++;
		if (!c)
			break;
		if (c == '\n') {
			cx = 10;
			cy += 11;
			continue;
		}
			
		c = toupper(c) - HU_FONTSTART;
		if (c < 0 || c> HU_FONTSIZE) {
			cx += 4;
			continue;
		}
			
		hu_fontC = Z_LoadBytesFromEMS(hu_fontRef[c]);
		w =  (hu_fontC->width);
		if (cx+w > SCREENWIDTH)
			break;
		V_DrawPatch(cx, cy, 0, hu_fontC);
		cx+=w;
    }
	
}

//
// Final DOOM 2 animation
// Casting by id Software.
//   in order of appearance
//
typedef struct
{
	int16_t		nameindex;
    mobjtype_t	type;
} castinfo_t;

castinfo_t	castorder[] = {
    {CC_ZOMBIE, MT_POSSESSED},
    {CC_SHOTGUN, MT_SHOTGUY},
    {CC_HEAVY, MT_CHAINGUY},
    {CC_IMP, MT_TROOP},
    {CC_DEMON, MT_SERGEANT},
    {CC_LOST, MT_SKULL},
    {CC_CACO, MT_HEAD},
    {CC_HELL, MT_KNIGHT},
    {CC_BARON, MT_BRUISER},
    {CC_ARACH, MT_BABY},
    {CC_PAIN, MT_PAIN},
    {CC_REVEN, MT_UNDEAD},
    {CC_MANCU, MT_FATSO},
    {CC_ARCH, MT_VILE},
    {CC_SPIDER, MT_SPIDER},
    {CC_CYBER, MT_CYBORG},
    {CC_HERO, MT_PLAYER},

    {-1,0}
};

int8_t		castnum;
int8_t		casttics;
state_t*	caststate;
boolean		castdeath;
int8_t		castframes;
int8_t		castonmelee;
boolean		castattacking;


//
// F_StartCast
//
extern	gamestate_t     wipegamestate;


void F_StartCast (void)
{
    wipegamestate = -1;		// force a screen wipe
    castnum = 0;
    caststate = &states[mobjinfo[castorder[castnum].type].seestate];
    casttics = caststate->tics;
    castdeath = false;
    finalestage = 2;	
    castframes = 0;
    castonmelee = 0;
    castattacking = false;
    S_ChangeMusic(mus_evil, true);
}


//
// F_CastTicker
//
void F_CastTicker (void)
{
    int16_t		st;
    int16_t		sfx;
	
    if (--casttics > 0)
	return;			// not time to change state yet
		
    if (caststate->tics == -1 || caststate->nextstate == S_NULL)
    {
	// switch from deathstate to next monster
	castnum++;
	castdeath = false;
	if (castorder[castnum].nameindex == -1)
	    castnum = 0;
	if (mobjinfo[castorder[castnum].type].seesound)
	    S_StartSound (NULL, mobjinfo[castorder[castnum].type].seesound);
	caststate = &states[mobjinfo[castorder[castnum].type].seestate];
	castframes = 0;
    }
    else
    {
	// just advance to next state in animation
	if (caststate == &states[S_PLAY_ATK1])
	    goto stopattack;	// Oh, gross hack!
	st = caststate->nextstate;
	caststate = &states[st];
	castframes++;
	
	// sound hacks....
	switch (st)
	{
	  case S_PLAY_ATK1:	sfx = sfx_dshtgn; break;
	  case S_POSS_ATK2:	sfx = sfx_pistol; break;
	  case S_SPOS_ATK2:	sfx = sfx_shotgn; break;
	  case S_VILE_ATK2:	sfx = sfx_vilatk; break;
	  case S_SKEL_FIST2:	sfx = sfx_skeswg; break;
	  case S_SKEL_FIST4:	sfx = sfx_skepch; break;
	  case S_SKEL_MISS2:	sfx = sfx_skeatk; break;
	  case S_FATT_ATK8:
	  case S_FATT_ATK5:
	  case S_FATT_ATK2:	sfx = sfx_firsht; break;
	  case S_CPOS_ATK2:
	  case S_CPOS_ATK3:
	  case S_CPOS_ATK4:	sfx = sfx_shotgn; break;
	  case S_TROO_ATK3:	sfx = sfx_claw; break;
	  case S_SARG_ATK2:	sfx = sfx_sgtatk; break;
	  case S_BOSS_ATK2:
	  case S_BOS2_ATK2:
	  case S_HEAD_ATK2:	sfx = sfx_firsht; break;
	  case S_SKULL_ATK2:	sfx = sfx_sklatk; break;
	  case S_SPID_ATK2:
	  case S_SPID_ATK3:	sfx = sfx_shotgn; break;
	  case S_BSPI_ATK2:	sfx = sfx_plasma; break;
	  case S_CYBER_ATK2:
	  case S_CYBER_ATK4:
	  case S_CYBER_ATK6:	sfx = sfx_rlaunc; break;
	  case S_PAIN_ATK3:	sfx = sfx_sklatk; break;
	  default: sfx = 0; break;
	}
		
	if (sfx)
	    S_StartSound (NULL, sfx);
    }
	
    if (castframes == 12)
    {
	// go into attack frame
	castattacking = true;
	if (castonmelee)
	    caststate=&states[getMeleeState(castorder[castnum].type)];
	else
	    caststate=&states[mobjinfo[castorder[castnum].type].missilestate];
	castonmelee ^= 1;
	if (caststate == &states[S_NULL])
	{
	    if (castonmelee)
		caststate=
		    &states[getMeleeState(castorder[castnum].type)];
	    else
		caststate=
		    &states[mobjinfo[castorder[castnum].type].missilestate];
	}
    }
	
    if (castattacking)
    {
	if (castframes == 24
	    ||	caststate == &states[mobjinfo[castorder[castnum].type].seestate] )
	{
	  stopattack:
	    castattacking = false;
	    castframes = 0;
	    caststate = &states[mobjinfo[castorder[castnum].type].seestate];
	}
    }
	
    casttics = caststate->tics;
    if (casttics == -1)
	casttics = 15;
}


//
// F_CastResponder
//

boolean F_CastResponder (event_t* ev)
{
    if (ev->type != ev_keydown)
	return false;
		
    if (castdeath)
	return true;			// already in dying frames
		
    // go into death frame
    castdeath = true;
    caststate = &states[mobjinfo[castorder[castnum].type].deathstate];
    casttics = caststate->tics;
    castframes = 0;
    castattacking = false;
    if (mobjinfo[castorder[castnum].type].deathsound)
	S_StartSound (NULL, mobjinfo[castorder[castnum].type].deathsound);
	
    return true;
}


void F_CastPrint (int8_t* text)
{
    int8_t*	ch;
    int16_t		c;
    int16_t		cx;
    int16_t		w;
    int16_t		width;
	patch_t* hu_fontC;
    
    // find width
    ch = text;
    width = 0;
	
    while (ch)
    {
	c = *ch++;
	if (!c)
	    break;
	c = toupper(c) - HU_FONTSTART;
	if (c < 0 || c> HU_FONTSIZE)
	{
	    width += 4;
	    continue;
	}
		
	hu_fontC = Z_LoadBytesFromEMS(hu_fontRef[c]);

	w = (hu_fontC->width);
	width += w;
    }
    
    // draw it
    cx = 160-width/2;
    ch = text;
    while (ch)
    {
	c = *ch++;
	if (!c)
	    break;
	c = toupper(c) - HU_FONTSTART;
	if (c < 0 || c> HU_FONTSIZE)
	{
	    cx += 4;
	    continue;
	}
		
	hu_fontC = Z_LoadBytesFromEMS(hu_fontRef[c]);
	w =  (hu_fontC->width);
	V_DrawPatch(cx, 180, 0, hu_fontC);
	cx+=w;
    }
	
}


//
// F_CastDrawer
//
void V_DrawPatchFlipped (int16_t x, int16_t y,  patch_t *patch);

void F_CastDrawer (void)
{
    spritedef_t*	sprite;
    spriteframe_t*	sprframe;
    int16_t			lump;
    boolean		flip;
	MEMREF			patchRef;
	patch_t*		patch;
	spritedef_t*	sprites;
	spriteframe_t*  spriteframes;
	int8_t			text[100];

    // erase the entire screen to a background
    V_DrawPatch (0,0,0, W_CacheLumpNameEMSAsPatch("BOSSBACK", PU_CACHE));
	getStringByIndex(castorder[castnum].nameindex, text);
    F_CastPrint (text);
    
    // draw the current frame in the middle of the screen
	sprites = (spritedef_t*) Z_LoadBytesFromEMS(spritesRef);
	sprite = &sprites[caststate->sprite];
	spriteframes = (spriteframe_t*)Z_LoadBytesFromEMS(sprite->spriteframesRef);

	sprframe = &spriteframes[caststate->frame & FF_FRAMEMASK];

	
	lump = sprframe->lump[0];
    flip = (boolean)sprframe->flip[0];
			
	W_CacheLumpNumCheck(lump + firstspritelump, -1);
	patchRef = W_CacheLumpNumEMS (lump+firstspritelump, PU_CACHE);
	patch = Z_LoadBytesFromEMS(patchRef);
	
	if (flip) {
		V_DrawPatchFlipped(160, 170, patch);
	}
	else {
		V_DrawPatch(160, 170, 0, patch);
	}
}


//
// F_DrawPatchCol
//
void
F_DrawPatchCol
( int16_t		x,
  patch_t*	patch,
  int16_t		col )
{
    column_t*	column;
    byte*	source;
    byte*	dest;
    byte*	desttop;
    int16_t		count;
	
    column = (column_t *)((byte *)patch + (patch->columnofs[col]));
    desttop = screen0+x;

    // step through the posts in a column
    while (column->topdelta != 0xff )
    {
	source = (byte *)column + 3;
	dest = desttop + column->topdelta*SCREENWIDTH;
	count = column->length;
		
	while (count--)
	{
	    *dest = *source++;
	    dest += SCREENWIDTH;
	}
	column = (column_t *)(  (byte *)column + column->length + 4 );
    }
}


//
// F_BunnyScroll
//
void F_BunnyScroll (void)
{
    int16_t		scrolled;
    int16_t		x;
	int8_t	name[10];
    int16_t		stage;
    static int16_t	laststage;
		


    V_MarkRect (0, 0, SCREENWIDTH, SCREENHEIGHT);
	
    scrolled = 320 - (finalecount-230)/2;
    if (scrolled > 320)
	scrolled = 320;
    if (scrolled < 0)
	scrolled = 0;
		
    for ( x=0 ; x<SCREENWIDTH ; x++)
    {
	if (x+scrolled < 320)
	    F_DrawPatchCol (x, W_CacheLumpNameEMSAsPatch("PFUB2", PU_LEVEL), x+scrolled);
	else
	    F_DrawPatchCol (x, W_CacheLumpNameEMSAsPatch("PFUB1", PU_LEVEL), x+scrolled - 320);
    }
	
    if (finalecount < 1130)
	return;
    if (finalecount < 1180)
    {
	V_DrawPatch ((SCREENWIDTH-13*8)/2,
		     (SCREENHEIGHT-8*8)/2,0, W_CacheLumpNameEMSAsPatch ("END0",PU_CACHE));
	laststage = 0;
	return;
    }
	
    stage = (finalecount-1180) / 5;
    if (stage > 6)
	stage = 6;
    if (stage > laststage)
    {
	S_StartSound (NULL, sfx_pistol);
	laststage = stage;
    }
	
    sprintf (name,"END%i",stage);
    V_DrawPatch ((SCREENWIDTH-13*8)/2, (SCREENHEIGHT-8*8)/2,0, W_CacheLumpNameEMSAsPatch(name, PU_CACHE));
}


//
// F_Drawer
//
void F_Drawer (void)
{
    if (finalestage == 2)
    {
	F_CastDrawer ();
	return;
    }

    if (!finalestage)
	F_TextWrite ();
    else
    {
	switch (gameepisode)
	{
	  case 1:
#if (EXE_VERSION < EXE_VERSION_ULTIMATE)
	    V_DrawPatch(0,0,0,
			W_CacheLumpNameEMSAsPatch("HELP2", PU_CACHE));
	    break;
#else
		  V_DrawPatch(0, 0, 0,
			  W_CacheLumpNameEMSAsPatch("CREDIT", PU_CACHE));
	    break;
#endif
	  case 2:
	    V_DrawPatch(0,0,0,
			W_CacheLumpNameEMSAsPatch("VICTORY2", PU_CACHE));
	    break;
	  case 3:
	    F_BunnyScroll ();
	    break;
	  case 4:
	    V_DrawPatch (0,0,0,
			W_CacheLumpNameEMSAsPatch("ENDPIC", PU_CACHE));
	    break;
	}
    }
			
}


