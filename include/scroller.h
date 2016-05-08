/*
 * xrick/include/scroller.h
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

#ifndef _SCROLLER_H
#define _SCROLLER_H

typedef enum {
    SCROLL_RUNNING,
    SCROLL_DONE
} scroll_t;

#define SCROLL_PERIOD 24

extern scroll_t scroll_up(void);
extern scroll_t scroll_down(void);

#endif

/* eof */


