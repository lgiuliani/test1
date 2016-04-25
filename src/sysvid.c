/*
 * xrick/src/sysvid.c
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

#include <stdlib.h> /* malloc */

#include <SDL.h>

#include "system.h"
#include "game.h"
#include "img.h"
#include "debug.h"

#ifdef __MSVC__
#include <memory.h> /* memset */
#endif

uint8_t *sysvid_fb; /* frame buffer */
rect_t SCREENRECT = {0, 0, SYSVID_WIDTH, SYSVID_HEIGHT, NULL}; /* whole fb */

static SDL_Color palette[256];
static SDL_Surface *screen;
static uint32_t videoFlags;

static uint8_t zoom = SYSVID_ZOOM; /* actual zoom level */
static uint8_t szoom = 0;  /* saved zoom level */
static uint8_t fszoom = 0;  /* fullscreen zoom level */

#include "img_icon.e"

/*
 * color tables
 */

#ifdef GFXPC
static uint8_t RED[] = { 0x00, 0x50, 0xf0, 0xf0, 0x00, 0x50, 0xf0, 0xf0 };
static uint8_t GREEN[] = { 0x00, 0xf8, 0x50, 0xf8, 0x00, 0xf8, 0x50, 0xf8 };
static uint8_t BLUE[] = { 0x00, 0x50, 0x50, 0x50, 0x00, 0xf8, 0xf8, 0xf8 };
#endif
#ifdef GFXST
static uint8_t RED[] = { 0x00, 0xd8, 0xb0, 0xf8,
                    0x20, 0x00, 0x00, 0x20,
                    0x48, 0x48, 0x90, 0xd8,
                    0x48, 0x68, 0x90, 0xb0,
                    /* cheat colors */
                    0x50, 0xe0, 0xc8, 0xf8,
                    0x68, 0x50, 0x50, 0x68,
                    0x80, 0x80, 0xb0, 0xe0,
                    0x80, 0x98, 0xb0, 0xc8
};
static uint8_t GREEN[] = { 0x00, 0x00, 0x6c, 0x90,
                      0x24, 0x48, 0x6c, 0x48,
                      0x6c, 0x24, 0x48, 0x6c,
                      0x48, 0x6c, 0x90, 0xb4,
		      /* cheat colors */
                      0x54, 0x54, 0x9c, 0xb4,
                      0x6c, 0x84, 0x9c, 0x84,
                      0x9c, 0x6c, 0x84, 0x9c,
                      0x84, 0x9c, 0xb4, 0xcc
};
static uint8_t BLUE[] = { 0x00, 0x00, 0x68, 0x68,
                     0x20, 0xb0, 0xd8, 0x00,
                     0x20, 0x00, 0x00, 0x00,
                     0x48, 0x68, 0x90, 0xb0,
		     /* cheat colors */
                     0x50, 0x50, 0x98, 0x98,
                     0x68, 0xc8, 0xe0, 0x50,
                     0x68, 0x50, 0x50, 0x50,
                     0x80, 0x98, 0xb0, 0xc8};
#endif

/*
 * Initialize screen
 */
static
SDL_Surface *initScreen(uint16_t w, uint16_t h, uint8_t bpp, uint32_t flags)
{
  return SDL_SetVideoMode(w, h, bpp, flags);
}

void
sysvid_setPalette(img_color_t *pal, uint16_t n)
{
  uint16_t i;

  for (i = 0; i < n; i++) {
    palette[i].r = pal[i].r;
    palette[i].g = pal[i].g;
    palette[i].b = pal[i].b;
  }
  SDL_SetColors(screen, (SDL_Color *)&palette, 0, n);
}

void
sysvid_restorePalette()
{
  SDL_SetColors(screen, (SDL_Color *)&palette, 0, 256);
}

void
sysvid_setGamePalette()
{
  uint8_t i;
  img_color_t pal[256];

  for (i = 0; i < 32; ++i) {
    pal[i].r = RED[i];
    pal[i].g = GREEN[i];
    pal[i].b = BLUE[i];
  }
  sysvid_setPalette(pal, 32);
}

