/*
 * xrick/src/scr_gameover.c
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

#include "stddef.h" /* NULL */

#include "system.h"
#include "game.h"
#include "screens.h"

#include "draw.h"
#include "control.h"

/*
 * Display the game over screen
 *
 * return: SCREEN_RUNNING, SCREEN_DONE, SCREEN_EXIT
 */
screen_status_t
screen_gameover(void)
{
	static uint8_t seq = 0;
	static uint8_t period = 0;
#ifdef GFXST
	static uint32_t tm = 0;
#endif

	if (seq == 0) {
		draw_tilesBank = 0;
		seq = 1;
		period = game_period; /* save period, */
		game_period = 50;     /* and use our own */
#ifdef ENABLE_SOUND
		game_setmusic("sounds/gameover.wav", 1);
#endif
	}

	switch (seq) {
	case 1:  /* display banner */
#ifdef GFXST
		sysvid_clear();
		tm = sys_gettime();
#endif
		draw_tllst = screen_gameovertxt;
		draw_setfb(120, 80);
#ifdef GFXPC
		draw_filter = 0xAAAA;
#endif
		draw_tilesList();

		game_rects = &draw_SCREENRECT;
		seq = 2;
		break;

	case 2:  /* wait for key pressed */
		if (control.fire)
			seq = 3;
#ifdef GFXST
		else if (sys_gettime() - tm > SCREEN_TIMEOUT)
			seq = 4;
#endif
		else
			sys_sleep(50);
		break;

	case 3:  /* wait for key released */
		if (!(control.fire))
			seq = 4;
		else
			sys_sleep(50);
		break;
	}

	if (control.exit)  /* check for exit request */
		return SCREEN_EXIT;

	if (seq == 4) {  /* we're done */
		sysvid_clear();
		seq = 0;
		game_period = period;
		return SCREEN_DONE;
	}

  return SCREEN_RUNNING;
}

/* eof */

