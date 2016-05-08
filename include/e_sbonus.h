/*
 * xrick/include/e_sbonus.h
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

#ifndef _E_SBONUS_H
#define _E_SBONUS_H

#include "system.h"

extern bool e_sbonus_counting;
extern uint8_t e_sbonus_counter;
extern uint16_t e_sbonus_bonus;

extern void e_sbonus_start(uint8_t);
extern void e_sbonus_stop(uint8_t);

#endif

/* eof */
