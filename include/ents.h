/*
 * xrick/include/ents.h
 *
 * Copyright (C) 1998-2002 BigOrno (bigorno@bigorno.net). All rights reserved.
 *
 * The use and distribution terms for this software are contained in the file
 * named README, which can be found in the root of this distribution. By
 * using this software in any fashion, you are agreeing to be bound by the
 * terms of this license.
 *
 * You must not remove this notice, or any other, from this software.
 */

#ifndef _ENTS_H
#define _ENTS_H

#include <stdbool.h> /* bool */

#include "system.h"
#include "rects.h"

#define ENT_XRICK ent_ents[1]

#define ENT_NBR_ENTDATA 0x4a
#define ENT_NBR_SPRSEQ 0x88
#define ENT_NBR_MVSTEP 0x310

#define ENT_ENTSNUM 0x0c

/*
 * flags for ent_ents[e].n  ("yes" when set)
 *
 * ENT_LETHAL: is entity lethal?
 */
#define ENT_LETHAL 0x80

/*
 * flags for ent_ents[e].flag  ("yes" when set)
 *
 * ENT_FLG_ONCE: should the entity run once only?
 * ENT_FLG_STOPRICK: does the entity stops rick (and goes to slot zero)?
 * ENT_FLG_LETHALR: is entity lethal when restarting?
 * ENT_FLG_LETHALI: is entity initially lethal?
 * ENT_FLG_TRIGBOMB: can entity be triggered by a bomb?
 * ENT_FLG_TRIGBULLET: can entity be triggered by a bullet?
 * ENT_FLG_TRIGSTOP: can entity be triggered by rick stop?
 * ENT_FLG_TRIGRICK: can entity be triggered by rick?
 */
#define ENT_FLG_ONCE 0x01
#define ENT_FLG_STOPRICK 0x02
#define ENT_FLG_LETHALR 0x04
#define ENT_FLG_LETHALI 0x08
#define ENT_FLG_TRIGBOMB 0x10
#define ENT_FLG_TRIGBULLET 0x20
#define ENT_FLG_TRIGSTOP 0x40
#define ENT_FLG_TRIGRICK 0x80

typedef struct {
  uint8_t n;          /* b00 */
  /*uint8_t b01;*/    /* b01 in ASM code but never used */
  int16_t x;         /* b02 - position */
  int16_t y;         /* w04 - position */
  uint8_t sprite;     /* b08 - sprite number */
  /*uint16_t w0C;*/   /* w0C in ASM code but never used */
  uint8_t w;          /* b0E - width */
  uint8_t h;          /* b10 - height */
  uint16_t mark;      /* w12 - number of the mark that created the entity */
  uint8_t flags;      /* b14 */
  int16_t trig_x;    /* b16 - position of trigger box */
  int16_t trig_y;    /* w18 - position of trigger box */
  int16_t xsave;     /* b1C */
  int16_t ysave;     /* w1E */
  uint16_t sprbase;   /* w20 */
  uint16_t step_no_i; /* w22 */
  uint16_t step_no;   /* w24 */
  int16_t c1;        /* b26 */
  int16_t c2;        /* b28 */
  uint8_t ylow;       /* b2A */
  int16_t offsy;     /* w2C */
  uint8_t latency;    /* b2E */
  uint8_t prev_n;     /* new */
  int16_t prev_x;    /* new */
  int16_t prev_y;    /* new */
  uint8_t prev_s;     /* new */
  bool front;    /* new */
  uint8_t trigsnd;    /* new */
} ent_t;

typedef struct {
  uint8_t w, h;
  uint16_t spr, sni;
  uint8_t trig_w, trig_h;
  uint8_t snd;
} entdata_t;

typedef struct {
  uint8_t count;
  int8_t dx, dy;
} mvstep_t;

extern ent_t ent_ents[ENT_ENTSNUM + 1];
extern entdata_t ent_entdata[ENT_NBR_ENTDATA];
extern rect_t *ent_rects;
extern uint8_t ent_sprseq[ENT_NBR_SPRSEQ];
extern mvstep_t ent_mvstep[ENT_NBR_MVSTEP];

extern void ent_reset(void);
extern void ent_actvis(uint8_t, uint8_t);
extern void ent_draw(void);
extern void ent_clprev(void);
extern void ent_action(void);

#endif

/* eof */
