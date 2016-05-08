/*
 * xrick/src/e_rick.c
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

#include "system.h"
#include "config.h"
#include "game.h"
#include "ents.h"
#include "e_rick.h"

#include "e_bullet.h"
#include "e_bomb.h"
#include "control.h"
#include "maps.h"
#include "util.h"

/*
 * public vars
 */
int16_t e_rick_stop_x = 0;
int16_t e_rick_stop_y = 0;
e_rick_state_t e_rick_state = E_RICK_STAND;

/*
 * local vars
 */
#define offsx  c1
static bool save_crawl;
static uint8_t seq;
static uint16_t save_x, save_y;

/*
 * Prototypes
 */
static void e_rick_zombie(void);
static void e_rick_move_horizontaly(void);
static void e_rick_climb(void);
static void e_rick_stop(void);
static void e_rick_shoot(void);
static void e_rick_pose_bomb(void);
static void e_rick_action2(void);

/*
 * Box test
 *
 * ASM 113E (based on)
 *
 * e: entity to test against (corresponds to SI in asm code -- here DI
 *    is assumed to point to rick).
 * ret: true/intersect, false/not.
 */
bool
e_rick_boxtest(uint8_t e)
{
	/*
	 * rick: x+0x05 to x+0x11, y+[0x08 if rick's crawling] to y+0x14
	 * entity: x to x+w, y to y+h
	 */

	if (E_RICK_ENT.x + 0x11 < ent_ents[e].x ||
		E_RICK_ENT.x + 0x05 > ent_ents[e].x + ent_ents[e].w ||
		E_RICK_ENT.y + 0x14 < ent_ents[e].y ||
		E_RICK_ENT.y + ((e_rick_state == E_RICK_CRAWL) ? 0x08 : 0x00) > ent_ents[e].y + ent_ents[e].h - 1)
		return false;
	else
		return true;
}

/*
 * Go zombie
 *
 * ASM 1851
 */
void
e_rick_gozombie(void)
{
#ifdef ENABLE_CHEATS
	if (game_cheat2) return;
#endif

	/* already zombie? */
	if (e_rick_state == E_RICK_ZOMBIE)
        return;


#ifdef ENABLE_SOUND
	syssnd_play(WAV_DIE, 1);
#endif

	e_rick_state = E_RICK_ZOMBIE;
	E_RICK_ENT.offsy = -0x0400;
	E_RICK_ENT.offsx = (E_RICK_ENT.x > 0x80 ? -3 : +3);
	E_RICK_ENT.ylow = 0;
	E_RICK_ENT.front = true;
}

/*
 * Action sub-function for e_rick when zombie
 *
 * ASM 17DC
 */
void
e_rick_zombie(void)
{
	uint32_t i;

	/* sprite */
	E_RICK_ENT.sprite = (E_RICK_ENT.x & 0x04) ? 0x1A : 0x19;

	/* x */
	E_RICK_ENT.x += E_RICK_ENT.offsx;

	/* y */
	i = (E_RICK_ENT.y << 8) + E_RICK_ENT.offsy + E_RICK_ENT.ylow;
	E_RICK_ENT.y = i >> 8;
	E_RICK_ENT.offsy += 0x80;
	E_RICK_ENT.ylow = i;

	/* dead when out of screen */
	if (E_RICK_ENT.y < 0 || E_RICK_ENT.y > 0x0140)
        e_rick_state = E_RICK_DEAD;
}

