/*
 * xrick/include/system.h
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

#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "config.h"
#include <stdbool.h> /* bool */
#include <stdint.h>

/*
 * If compiling w/gcc, then we can use attributes. UNUSED(x) flags a
 * parameter or a variable as potentially being unused, so that gcc doesn't
 * complain about it.
 *
 * Note: from OpenAL code: Darwin OS cc is based on gcc and has __GNUC__
 * defined, yet does not support attributes. So in theory we should exclude
 * Darwin target here.
 */
#ifdef __GNUC__
#define UNUSED(x) x __attribute((unused))
#else
#define UNUSED(x) x
#endif

/*
 * Detect Microsoft Visual C
 */
#ifdef _MSC_VER
#define __MSVC__
/*
 * FIXME disable "integral size mismatch in argument; conversion supplied" warning
 * as long as the code has not been cleared -- there are so many of them...
 */

#pragma warning( disable : 4761 )
#endif

/*
 * Detect Microsoft Windows
 */
#ifdef WIN32
#define __WIN32__
#endif

/* this must be after typedefs because it relies on types defined above */
#include "rects.h"
#include "img.h"

/*
 * main section
 */
extern void sys_init(int, char **);
extern void sys_shutdown(void);
extern void sys_panic(char *, ...);
extern void sys_printf(char *, ...);
extern uint32_t sys_gettime(void);
extern void sys_sleep(int);

/*
 * video section
 */
#define SYSVID_ZOOM 2
#define SYSVID_MAXZOOM 4
#define SYSVID_WIDTH 320
#define SYSVID_HEIGHT 200

extern void sysvid_init(void);
extern void sysvid_shutdown(void);
extern void sysvid_update(rect_t *);
extern void sysvid_clear(void);
extern void sysvid_zoom(int8_t);
extern void sysvid_toggleFullscreen(void);
extern void sysvid_setGamePalette(void);
extern void sysvid_setPalette(img_color_t *, uint16_t);

/*
 * events section
 */
extern void sysevt_poll(void);
extern void sysevt_wait(void);

/*
 * keyboard section
 */
extern uint8_t syskbd_up;
extern uint8_t syskbd_down;
extern uint8_t syskbd_left;
extern uint8_t syskbd_right;
extern uint8_t syskbd_pause;
extern uint8_t syskbd_end;
extern uint8_t syskbd_xtra;
extern uint8_t syskbd_fire;

/*
 * sound section
 */
#ifdef ENABLE_SOUND
typedef struct {
#ifdef DEBUG
  char *name;
#endif
  uint8_t *buf;
  uint32_t len;
  bool dispose;
} sound_t;

extern void syssnd_init(void);
extern void syssnd_shutdown(void);
extern void syssnd_vol(int8_t);
extern void syssnd_toggleMute(void);
extern void syssnd_play(sound_t *, int8_t);
extern void syssnd_pause(bool, bool);
extern void syssnd_stopchan(int8_t);
extern void syssnd_stopsound(sound_t *);
extern void syssnd_stopall();
extern bool syssnd_isplaying(sound_t *);
extern sound_t *syssnd_load(char *name);
extern void syssnd_free(sound_t *);
#endif

/*
 * args section
 */
extern int sysarg_args_period;
extern int sysarg_args_map;
extern int sysarg_args_submap;
extern bool sysarg_args_fullscreen;
extern int sysarg_args_zoom;
#ifdef ENABLE_SOUND
extern int sysarg_args_nosound;
extern int sysarg_args_vol;
#endif
extern char *sysarg_args_data;

extern void sysarg_init(int, char **);

/*
 * joystick section
 */
#ifdef ENABLE_JOYSTICK
extern void sysjoy_init(void);
extern void sysjoy_shutdown(void);
#endif


#endif

/* eof */


