#ifndef MULTITET_H
#define MULTITET_H

#include <allegro.h>

/* number of ticks a second has... */
#define SECOND 30

#define GAME_NAME "Multitet 1.33"
#define GAME_VER        1
#define GAME_SUB_VER    33

#define     STR_KEYS       "KEYS"
#define     STR_SOUND      "SOUND"
#define     STR_GAME       "GAME"

#define		TSCREEN_W		640
#define		TSCREEN_H		480

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
void stretch_virtual_screen();

#endif