/*
 * Initialize video modes
 */
void
sysvid_chkvm(void)
{
  SDL_Rect **modes;
  uint8_t i, mode = 0;

  IFDEBUG_VIDEO(sys_printf("xrick/video: checking video modes\n"););

  modes = SDL_ListModes(NULL, videoFlags|SDL_FULLSCREEN);

  if (modes == (SDL_Rect **)0)
    sys_panic("xrick/video: SDL can not find an appropriate video mode\n");

  if (modes == (SDL_Rect **)-1) {
    /* can do what you want, everything is possible */
    IFDEBUG_VIDEO(sys_printf("xrick/video: SDL says any video mode is OK\n"););
    fszoom = 1;
  }
  else {
    IFDEBUG_VIDEO(sys_printf("xrick/video: SDL says, use these modes:\n"););
    for (i = 0; modes[i]; i++) {
      IFDEBUG_VIDEO(sys_printf("  %dx%d\n", modes[i]->w, modes[i]->h););
      if (modes[i]->w <= modes[mode]->w && modes[i]->w >= SYSVID_WIDTH &&
	  modes[i]->h * SYSVID_WIDTH >= modes[i]->w * SYSVID_HEIGHT) {
	mode = i;
	fszoom = modes[mode]->w / SYSVID_WIDTH;
      }
    }
    if (fszoom != 0) {
      IFDEBUG_VIDEO(
        sys_printf("xrick/video: fullscreen at %dx%d w/zoom=%d\n",
		   modes[mode]->w, modes[mode]->h, fszoom);
	);
    }
    else {
      IFDEBUG_VIDEO(
        sys_printf("xrick/video: can not compute fullscreen zoom, use 1\n");
	);
      fszoom = 1;
    }
  }
}

/*
 * Initialise video
 */
void
sysvid_init(void)
{
  SDL_Surface *s;
  uint8_t tpix;

  IFDEBUG_VIDEO(printf("xrick/video: start\n"););

  /* SDL */
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0)
    sys_panic("xrick/video: could not init SDL\n");

  /* various WM stuff */
  SDL_WM_SetCaption("xrick", "xrick");
  SDL_ShowCursor(SDL_DISABLE);
  s = SDL_CreateRGBSurfaceFrom(IMG_ICON->pixels, IMG_ICON->w, IMG_ICON->h, 8, IMG_ICON->w, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
  SDL_SetColors(s, (SDL_Color *)IMG_ICON->colors, 0, IMG_ICON->ncolors);

  tpix = *(IMG_ICON->pixels);
  IFDEBUG_VIDEO(
    sys_printf("xrick/video: icon is %dx%d\n",
	       IMG_ICON->w, IMG_ICON->h);
    sys_printf("xrick/video: icon transp. color is #%d (%d,%d,%d)\n", tpix,
	       IMG_ICON->colors[tpix].r,
	       IMG_ICON->colors[tpix].g,
	       IMG_ICON->colors[tpix].b);
    );
  SDL_SetColorKey(s, SDL_SRCCOLORKEY,
                    SDL_MapRGB(s->format,IMG_ICON->colors[tpix].r,IMG_ICON->colors[tpix].g,IMG_ICON->colors[tpix].b));

  SDL_WM_SetIcon(s, NULL);
  SDL_FreeSurface(s);

  /* video modes and screen */
  videoFlags = SDL_HWSURFACE|SDL_HWPALETTE;
  sysvid_chkvm();  /* check video modes */
  if (sysarg_args_zoom)
    zoom = sysarg_args_zoom;
  if (sysarg_args_fullscreen) {
    videoFlags |= SDL_FULLSCREEN;
    szoom = zoom;
    zoom = fszoom;
  }
  screen = initScreen(SYSVID_WIDTH * zoom,
		      SYSVID_HEIGHT * zoom,
		      8, videoFlags);

  /*
   * create v_ frame buffer
   */
  sysvid_fb = malloc(SYSVID_WIDTH * SYSVID_HEIGHT);
  if (!sysvid_fb)
    sys_panic("xrick/video: sysvid_fb malloc failed\n");

  IFDEBUG_VIDEO(printf("xrick/video: ready\n"););
}

