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
// DESCRIPTION: Door animation code (opening/closing)
//

#include "z_zone.h"
#include "doomdef.h"
#include "p_local.h"

#include "s_sound.h"


// State.
#include "doomstat.h"
#include "r_state.h"

// Data.
#include "dstrings.h"
#include "sounds.h"
#include "i_system.h"


//
// VERTICAL DOORS
//

//
// T_VerticalDoor
//
void T_VerticalDoor (MEMREF memref)
{
    result_e	res;
	vldoor_t* door = (vldoor_t*)Z_LoadBytesFromEMS(memref);
	int16_t doorsecnum = door->secnum;
	sector_t* sectors = (sector_t*)Z_LoadBytesFromEMS(sectorsRef);
	sector_t doorsector = sectors[doorsecnum];
	THINKERREF doorthinkerRef;
	door = (vldoor_t*)Z_LoadBytesFromEMS(memref);

	switch(door->direction) {
		  case 0:
		// WAITING
		if (!--door->topcountdown) {
			switch(door->type) {
				case blazeRaise:
					door->direction = -1; // time to go back down
					S_StartSoundWithParams(doorsector.soundorgX, doorsector.soundorgY, sfx_bdcls);
					break;
		
				case normal:
					door->direction = -1; // time to go back down
					S_StartSoundWithParams(doorsector.soundorgX, doorsector.soundorgY, sfx_dorcls);
					break;
		
				case close30ThenOpen:
					door->direction = 1;
					S_StartSoundWithParams(doorsector.soundorgX, doorsector.soundorgY, sfx_doropn);
					break;
		
				default:
					break;
			}
		}
		break;
	
		  case 2:
		//  INITIAL WAIT
		if (!--door->topcountdown) {
			switch(door->type) {
				case raiseIn5Mins:
					door->direction = 1;
					door->type = normal;
					S_StartSoundWithParams(doorsector.soundorgX, doorsector.soundorgY, sfx_doropn);
					break;
		
				default:
					break;
			}
		}
		break;
	
		  case -1:
		// DOWN

			res = T_MovePlane(door->secnum, door->speed, doorsector.floorheight, false,1,door->direction);
			door = (vldoor_t*)Z_LoadBytesFromEMS(memref);
			doorthinkerRef = door->thinkerRef;
			if (res == floor_pastdest) {
				switch(door->type) {
					case blazeRaise:
					case blazeClose:
						sectors = (sector_t*)Z_LoadBytesFromEMS(sectorsRef);
						sectors[doorsecnum].specialdataRef = NULL_MEMREF;
						P_RemoveThinker (doorthinkerRef);  // unlink and free
						S_StartSoundWithParams(doorsector.soundorgX, doorsector.soundorgY, sfx_bdcls);
						break;
		
					case normal:
					case close:
						sectors = (sector_t*)Z_LoadBytesFromEMS(sectorsRef);
						sectors[doorsecnum].specialdataRef = NULL_MEMREF;
						P_RemoveThinker (doorthinkerRef);  // unlink and free
						break;
		
					case close30ThenOpen:
						door->direction = 0;
						door->topcountdown = 35*30;
						break;
		
					default:
						break;
				}
			} else if (res == floor_crushed) {
				switch(door->type) {
					case blazeClose:
					case close:		// DO NOT GO BACK UP!
						break;
		
					default:
						door->direction = 1;
						S_StartSoundWithParams(doorsector.soundorgX, doorsector.soundorgY, sfx_doropn);
						break;
				}
			}
			break;
	
			case 1:
				// UP
				res = T_MovePlane(door->secnum,   door->speed, door->topheight, false,1,door->direction);
				door = (vldoor_t*)Z_LoadBytesFromEMS(memref);

				doorthinkerRef = door->thinkerRef;



				if (res == floor_pastdest) {
					switch(door->type) {
						case blazeRaise:
						case normal:
							door->direction = 0; // wait at top
							door->topcountdown = door->topwait;
							break;
		
						case close30ThenOpen:
						case blazeOpen:
						case open:
							sectors = (sector_t*)Z_LoadBytesFromEMS(sectorsRef);
							sectors[doorsecnum].specialdataRef = NULL_MEMREF;
							P_RemoveThinker (doorthinkerRef);  // unlink and free
						break;
		
						default:
							break;
			}
		}
		break;
    }
 
}


