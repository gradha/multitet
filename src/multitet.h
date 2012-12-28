#ifndef MULTITET_H
#define MULTITET_H

#include <allegro/palette.h>
#include <allegro/gfx.h>
#include <allegro/datafile.h>
#include <allegro/color.h>

/* number of ticks a second has... */
#define SECOND 30

#define GAME_NAME "Multitet 1.32"
#define GAME_VER        1
#define GAME_SUB_VER    32

#define     STR_KEYS       "KEYS"
#define     STR_SOUND      "SOUND"
#define     STR_GAME       "GAME"

/**********************
 ***** multitet.h *****
 **********************/

/*****
 * Globals
 *****/

extern volatile int tick_counter;

extern DATAFILE *datafile;
extern BITMAP *virtual_screen, *pause_screen, *background, *blocks;
extern PALETTE pal_title, pal_pause;


/*****
 * Prototypes
 *****/

void dibuja_tablero(int wp);
void draw_menu_back_in_buffer(void);
void redefine_keys(void);

#endif
