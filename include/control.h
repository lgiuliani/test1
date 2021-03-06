/*
 * xrick/include/control.h
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

#ifndef _CONTROL_H
#define _CONTROL_H

/* Bit field structure that represent Rick's controls */
typedef struct
{
    bool right;
    bool left;
    bool down;
    bool up;
    bool fire;
    bool exit;
    bool terminate;
    bool pause;
} control_t;

extern control_t control;
extern bool control_active;

#endif

/* eof */


