/*
 * xrick/include/e_bomb.h
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

#ifndef _E_BOMB_H
#define _E_BOMB_H

#include "system.h"

#define E_BOMB_NO 3
#define E_BOMB_ENT ent_ents[E_BOMB_NO]
#define E_BOMB_TICKER (0x2D)

extern bool e_bomb_lethal;
extern uint8_t e_bomb_ticker;
extern uint8_t e_bomb_xc;
extern uint16_t e_bomb_yc;

extern bool e_bomb_hit(uint8_t);
extern void e_bomb_init(uint16_t, uint16_t);
extern void e_bomb_action(uint8_t);

#endif

/* eof */