void
e_rick_move_horizontaly(void)
{
    int16_t x;
    uint8_t env0, env1;

    /* should move? */
    if (control.left) {      /* move left */
        isRickOnLeft = true;
        x = E_RICK_ENT.x - 2;
    }
    else if (control.right) { /* move right */
        isRickOnLeft = false;
        x = E_RICK_ENT.x + 2;
    }
    else    /* no: reset seq and return */
    {
        seq = 0;
        return;
    }

    if (x < 0 || x >= 0xe8) {  /* prev/next submap */
        game_chsm = true;
        E_RICK_ENT.x = (isRickOnLeft == true)? 0xe2 : 0x04;
        return;
    }

    /* still within this map: test environment */
    u_envtest(x, E_RICK_ENT.y, (e_rick_state == E_RICK_CRAWL), &env0, &env1);

    /* save x-position if it is possible to move */
    if (!(env1 & (MAP_EFLG_SOLID|MAP_EFLG_SPAD|MAP_EFLG_WAYUP))) {
        E_RICK_ENT.x = x;
        if (env1 & MAP_EFLG_LETHAL)
            e_rick_gozombie();
    }
}

void
e_rick_climb(void)
{
    uint8_t env0, env1;
    int16_t x, y;

    /* should move? */
    if (!(control.up || control.down || control.left || control.right)) {
        seq = 0; /* no: reset seq and return */
        return;
    }

    x = E_RICK_ENT.x;
    y = E_RICK_ENT.y;

    if (control.up)  /* up-down: calc new y and test environment */
        y = y - 0x02;
    else if (control.down)
        y = y + 0x02;

    if (control.left)
        x = x - 0x02;
    else if (control.right)
        x = x + 0x02;

    u_envtest(x, y, false, &env0, &env1);

    if (env1 & (MAP_EFLG_SOLID|MAP_EFLG_SPAD|MAP_EFLG_WAYUP) && (!control.up)) {
            E_RICK_ENT.offsy = 0x0100;
            e_rick_state = E_RICK_STAND;
            return;
    }

    if (!(env1 & (MAP_EFLG_SOLID|MAP_EFLG_SPAD))) {
        E_RICK_ENT.x = x;
        E_RICK_ENT.y = y;

        if (env1 & MAP_EFLG_LETHAL) {
            e_rick_gozombie();
            return;
        }

        if (!(env1 & (MAP_EFLG_VERT|MAP_EFLG_CLIMB))) {
            /* reached end of climb zone */
            if (control.up)
                E_RICK_ENT.offsy = -0x0300;
            else if (control.down)
                E_RICK_ENT.offsy = 0x0100;
#ifdef ENABLE_SOUND
            if (control.up)
                syssnd_play(WAV_JUMP, 1);
#endif
            e_rick_state = E_RICK_STAND;
        }
    }

#ifdef ENABLE_SOUND
            seq = (seq + 1) & 0x03;
            if (seq == 0)
                syssnd_play(WAV_WALK, 1);
#endif
}

void
e_rick_stop(void)
{
    /* Play Sound once */
    if (e_rick_state != E_RICK_STOP)
    {
#ifdef ENABLE_SOUND
        syssnd_play(WAV_STICK, 1);
        e_rick_state = E_RICK_STOP;
#endif
    }

    if (control.right)
    {
        isRickOnLeft = false;
        e_rick_stop_x = E_RICK_ENT.x + 0x17;
    }
    else
    {
        isRickOnLeft = true;
        e_rick_stop_x = E_RICK_ENT.x;
    }
    e_rick_stop_y = E_RICK_ENT.y + 0x000E;
}

void
e_rick_shoot(void)
{
    /* not an automatic gun: shoot once only */
    if (e_rick_state == E_RICK_SHOOT)
        return;
    e_rick_state = E_RICK_SHOOT;

    /* already a bullet in the air ... that's enough */
    /* else use a bullet, if any available */
    if (E_BULLET_ENT.n != 0 || game_bullets == 0)
        return;

#ifdef ENABLE_CHEATS
    if (!game_cheat1) game_bullets--;
#else
    game_bullets--;
#endif

    e_bullet_init(E_RICK_ENT.x, E_RICK_ENT.y);
#ifdef ENABLE_SOUND
    syssnd_play(WAV_BULLET, 1);
#endif
}