/*
 * Shutdown video
 */
void
sysvid_shutdown(void)
{
  free(sysvid_fb);
  sysvid_fb = NULL;

  SDL_Quit();
}

/*
 * Update screen
 * NOTE errors processing ?
 */
void
sysvid_update(rect_t *rects)
{
  static SDL_Rect area;
  uint16_t x, y, xz, yz;
  uint8_t *p, *q, *p0, *q0;

  if (rects == NULL)
    return;

  if (SDL_LockSurface(screen) == -1)
    sys_panic("xrick/panic: SDL_LockSurface failed\n");

  while (rects) {
    p0 = sysvid_fb;
    p0 += rects->x + rects->y * SYSVID_WIDTH;
    q0 = (uint8_t *)screen->pixels;
    q0 += (rects->x + rects->y * SYSVID_WIDTH * zoom) * zoom;

    for (y = rects->y; y < rects->y + rects->height; y++) {
      for (yz = 0; yz < zoom; yz++) {
	p = p0;
	q = q0;
	for (x = rects->x; x < rects->x + rects->width; x++) {
	  for (xz = 0; xz < zoom; xz++) {
	    *q = *p;
	    q++;
	  }
	  p++;
	}
	q0 += SYSVID_WIDTH * zoom;
      }
      p0 += SYSVID_WIDTH;
    }

    IFDEBUG_VIDEO2(
    for (y = rects->y; y < rects->y + rects->height; y++)
      for (yz = 0; yz < zoom; yz++) {
	p = (uint8_t *)screen->pixels + rects->x * zoom + (y * zoom + yz) * SYSVID_WIDTH * zoom;
	*p = 0x01;
	*(p + rects->width * zoom - 1) = 0x01;
      }

    for (x = rects->x; x < rects->x + rects->width; x++)
      for (xz = 0; xz < zoom; xz++) {
	p = (uint8_t *)screen->pixels + x * zoom + xz + rects->y * zoom * SYSVID_WIDTH * zoom;
	*p = 0x01;
	*(p + ((rects->height * zoom - 1) * zoom) * SYSVID_WIDTH) = 0x01;
      }
    );

    area.x = rects->x * zoom;
    area.y = rects->y * zoom;
    area.h = rects->height * zoom;
    area.w = rects->width * zoom;
    SDL_UpdateRects(screen, 1, &area);

    rects = rects->next;
  }

  SDL_UnlockSurface(screen);
}


/*
 * Clear screen
 * (077C)
 */
void
sysvid_clear(void)
{
  memset(sysvid_fb, 0, SYSVID_WIDTH * SYSVID_HEIGHT);
}


/*
 * Zoom
 */
void
sysvid_zoom(int8_t z)
{
  if (!(videoFlags & SDL_FULLSCREEN) &&
      ((z < 0 && zoom > 1) ||
       (z > 0 && zoom < SYSVID_MAXZOOM))) {
    zoom += z;
    screen = initScreen(SYSVID_WIDTH * zoom,
			SYSVID_HEIGHT * zoom,
			screen->format->BitsPerPixel, videoFlags);
    sysvid_restorePalette();
    sysvid_update(&SCREENRECT);
  }
}

/*
 * Toggle fullscreen
 */
void
sysvid_toggleFullscreen(void)
{
  videoFlags ^= SDL_FULLSCREEN;

  if (videoFlags & SDL_FULLSCREEN) {  /* go fullscreen */
    szoom = zoom;
    zoom = fszoom;
  }
  else {  /* go window */
    zoom = szoom;
  }
  screen = initScreen(SYSVID_WIDTH * zoom,
		      SYSVID_HEIGHT * zoom,
		      screen->format->BitsPerPixel, videoFlags);
  sysvid_restorePalette();
  sysvid_update(&SCREENRECT);
}

/* eof */