//
// EV_DoLockedDoor
// Move a locked door up/down
//

int16_t
EV_DoLockedDoor
( uint8_t linetag,
	int16_t linespecial,
  vldoor_e	type,
  MEMREF thingRef )
{
    player_t*	p;
	mobj_t*	thing = (mobj_t*)Z_LoadBytesFromEMS(thingRef);
	
    p = thing->player;
	
    if (!p)
	return 0;
		
    switch(linespecial)
    {
      case 99:	// Blue Lock
      case 133:
	if ( !p )
	    return 0;
	if (!p->cards[it_bluecard] && !p->cards[it_blueskull])
	{
	    p->message = PD_BLUEO;
	    S_StartSound(NULL,sfx_oof);
	    return 0;
	}
	break;
	
      case 134: // Red Lock
      case 135:
	if ( !p )
	    return 0;
	if (!p->cards[it_redcard] && !p->cards[it_redskull])
	{
	    p->message = PD_REDO;
	    S_StartSound(NULL,sfx_oof);
	    return 0;
	}
	break;
	
      case 136:	// Yellow Lock
      case 137:
	if ( !p )
	    return 0;
	if (!p->cards[it_yellowcard] &&
	    !p->cards[it_yellowskull])
	{
	    p->message = PD_YELLOWO;
	    S_StartSound(NULL,sfx_oof);
	    return 0;
	}
	break;	
    }

    return EV_DoDoor(linetag,type);
}


int16_t
EV_DoDoor
( uint8_t linetag,
  vldoor_e	type )
{
    int16_t		secnum,rtn;
    vldoor_t*	door;
	MEMREF doorRef;
	int16_t doortopheight;
	sector_t* sectors;
	sector_t doorsector;

    secnum = -1;
    rtn = 0;
    
    while ((secnum = P_FindSectorFromLineTag(linetag,secnum)) >= 0) {
		sectors = (sector_t*)Z_LoadBytesFromEMS(sectorsRef);

		if (sectors[secnum].specialdataRef)
			continue;
		
	
		// new door thinker
		rtn = 1;



		doorRef = Z_MallocEMSNew(sizeof(*door), PU_LEVSPEC, 0, ALLOC_TYPE_LEVSPEC);
		sectors[secnum].specialdataRef = doorRef;
		doorsector = sectors[secnum];
		door = (vldoor_t*)Z_LoadBytesFromEMS(doorRef);

		door->thinkerRef = P_AddThinker (doorRef, TF_VERTICALDOOR);
	
		door->secnum = secnum;
		door->type = type;
		door->topwait = VDOORWAIT;
		door->speed = VDOORSPEED;
		
		switch(type)
		{
		  case blazeClose:
			doortopheight = P_FindLowestCeilingSurrounding(secnum);
			door = (vldoor_t*)Z_LoadBytesFromEMS(doorRef);
			door->topheight = doortopheight - (4 << SHORTFLOORBITS);
			door->direction = -1;
			door->speed = VDOORSPEED * 4;
			S_StartSoundWithParams(doorsector.soundorgX, doorsector.soundorgY, sfx_bdcls);
			break;
	    
		  case close:
			doortopheight = P_FindLowestCeilingSurrounding(secnum);
			door = (vldoor_t*)Z_LoadBytesFromEMS(doorRef);
			door->topheight = doortopheight - (4 << SHORTFLOORBITS);
			door->direction = -1;
			S_StartSoundWithParams(doorsector.soundorgX, doorsector.soundorgY, sfx_dorcls);
			break;
	    
		  case close30ThenOpen:
			door->topheight = doorsector.ceilingheight;
			door->direction = -1;
			S_StartSoundWithParams(doorsector.soundorgX, doorsector.soundorgY, sfx_dorcls);
			break;
	    
		  case blazeRaise:
		  case blazeOpen:
			door->direction = 1;
			doortopheight = P_FindLowestCeilingSurrounding(secnum);
			door = (vldoor_t*)Z_LoadBytesFromEMS(doorRef);
			door->topheight = doortopheight - (4 << SHORTFLOORBITS);
			door->speed = VDOORSPEED * 4;
			if (door->topheight != (doorsector.ceilingheight))
				S_StartSoundWithParams(doorsector.soundorgX, doorsector.soundorgY, sfx_bdopn);
			break;
	    
		  case normal:
		  case open:
			door->direction = 1;
			doortopheight = P_FindLowestCeilingSurrounding(secnum);
			door = (vldoor_t*)Z_LoadBytesFromEMS(doorRef);
			door->topheight = doortopheight - (4 << SHORTFLOORBITS);
			if (door->topheight != doorsector.ceilingheight)
				S_StartSoundWithParams(doorsector.soundorgX, doorsector.soundorgY, sfx_doropn);
			break;
	    
		  default:
			break;
		}
		
    }
    return rtn;
}