void
e_rick_pose_bomb(void)
{
    /* already a bomb ticking ... that's enough */
    /* else use a bomb, if any available */
    if (E_BOMB_ENT.n || !game_bombs)
        return;

#ifdef ENABLE_CHEATS
    if (!game_cheat1)
        game_bombs--;
#else
    game_bombs--;
#endif
    /* initialize bomb */
    e_bomb_init(E_RICK_ENT.x, E_RICK_ENT.y);
}

/*
 * Action sub-function for e_rick.
 *
 * ASM 13BE
 */
void
e_rick_action2(void)
{
	uint8_t env0, env1;
	int16_t y;
	uint32_t i;

	/* climbing? */
	if (e_rick_state == E_RICK_CLIMB){
        e_rick_climb();
        return;
	}

	/*
	* NOT CLIMBING
	*/
	/* calc y */
	i = (E_RICK_ENT.y << 8) + E_RICK_ENT.offsy + E_RICK_ENT.ylow;
	y = i >> 8;
	/* test environment */
	u_envtest(E_RICK_ENT.x, y, (e_rick_state == E_RICK_CRAWL), &env0, &env1);

	/* can move vertically? */
	if (env1 & (E_RICK_ENT.offsy < 0 ?
					MAP_EFLG_VERT|MAP_EFLG_SOLID|MAP_EFLG_SPAD :
					MAP_EFLG_VERT|MAP_EFLG_SOLID|MAP_EFLG_SPAD|MAP_EFLG_WAYUP))
    {
        /*
        * NO VERTICAL MOVE
        */
        E_RICK_ENT.ylow = 0;
        E_RICK_ENT.y &= 0xF8;

        if (E_RICK_ENT.offsy < 0) {
            /* not climbing + trying to go _up_ not possible -> hit the roof */
            /* fall back to the ground */
            E_RICK_ENT.offsy = 0;
            e_rick_state = E_RICK_JUMP;
            e_rick_move_horizontaly();
            return;
        }
        /* else: not climbing + trying to go _down_ not possible -> standing */
        /* align to ground */
        E_RICK_ENT.y |= 0x03;

        /* standing on a super pad? */
        if ((env1 & MAP_EFLG_SPAD) && E_RICK_ENT.offsy >= 0X0200) {
            E_RICK_ENT.offsy = (control.up) ? 0xf800 : 0x00fe - E_RICK_ENT.offsy;
#ifdef ENABLE_SOUND
            syssnd_play(WAV_PAD, 1);
#endif
            e_rick_move_horizontaly();
            return;
        }
        E_RICK_ENT.offsy = 0x0100;  /* reset*/

        if (env0) {
            e_rick_state = E_RICK_CRAWL;
            e_rick_move_horizontaly();
            return;
        }

        /* standing. firing ? */
        if (control.fire) {
            if (control.left || control.right)  /* stop */
                e_rick_stop();
            else if (control.down)              /* bomb */
                e_rick_pose_bomb();
            else if (control.up)                /* bullet */
                e_rick_shoot();
            else {
                e_rick_state = E_RICK_STAND; /* not shooting means trigger is released */
                seq = 0; /* Avoid Rick to walk */
            }
           return;
        }

        if (control.up) {
            if (env1 & MAP_EFLG_CLIMB)    /* climb */
            {
                e_rick_state = E_RICK_CLIMB;
                return;
            }
            E_RICK_ENT.offsy = -0x0580;  /* jump */
#ifdef ENABLE_SOUND
            syssnd_play(WAV_JUMP, 1);
#endif
        }
        else if (control.down) {
            if ((env1 & MAP_EFLG_VERT) &&                /* can go down */
                    !(control.left || control.right) &&  /* + not moving horizontaly */
                    (E_RICK_ENT.x & 0x1f) < 0x0a)        /* + aligned -> climb */
            {
                E_RICK_ENT.x &= 0xf0;
                E_RICK_ENT.x |= 0x04;
                e_rick_state = E_RICK_CLIMB;
                return;
            }
            else    /* crawl */
            {
                e_rick_state = E_RICK_CRAWL;
                e_rick_move_horizontaly();
                return;
            }
        }

        e_rick_state = E_RICK_STAND;
    }
    else
    {
        /* killed? */
        if (env1 & MAP_EFLG_LETHAL) {
            e_rick_gozombie();
            return;
        }
        /* save */
        E_RICK_ENT.y = y;
        E_RICK_ENT.ylow = i;
        /* climb? */
        if ((env1 & MAP_EFLG_CLIMB) && (control.up || control.down)) {
            e_rick_state = E_RICK_CLIMB;
            return;
        }
        /* fall */
        E_RICK_ENT.offsy += 0x0080;
        if (E_RICK_ENT.offsy > 0x0800) {
            E_RICK_ENT.offsy = 0x0800;
            E_RICK_ENT.ylow = 0;
        }

        /* If not crawling, is there room before jump ? */
        if (e_rick_state != E_RICK_CRAWL || !env0)
            e_rick_state = E_RICK_JUMP;
    }

	/*
	* HORIZONTAL MOVE
	*/
    e_rick_move_horizontaly();
}


