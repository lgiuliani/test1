/*
 * xrick/src/sysevt.c
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

/*
 * 20021010 SDLK_n replaced by SDLK_Fn because some non-US keyboards
 *          requires that SHIFT be pressed to input numbers.
 */

#include <SDL.h>

#include "system.h"
#include "config.h"
#include "game.h"
#include "debug.h"

#include "control.h"
#include "draw.h"

#define SYSJOY_RANGE 3280

static SDL_Event event;

/*
 * Process an event
 */
static void
processEvent()
{
	uint16_t key;
#ifdef ENABLE_FOCUS
	SDL_ActiveEvent *aevent;
#endif

  switch (event.type) {
  case SDL_KEYDOWN:
    key = event.key.keysym.sym;
    if (key == syskbd_up || key == SDLK_UP)
        control.up = true;
    else if (key == syskbd_down || key == SDLK_DOWN)
        control.down = true;
    else if (key == syskbd_left || key == SDLK_LEFT)
        control.left = true;
    else if (key == syskbd_right || key == SDLK_RIGHT)
        control.right = true;
    else if (key == syskbd_pause)
        control.pause = true;
    else if (key == syskbd_end)
        control.terminate = true;
    else if (key == syskbd_xtra)
        control.exit = true;
    else if (key == syskbd_fire)
        control.fire = true;
    else if (key == SDLK_F1)
      sysvid_toggleFullscreen();
    else if (key == SDLK_F2)
      sysvid_zoom(-1);
    else if (key == SDLK_F3)
      sysvid_zoom(+1);
#ifdef ENABLE_SOUND
    else if (key == SDLK_F4)
      syssnd_toggleMute();
    else if (key == SDLK_F5)
      syssnd_vol(-1);
    else if (key == SDLK_F6)
      syssnd_vol(+1);
#endif
#ifdef ENABLE_CHEATS
    else if (key == SDLK_F7)
      game_toggleCheat(1);
    else if (key == SDLK_F8)
      game_toggleCheat(2);
    else if (key == SDLK_F9)
      game_toggleCheat(3);
#endif
    break;
  case SDL_KEYUP:
    key = event.key.keysym.sym;
    if (key == syskbd_up || key == SDLK_UP)
        control.up = false;
    else if (key == syskbd_down || key == SDLK_DOWN)
        control.down = false;
    else if (key == syskbd_left || key == SDLK_LEFT)
        control.left = false;
    else if (key == syskbd_right || key == SDLK_RIGHT)
        control.right = false;
    else if (key == syskbd_pause)
        control.pause = false;
    else if (key == syskbd_end)
        control.terminate = false;
    else if (key == syskbd_xtra)
        control.exit = false;
    else if (key == syskbd_fire)
        control.fire = false;
    break;
  case SDL_QUIT:
    /* player tries to close the window -- this is the same as pressing ESC */
    control.exit = true;
    break;
#ifdef ENABLE_FOCUS
  case SDL_ACTIVEEVENT: {
    aevent = (SDL_ActiveEvent *)&event;
    IFDEBUG_EVENTS(
      printf("xrick/events: active %x %x\n", aevent->gain, aevent->state);
      );
    if (aevent->gain == 1)
      control_active = true;
    else
      control_active = false;
    }
  break;
#endif
#ifdef ENABLE_JOYSTICK
  case SDL_JOYAXISMOTION:
    IFDEBUG_EVENTS(sys_printf("xrick/events: joystick\n"););
    if (event.jaxis.axis == 0) {  /* left-right */
        if (event.jaxis.value < -SYSJOY_RANGE) {  /* left */
            control.left = true;
            control.right = false;
        }
        else if (event.jaxis.value > SYSJOY_RANGE) {  /* right */
            control.right = true;
            control.left = false;
        }
        else {  /* center */
            control.left = false;
            control.right = false;
        }
    }
    if (event.jaxis.axis == 1) {  /* up-down */
        if (event.jaxis.value < -SYSJOY_RANGE) {  /* up */
            control.up = true;
            control.down = false;
        }
        else if (event.jaxis.value > SYSJOY_RANGE) {  /* down */
            control.down = true;
            control.up = false;
        }
        else {  /* center */
            control.up = false;
            control.down = false;
        }
    }
    break;
  case SDL_JOYBUTTONDOWN:
    control.fire = true;
    break;
  case SDL_JOYBUTTONUP:
    control.fire = false;
    break;
#endif
  default:
    break;
  }
}

/*
 * Process events, if any, then return
 */
void
sysevt_poll(void)
{
  while (SDL_PollEvent(&event))
    processEvent();
}

/*
 * Wait for an event, then process it and return
 */
void
sysevt_wait(void)
{
  SDL_WaitEvent(&event);
  processEvent();
}

/* eof */