//
// EV_VerticalDoor : open a door manually, no tag value
//
void
EV_VerticalDoor
( int16_t linenum,
  MEMREF thingRef )
{
    player_t*	player;
    int16_t		secnum;
    //sector_t*	sec;
    vldoor_t*	door;
    int16_t		side = 0;
	MEMREF doorRef;
	line_t* lines = (line_t*)Z_LoadBytesFromEMS(linesRef);
	int16_t linespecial = lines[linenum].special;
	int16_t sidenum;
	mobj_t*	thing = (mobj_t*)Z_LoadBytesFromEMS(thingRef);
	side_t* sides;
	int16_t doortopheight;
	sector_t doorsector;
	sector_t* sectors;



    //	Check for locks
    player = thing->player;
		
    switch(linespecial)
    {
      case 26: // Blue Lock
      case 32:
	if ( !player )
	    return;
	
	if (!player->cards[it_bluecard] && !player->cards[it_blueskull])
	{
	    player->message = PD_BLUEK;
	    S_StartSound(NULL,sfx_oof);
	    return;
	}
	break;
	
      case 27: // Yellow Lock
      case 34:
	if ( !player )
	    return;
	
	if (!player->cards[it_yellowcard] &&
	    !player->cards[it_yellowskull])
	{
	    player->message = PD_YELLOWK;
	    S_StartSound(NULL,sfx_oof);
	    return;
	}
	break;
	
      case 28: // Red Lock
      case 33:
		  if (!player) {
			  return;
		  }

	if (!player->cards[it_redcard] && !player->cards[it_redskull])
	{
	    player->message = PD_REDK;
	    S_StartSound(NULL,sfx_oof);
	    return;
	}
	break;
    }
 

    // if the sector has an active thinker, use it
	

	lines = (line_t*)Z_LoadBytesFromEMS(linesRef);
	sidenum = lines[linenum].sidenum[side ^ 1];

	sides = (side_t*)Z_LoadBytesFromEMS(sidesRef);
	secnum = sides[sidenum].secnum;
	sectors = (sector_t*)Z_LoadBytesFromEMS(sectorsRef);
	doorsector = sectors[secnum];


    if (doorsector.specialdataRef) {
		
		doorRef = doorsector.specialdataRef;
		door = (vldoor_t*)Z_LoadBytesFromEMS(doorRef);
	 

		switch(linespecial) {
			case	1: // ONLY FOR "RAISE" DOORS, NOT "OPEN"s
			case	26:
			case	27:
			case	28:
			case	117:


				if (door->direction == -1) {
					door->direction = 1;	// go back up
				} else {
					if (!player) {

					
						return;		// JDC: bad guys never close doors
					}
					door->direction = -1;	// start going down immediately
				}

				return;
		}
    }
	 
    // for proper sound
    switch(linespecial)
    {
      case 117:	// BLAZING DOOR RAISE
      case 118:	// BLAZING DOOR OPEN
		  S_StartSoundWithParams(doorsector.soundorgX, doorsector.soundorgY, sfx_bdopn);
	break;
	
      case 1:	// NORMAL DOOR SOUND
      case 31:
		  S_StartSoundWithParams(doorsector.soundorgX, doorsector.soundorgY, sfx_doropn);
	break;
	
      default:	// LOCKED DOOR SOUND
		  S_StartSoundWithParams(doorsector.soundorgX, doorsector.soundorgY, sfx_doropn);
	break;
    }
	
    
    // new door thinker
	doorRef = Z_MallocEMSNew(sizeof(*door), PU_LEVSPEC, 0, ALLOC_TYPE_LEVSPEC);
	sectors = (sector_t*)Z_LoadBytesFromEMS(sectorsRef);
	sectors[secnum].specialdataRef = doorRef;

	
	door = (vldoor_t*)Z_LoadBytesFromEMS(doorRef);
    door->thinkerRef = P_AddThinker (doorRef, TF_VERTICALDOOR);
	door->secnum = secnum;
	door->direction = 1;
	door->speed = VDOORSPEED;
	door->topwait = VDOORWAIT;
 

    switch(linespecial) {
		case 1:
		case 26:
		case 27:
		case 28:
			door->type = normal;
			break;
	
		case 31:
		case 32:
		case 33:
		case 34:
			door->type = open;
			lines = (line_t*)Z_LoadBytesFromEMS(linesRef);
			(&lines[linenum])->special = 0;
			linespecial = lines[linenum].special;
			break;
	
		case 117:	// blazing door raise
			door->type = blazeRaise;
			door->speed = VDOORSPEED*4;
			break;
		case 118:	// blazing door open
			door->type = blazeOpen;
			door->speed = VDOORSPEED*4;
			lines = (line_t*)Z_LoadBytesFromEMS(linesRef);
			(&lines[linenum])->special = 0;
			linespecial = lines[linenum].special;
			break;
    }
    
    // find the top and bottom of the movement range
	doortopheight = P_FindLowestCeilingSurrounding(secnum);
	door = (vldoor_t*)Z_LoadBytesFromEMS(doorRef);
	door->topheight = doortopheight - (4 << SHORTFLOORBITS);
}