/*
 * Action function for e_rick
 *
 * ASM 12CA
 */
void e_rick_action(UNUSED(uint8_t e))
{
    if (e_rick_state != E_RICK_ZOMBIE && e_rick_state != E_RICK_DEAD)
        e_rick_action2();

    switch(e_rick_state) {
        case E_RICK_DEAD:
            break;
        case E_RICK_ZOMBIE:
            e_rick_zombie();
            break;
        case E_RICK_STOP:
            E_RICK_ENT.sprite = (isRickOnLeft == true ? 0x17 : 0x0B);
            break;
        case E_RICK_SHOOT:
            E_RICK_ENT.sprite = (isRickOnLeft ? 0x16 : 0x0A);
            break;
        case E_RICK_CLIMB:
            E_RICK_ENT.sprite = (((E_RICK_ENT.x ^ E_RICK_ENT.y) & 0x04) ? 0x18 : 0x0c);
            break;
        case E_RICK_CRAWL:
            E_RICK_ENT.sprite = (isRickOnLeft ? 0x13 : 0x07);
            if (E_RICK_ENT.x & 0x04) E_RICK_ENT.sprite++;
#ifdef ENABLE_SOUND
            seq = (seq + 1) & 0x03;
            if (seq == 0) syssnd_play(WAV_CRAWL, 1);
#endif
            break;
        case E_RICK_JUMP:
            E_RICK_ENT.sprite = (isRickOnLeft ? 0x15 : 0x06);
            break;
        case E_RICK_STAND:
            seq++;

            if (seq >= 0x14)
            {
#ifdef ENABLE_SOUND
                syssnd_play(WAV_WALK, 1);
#endif
                seq = 0x04;
            }
#ifdef ENABLE_SOUND
            else if (seq == 0x0C)
                syssnd_play(WAV_WALK, 1);
#endif
            E_RICK_ENT.sprite = (seq >> 2) + 1 + (isRickOnLeft ? 0x0c : 0x00);
            break;
        default:
            break;
    }
}


/*
 * Save status
 *
 * ASM part of 0x0BBB
 */
void e_rick_save(void)
{
	save_x = E_RICK_ENT.x;
	save_y = E_RICK_ENT.y;
	save_crawl = (e_rick_state == E_RICK_CRAWL);
	/* FIXME
	 * save_C0 = E_RICK_ENT.b0C;
	 * plus some 6DBC stuff?
	 */
}

/*
 * Restore status
 *
 * ASM part of 0x0BDC
 */
void e_rick_restore(void)
{
	E_RICK_ENT.x = save_x;
	E_RICK_ENT.y = save_y;
	E_RICK_ENT.front = false;
	if (save_crawl)
		e_rick_state = E_RICK_CRAWL;
	else
		e_rick_state = E_RICK_STAND;
	/* FIXME
	 * E_RICK_ENT.b0C = save_C0;
	 * plus some 6DBC stuff?
	 */
}
#undef offsx
/* eof */
