#ifndef GAMECORE_H
#define GAMECORE_H

/**********************
 ***** gamecore.c *****
 **********************/

/*****
 * Globals
 *****/

/* ficha_prox is the array of the piece which is showed to the player and
 * which will come next. We define one for every player
 */

struct {
   char now[4][4][4],next[4][4][4];
} ficha[2];

extern char arena[2][21][12], old_arena[2][21][12];

extern int TECLA_1_UP, TECLA_1_DOWN, TECLA_1_LEFT, TECLA_1_RIGHT,
   TECLA_2_UP, TECLA_2_DOWN, TECLA_2_LEFT, TECLA_2_RIGHT,
   TECLA_MUSIC, TECLA_PAUSE;

extern int count_old[2], death_match, death_lines, visible_blocks, pierde[2];

/*****
 * Prototypes
 *****/

void one_player(void);
void two_player(void);


#endif
