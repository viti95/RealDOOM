//
// Copyright (C) 1993-1996 Id Software, Inc.
// Copyright (C) 1993-2008 Raven Software
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
//      System specific interface stuff.
//


#ifndef __I_SYSTEM__
#define __I_SYSTEM__

#include "d_ticcmd.h"
#include "d_event.h"
#include "sounds.h"


// Called by DoomMain.
void I_Init (void);

// Called by startup code
// to get the ammount of memory to malloc
// for the zone management.
byte*   I_ZoneBase (int32_t *size);


// Called by startup code
// to prep EMS memory system used
// in the zone management.
byte* I_InitEMS(int32_t *size);
// int32_t I_InitEMS(void);

// Called by startup code
// to get the ammount of memory to malloc
// for the zone management.

#ifdef _M_I86
byte *I_ZoneBaseEMS(int32_t *size, int16_t *emshandle);
#else
byte *I_ZoneBaseEMS(int32_t *size);
#endif

extern uint32_t ticcount;


 
//
// Called by D_DoomLoop,
// called before processing each tic in a frame.
// Quick syncronous operations are performed here.
// Can call D_PostEvent.
void I_StartTic (void);

// Asynchronous interrupt functions should maintain private queues
// that are read by the synchronous functions
// to be converted into events.



// Called by M_Responder when quit is selected.
// Clean exit, displays sell blurb.
void I_Quit (void);


// Allocates from low memory under dos,
// just mallocs under unix
byte* I_AllocLow (filelength_t length);

//void I_Tactile (int32_t on, int32_t off, int32_t total);


void I_Error (int8_t *error, ...);

void I_BeginRead(void);
void I_EndRead(void);

//
//  MUSIC I/O
//

int16_t I_RegisterSong(void *data);
// called by anything that wants to register a song lump with the sound lib
// calls Paul's function of the similar name to register music only.
// note that the song data is the same for any sound card and is paul's
// MUS format.  Returns a handle which will be passed to all other music
// functions.

void I_UnRegisterSong(int16_t handle);
// called by anything which is finished with a song and no longer needs
// the sound library to be aware of it.  All songs should be stopped
// before calling this, but it will double check and stop it if necessary.

void I_LoopSong(int16_t handle);
// called by anything that wishes to start music.
// plays a song, and when the song is done, starts playing it again in
// an endless loop.  the start is faded in over three seconds.

void I_StopSong(int16_t handle);
// called by anything that wishes to stop music.
// stops a song abruptly.
void I_SetMusicVolume(uint8_t volume);
void I_ResumeSong(int16_t handle);
void I_PlaySong(int16_t handle, boolean looping);
void I_PauseSong(int16_t handle);
void I_ResumeSong(int16_t handle);

//  SFX I/O
//

int16_t I_GetSfxLumpNum(sfxinfo_t* sfx);
// called by routines which wish to play a sound effect at some later
// time.  Pass it the lump name of a sound effect WITHOUT the sfx
// prefix.  This means the maximum name length is 7 letters/digits.
// The prefixes for different sound cards are 'S','M','A', and 'P'.
// They refer to the card type.  The routine will cache in the
// appropriate sound effect when it is played.

int16_t I_StartSound (int16_t id, void *data, uint8_t vol, uint8_t sep, uint8_t pitch, uint8_t priority);
// Starts a sound in a particular sound channel

void I_UpdateSoundParams(int16_t handle, uint8_t vol, uint8_t sep, uint8_t pitch);
// Updates the volume, separation, and pitch of a sound channel

void I_StopSound(int16_t handle);
// Stops a sound channel

boolean I_SoundIsPlaying(int16_t handle);
// called by S_*()'s to see if a channel is still playing.  Returns 0
// if no longer playing, 1 if playing.
void I_SetChannels(int8_t channels);

// Called by D_DoomMain,
// determines the hardware configuration
// and sets up the video mode
void I_InitGraphics(void);


void I_ShutdownGraphics(void);

// Takes full 8 bit values.
void I_SetPalette(byte* palette);

void I_UpdateNoBlit(void);
void I_FinishUpdate(void);

// Wait for vertical retrace or pause a bit.
void I_WaitVBL(int16_t count);

void I_BeginRead(void);
void I_EndRead(void);

#ifndef SKIPWIPE
void I_ReadScreen(byte* scr);
#endif


#endif
