/*
 * xrick/include/e_rick.h
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

#ifndef _E_RICK_H
#define _E_RICK_H

#include "system.h"

#define E_RICK_NO 1
#define E_RICK_ENT ent_ents[E_RICK_NO]

typedef enum {  E_RICK_STAND,
                E_RICK_STOP,
                E_RICK_SHOOT,
                E_RICK_CLIMB,
                E_RICK_JUMP,
                E_RICK_ZOMBIE,
                E_RICK_DEAD,
                E_RICK_CRAWL
} e_rick_state_t;

extern e_rick_state_t e_rick_state;
extern int16_t e_rick_stop_x, e_rick_stop_y;

extern void e_rick_save(void);
extern void e_rick_restore(void);
extern void e_rick_action(uint8_t);
extern void e_rick_gozombie(void);
extern bool e_rick_boxtest(uint8_t);

#endif

/* eof */