//
// Spawn a door that closes after 30 seconds
//
void P_SpawnDoorCloseIn30 (int16_t secnum)
{
    vldoor_t*	door;
	MEMREF doorRef;
	sector_t* sectors;

	doorRef = Z_MallocEMSNew(sizeof(*door), PU_LEVSPEC, 0, ALLOC_TYPE_LEVSPEC);

	door = (vldoor_t*)Z_LoadBytesFromEMS(doorRef);
	door->thinkerRef = P_AddThinker(doorRef, TF_VERTICALDOOR);
	door->secnum = secnum;
	door->direction = 0;
	door->type = normal;
	door->speed = VDOORSPEED;
	door->topcountdown = 30 * 35;

	sectors = (sector_t*)Z_LoadBytesFromEMS(sectorsRef);
	sectors[secnum].specialdataRef = doorRef;
	sectors[secnum].special = 0;

   
}

//
// Spawn a door that opens after 5 minutes
//
void
P_SpawnDoorRaiseIn5Mins
( int16_t secnum)
{
	vldoor_t*	door;
	MEMREF doorRef;
	int16_t doortopheight;
	sector_t* sectors;
	doorRef = Z_MallocEMSNew(sizeof(*door), PU_LEVSPEC, 0, ALLOC_TYPE_LEVSPEC);

	door = (vldoor_t*)Z_LoadBytesFromEMS(doorRef);
	door->thinkerRef = P_AddThinker(doorRef, TF_VERTICALDOOR);

	
    door->secnum = secnum;
    door->direction = 2;
    door->type = raiseIn5Mins;
    door->speed = VDOORSPEED;

	doortopheight = P_FindLowestCeilingSurrounding(secnum);
	door = (vldoor_t*)Z_LoadBytesFromEMS(doorRef);
	door->topheight = doortopheight - (4 << SHORTFLOORBITS);
	door->topwait = VDOORWAIT;
    door->topcountdown = 5 * 60 * 35;

	sectors = (sector_t*)Z_LoadBytesFromEMS(sectorsRef);
	sectors[secnum].specialdataRef = doorRef;
	sectors[secnum].special = 0;


}

