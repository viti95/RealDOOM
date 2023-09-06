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
//	Printed strings for translation.
//	English language support (default).
//

#ifndef __D_ENGLSH__
#define __D_ENGLSH__


int8_t* getStringByIndex(int16_t stringindex, int8_t* returndata);
int16_t getStringLength(int16_t stringindex);
//#define getStringByIndex(a) getStringByIndex(a, (char*) alloca(getStringLength(a)+1))
#define getStringByIndexWithBuffer(a, b) getStringByIndex(a, (char*) alloca(b))


#define D_DEVSTR		    0  
#define D_CDROM	            1  
#define QUITMSG	            2  
#define LOADNET             3  
#define QLOADNET	        4  
#define QSAVESPOT	        5  
#define SAVEDEAD            6  
#define QSPROMPT            7  
#define QLPROMPT	        8  
#define NIGHTMARE	        9  
#define SWSTRING	        10 
#define MSGOFF	            11 
#define MSGON	            12 
#define NETEND	            13 
#define ENDGAME	            14 
#define DOSY	            15 
#define DETAILHI	        16 
#define DETAILLO	        17 
#define GAMMALVL0	        18 
#define GAMMALVL1	        19 
#define GAMMALVL2	        20 
#define GAMMALVL3	        21 
#define GAMMALVL4	        22 
#define EMPTYSTRING	        23 
#define GOTARMOR	        24 
#define GOTMEGA	            25 
#define GOTHTHBONUS	        26 
#define GOTARMBONUS	        27 
#define GOTSTIM	            28 
#define GOTMEDIKIT	        29 
#define GOTSUPER	        30 
#define GOTBLUECARD	        31 
#define GOTYELWCARD	        32 
#define GOTREDCARD	        33 
#define GOTBLUESKUL	        34 
#define GOTYELWSKUL	        35 
#define GOTREDSKULL	        36 
#define GOTINVUL	        37 
#define GOTBERSERK	        38 
#define GOTINVIS	        39 
#define GOTSUIT	            40 
#define GOTMAP	            41 
#define GOTVISOR	        42 
#define GOTMSPHERE	        43 
#define GOTCLIP	            44 
#define GOTCLIPBOX	        45 
#define GOTROCKET	        46 
#define GOTROCKBOX	        47 
#define GOTCELL	            48 
#define GOTCELLBOX	        49 
#define GOTSHELLS	        50 
#define GOTSHELLBOX	        51 
#define GOTBACKPACK	        52 
#define GOTBFG9000	        53 
#define GOTCHAINGUN	        54 
#define GOTCHAINSAW	        55 
#define GOTLAUNCHER	        56 
#define GOTPLASMA	        57 
#define GOTSHOTGUN	        58 
#define GOTSHOTGUN2	        59 
#define PD_BLUEO	        60 
#define PD_REDO	            61 
#define PD_YELLOWO	        62 
#define PD_BLUEK	        63 
#define PD_REDK	            64 
#define PD_YELLOWK	        65 
#define GGSAVED	            66 
#define HUSTR_E1M1	        67 
#define HUSTR_E1M2	        68 
#define HUSTR_E1M3	        69 
#define HUSTR_E1M4	        70 
#define HUSTR_E1M5	        71 
#define HUSTR_E1M6	        72 
#define HUSTR_E1M7	        73 
#define HUSTR_E1M8	        74 
#define HUSTR_E1M9	        75 
#define HUSTR_E2M1	        76 
#define HUSTR_E2M2	        77 
#define HUSTR_E2M3	        78 
#define HUSTR_E2M4	        79 
#define HUSTR_E2M5	        80 
#define HUSTR_E2M6	        81 
#define HUSTR_E2M7	        82 
#define HUSTR_E2M8	        83 
#define HUSTR_E2M9	        84 
#define HUSTR_E3M1	        85 
#define HUSTR_E3M2	        86 
#define HUSTR_E3M3	        87 
#define HUSTR_E3M4	        88 
#define HUSTR_E3M5	        89 
#define HUSTR_E3M6	        90 
#define HUSTR_E3M7	        91 
#define HUSTR_E3M8	        92 
#define HUSTR_E3M9	        93 
#define HUSTR_E4M1	        94 
#define HUSTR_E4M2	        95 
#define HUSTR_E4M3	        96 
#define HUSTR_E4M4	        97 
#define HUSTR_E4M5	        98 
#define HUSTR_E4M6	        99 
#define HUSTR_E4M7	        100
#define HUSTR_E4M8	        101
#define HUSTR_E4M9	        102
#define HUSTR_1	            103
#define HUSTR_2	            104
#define HUSTR_3	            105
#define HUSTR_4	            106
#define HUSTR_5	            107
#define HUSTR_6	            108
#define HUSTR_7	            109
#define HUSTR_8	            110
#define HUSTR_9	            111
#define HUSTR_10	        112
#define HUSTR_11	        113
#define HUSTR_12	        114
#define HUSTR_13	        115
#define HUSTR_14	        116
#define HUSTR_15	        117
#define HUSTR_16	        118
#define HUSTR_17	        119
#define HUSTR_18	        120
#define HUSTR_19	        121
#define HUSTR_20	        122
#define HUSTR_21	        123
#define HUSTR_22	        124
#define HUSTR_23	        125
#define HUSTR_24	        126
#define HUSTR_25	        127
#define HUSTR_26	        128
#define HUSTR_27	        129
#define HUSTR_28	        130
#define HUSTR_29	        131
#define HUSTR_30	        132
#define HUSTR_31	        133
#define HUSTR_32	        134
#define PHUSTR_1	        135
#define PHUSTR_2	        136
#define PHUSTR_3	        137
#define PHUSTR_4	        138
#define PHUSTR_5	        139
#define PHUSTR_6	        140
#define PHUSTR_7	        141
#define PHUSTR_8	        142
#define PHUSTR_9	        143
#define PHUSTR_10	        144
#define PHUSTR_11	        145
#define PHUSTR_12	        146
#define PHUSTR_13	        147
#define PHUSTR_14	        148
#define PHUSTR_15	        149
#define PHUSTR_16	        150
#define PHUSTR_17	        151
#define PHUSTR_18	        152
#define PHUSTR_19	        153
#define PHUSTR_20	        154
#define PHUSTR_21	        155
#define PHUSTR_22	        156
#define PHUSTR_23	        157
#define PHUSTR_24	        158
#define PHUSTR_25	        159
#define PHUSTR_26	        160
#define PHUSTR_27	        161
#define PHUSTR_28	        162
#define PHUSTR_29	        163
#define PHUSTR_30	        164
#define PHUSTR_31	        165
#define PHUSTR_32	        166
#define THUSTR_1	        167
#define THUSTR_2	        168
#define THUSTR_3	        169
#define THUSTR_4	        170
#define THUSTR_5	        171
#define THUSTR_6	        172
#define THUSTR_7	        173
#define THUSTR_8	        174
#define THUSTR_9	        175
#define THUSTR_10	        176
#define THUSTR_11	        177
#define THUSTR_12	        178
#define THUSTR_13	        179
#define THUSTR_14	        180
#define THUSTR_15	        181
#define THUSTR_16	        182
#define THUSTR_17	        183
#define THUSTR_18	        184
#define THUSTR_19	        185
#define THUSTR_20	        186
#define THUSTR_21	        187
#define THUSTR_22	        188
#define THUSTR_23	        189
#define THUSTR_24	        190
#define THUSTR_25	        191
#define THUSTR_26	        192
#define THUSTR_27	        193
#define THUSTR_28	        194
#define THUSTR_29	        195
#define THUSTR_30	        196
#define THUSTR_31	        197
#define THUSTR_32	        198
#define HUSTR_CHATMACRO1	199
#define HUSTR_CHATMACRO2	200
#define HUSTR_CHATMACRO3	201
#define HUSTR_CHATMACRO4	202
#define HUSTR_CHATMACRO5	203
#define HUSTR_CHATMACRO6	204
#define HUSTR_CHATMACRO7	205
#define HUSTR_CHATMACRO8	206
#define HUSTR_CHATMACRO9	207
#define HUSTR_CHATMACRO0	208
#define HUSTR_TALKTOSELF1	209
#define HUSTR_TALKTOSELF2	210
#define HUSTR_TALKTOSELF3	211
#define HUSTR_TALKTOSELF4	212
#define HUSTR_TALKTOSELF5	213
#define HUSTR_MESSAGESENT	214
#define HUSTR_PLRGREEN	    215
#define HUSTR_PLRINDIGO	    216
#define HUSTR_PLRBROWN	    217
#define HUSTR_PLRRED	    218
#define AMSTR_FOLLOWON	    219
#define AMSTR_FOLLOWOFF	    220
#define AMSTR_GRIDON	    221
#define AMSTR_GRIDOFF	    222
#define AMSTR_MARKEDSPOT	223
#define AMSTR_MARKSCLEARED	224
#define STSTR_MUS	        225
#define STSTR_NOMUS	        226
#define STSTR_DQDON	        227
#define STSTR_DQDOFF	    228
#define STSTR_KFAADDED	    229
#define STSTR_FAADDED	    230
#define STSTR_NCON	        231
#define STSTR_NCOFF	        232
#define STSTR_BEHOLD	    233
#define STSTR_BEHOLDX	    234
#define STSTR_CHOPPERS	    235
#define STSTR_CLEV	        236
#define E1TEXT              237
#define E2TEXT              238
#define E3TEXT              239
#define E4TEXT              240
#define C1TEXT              241
#define C2TEXT              242
#define C3TEXT              243
#define C4TEXT              244
#define C5TEXT              245
#define C6TEXT              246
#define P1TEXT              247
#define P2TEXT              248
#define P3TEXT              249
#define P4TEXT              250
#define P5TEXT              251
#define P6TEXT              252
#define T1TEXT              253
#define T2TEXT              254
#define T3TEXT              255
#define T4TEXT              256
#define T5TEXT              257
#define T6TEXT              258
#define CC_ZOMBIE	        259
#define CC_SHOTGUN	        260
#define CC_HEAVY	        261
#define CC_IMP	            262
#define CC_DEMON	        263
#define CC_LOST	            264
#define CC_CACO	            265
#define CC_HELL	            266
#define CC_BARON	        267
#define CC_ARACH	        268
#define CC_PAIN	            269
#define CC_REVEN	        270
#define CC_MANCU	        271
#define CC_ARCH	            272
#define CC_SPIDER	        273
#define CC_CYBER	        274
#define CC_HERO	            275
#define QUITMSGD11			276
#define QUITMSGD12			277
#define QUITMSGD13			278
#define QUITMSGD14			279
#define QUITMSGD15			280
#define QUITMSGD16			281
#define QUITMSGD17			282
#define QUITMSGD21			283
#define QUITMSGD22			284
#define QUITMSGD23			285
#define QUITMSGD24			286
#define QUITMSGD25			287
#define QUITMSGD26			288
#define QUITMSGD27			289
#define NEWLEVELMSG			290
#define MODIFIED_GAME		291
#define NOT_SHAREWARE		292
#define DO_NOT_DISTRIBUTE	293
#define VERSION_REGISTERED	294
#define VERSION_SHAREWARE	295
#define VERSION_COMMERCIAL	296
#define M_INIT_TEXT			297
#define R_INIT_TEXT			298
#define P_INIT_TEXT			299
#define I_INIT_TEXT			300
#define S_INIT_TEXT			301
#define HU_INIT_TEXT		302
#define ST_INIT_TEXT		303



#endif
