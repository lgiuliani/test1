/*
 * xrick/include/util.h
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

#ifndef _UTIL_H
#define _UTIL_H

extern void u_envtest(int16_t, int16_t, uint8_t, uint8_t *, uint8_t *);
extern uint8_t u_boxtest(uint8_t, uint8_t);
extern uint8_t u_fboxtest(uint8_t, int16_t, int16_t);
extern uint8_t u_trigbox(uint8_t, int16_t, int16_t);

#endif

/* eof */
