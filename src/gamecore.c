#ifdef MSS
#include <mss.h>
#endif
#include "gamecore.h"
#include "data.h"
#include "multitet.h"
#include "score.h"
#include "misc.h"
#include <allegro.h>

#define RECOVER_BACKGROUND() stretch_blit(datafile[BMP_MENU].dat, background, 0, 0, \
   ((BITMAP*)datafile[BMP_MENU].dat)->w, \
   ((BITMAP*)datafile[BMP_MENU].dat)->h, \
   0, 0, background->w, background->h);

/* ficha_prox is the array of the piece which is showed to the player and
 * which will come next. We define one for every player

struct {
   char now[4][4][4],next[4][4][4];
   } ficha[2];

 */

static char _frames_prox[2];
static int _frame[2], _frames_ficha[2];

/* arena[]is the arenas of each player. old_arena[] is a buffers where we
 * will store the actual screen appearence. Thanks to this, the engine
 * knows which part of the screen has to updated, and we will avoid
 * drawing a lot. Good for old/slow computers.
 */
char arena[2][21][12], old_arena[2][21][12];

/* We will create the buffers for the redefined keys, which already will be
 * 'filled' by the default keys. Remember that the key's values are stored as
 * scancodes, and readkey() doesn't return scancodes directly, but you will
 * see this later when we explain redefine_keys().
 */

int TECLA_1_UP, TECLA_1_DOWN, TECLA_1_LEFT, TECLA_1_RIGHT,
   TECLA_2_UP, TECLA_2_DOWN, TECLA_2_LEFT, TECLA_2_RIGHT,
   TECLA_MUSIC, TECLA_PAUSE;

int count_old[2], death_match, death_lines, visible_blocks, pierde[2];

/* These point to the random table. */
static long _random_counter[2];

/* And this is the random table. */
static char _order_of_blocks[250000];

static int _ficha_x[2], _ficha_y[2], _desp_x[2], _speed[2];
static int _last[2],_last_move[2], _last_rotate[2];
static int _pierde_ambos, _starting_game_up, _number_of_players;

static void _borra_ficha(int wp);
static void _choque_abajo(int wp);
static void _choque_frame(int wp);
static void _choque_h(int wp);
static void _dibuja_ficha(int wp);
static void _ficha_nueva(int wp);
static void _lineas(int wp);
static void _sube(int wp, int lin);
static void _read_player_input(int which_player);
static void _init_game_variables(void);


/* Here is the important thing. This is the loop we use when only one
 * player is playing.
 */

void one_player(void)
{
   int g;
   static const char *given_points[] = { "10", "50", "125", "500" };

   _number_of_players = 1;
   show_mouse(NULL);
   blit(virtual_screen, background, 0, 0, 0, 0,
      virtual_screen->w, virtual_screen->h);

   rectfill(background, 251, 300, 381, 400, black_color);
   rectfill(virtual_screen, 251, 300, 381, 400, black_color);
   rectfill(virtual_screen, 251, 20, 381, 130, black_color);
   rectfill(background, 251, 20, 381, 130, black_color);

   rectfill(background, 19, 19, 19+220, 19+440, black_color);
   rectfill(virtual_screen, 19, 19, 19+220, 19+440, black_color);

   _init_game_variables();

   pierde[1]++; /* Only one player is acting */

   clear_keybuf();
   count_old[0] = _last_move[0] = _last_rotate[0] = tick_counter;
   
   /* This prints the help box in the place where we would print
    * the scores of the second player
    */
   outline_textout_centre(virtual_screen, datafile[SMALL_FONT].dat,
      get_config_text("Number"), 22*11+39, 305, white_color, black_color);
   outline_textout_centre(virtual_screen, datafile[SMALL_FONT].dat,
      get_config_text("of lines"), 22*11+39, 315, white_color,  black_color);
   outline_textout_centre(virtual_screen, datafile[SMALL_FONT].dat,
      get_config_text("Points"), 22*11+105, 312, white_color, black_color);

   for (g = 0; g < 4; g++) {
      char number[2] = "a";
      number[0] = 49 + g;
      outline_textout_centre(virtual_screen, datafile[SMALL_FONT].dat,
         number, 22*11+39, 335 + 15*g, white_color, black_color);
      outline_textout_centre(virtual_screen, datafile[SMALL_FONT].dat,
         given_points[g], 22*11+105, 335 + 15*g, white_color, black_color);
   }
   
   blit(virtual_screen, screen, 251, 285, 251, 285, 389, 286);
   
   _dibuja_ficha(0);
   dibuja_tablero(0);

   /* Finally,  this is the loop */
   while (!key[KEY_ESC] && !pierde[0]) {
      hot_keys();
      yield_timeslice();
      
      if (key[TECLA_PAUSE])
         pausa();

      for (g = 1; g <= _number_of_players; g++)
         _read_player_input(g);

      if (count_old[0]+_speed[0] < tick_counter) {
         _choque_abajo(0);
         count_old[0] = tick_counter;
      }
   }
   
   RECOVER_BACKGROUND();
   /* We look if we have to wait for the game over laugh to shut up */
   if (pierde[0]) {
      clear_keybuf();
      count_old[0] = tick_counter + (30*7.5);
      while (count_old[0] > tick_counter)
         yield_timeslice();
   }
   
   /* Look at the player score and see if he made new high score */
   if (score[0]>=high_scores[9].score) {
      got_score(1);
      print_high_scores(0);
   }
   else
      print_high_scores(1);
   
   clear_keybuf();
   while (!keypressed()) {
      hot_keys();
      yield_timeslice();
   }
   stop_sample(datafile[RISA].dat);

   RECOVER_BACKGROUND();
   show_mouse(screen);
}



static void _read_player_input(int which_player)
{
   int wp = which_player - 1;
   ASSERT(which_player == 1 || which_player == 2);

   if (_number_of_players == 1){
      if ((key[TECLA_1_UP] && _last_rotate[wp] < tick_counter) ||
          (key[TECLA_2_UP] && _last_rotate[wp] < tick_counter)) {
         _choque_frame(0);
         _last_rotate[wp] = tick_counter+5;
      }

      if ((key[TECLA_1_DOWN] && count_old[wp]+1 < tick_counter) ||
          (key[TECLA_2_DOWN] && count_old[wp]+1 < tick_counter))
         count_old[wp] -= _speed[wp];

      if ((key[TECLA_1_RIGHT] && _last_move[wp] < tick_counter) ||
          (key[TECLA_2_RIGHT] && _last_move[wp] < tick_counter)) {
         _desp_x[wp] = 1;
         _choque_h(0);
         _last_move[wp]=tick_counter+3;
      }

      if ((key[TECLA_1_LEFT] && _last_move[wp] < tick_counter) ||
          (key[TECLA_2_LEFT] && _last_move[wp] < tick_counter)) {
         _desp_x[wp]=-1;
         _choque_h(0);
         _last_move[wp]=tick_counter+3;
      }
   } else switch(which_player) {
      case 1:
         if (key[TECLA_1_UP] && _last_rotate[wp] < tick_counter && pierde[wp] == 0) {
            _choque_frame(0);
            _last_rotate[wp] = tick_counter+5;
         }
         if (key[TECLA_1_RIGHT] && _last_move[wp] < tick_counter && pierde[wp] == 0) {
            _desp_x[wp] =+ 1;
            _choque_h(0);
            _last_move[wp]=tick_counter+3;
         }
         if (key[TECLA_1_LEFT] && _last_move[wp] < tick_counter && pierde[wp] == 0) {
            _desp_x[wp] =- 1;
            _choque_h(0);
            _last_move[wp]=tick_counter+3;
         }
         if (key[TECLA_1_DOWN] && count_old[wp]+1 < tick_counter && pierde[wp] == 0)
            count_old[wp] -= _speed[wp];
         break;
         
      case 2:
         if (key[TECLA_2_UP] && _last_rotate[1] < tick_counter && pierde[wp] == 0) {
            _choque_frame(1);
            _last_rotate[wp]=tick_counter+5;
         }
         if (key[TECLA_2_RIGHT] && _last_move[1] < tick_counter && pierde[wp] == 0) {
            _desp_x[wp] = +1;
            _choque_h(1);
            _last_move[wp]=tick_counter+3;
         }
         if (key[TECLA_2_LEFT] && _last_move[1] < tick_counter && pierde[wp] == 0) {
            _desp_x[wp] = -1;
            _choque_h(1);
            _last_move[wp] = tick_counter+3;
         }
         if (key[TECLA_2_DOWN] && count_old[1]+1 < tick_counter && pierde[wp] == 0)
            count_old[wp] -= _speed[wp];

         break;
      default:
         ASSERT(0);
         break;
   }
}

void two_player(void)
{
   show_mouse(NULL);
   /* This is like player one,  so don't expect too many coments ;-) */

   blit(virtual_screen, background, 0, 0, 0, 0, virtual_screen->w, virtual_screen->h);
   _number_of_players = 2;

   rectfill(background, 251, 20, 381, 130,  black_color);
   rectfill(virtual_screen, 251, 20, 381, 130,  black_color);
   rectfill(virtual_screen, 251, 285, 381, 400,  black_color);
   rectfill(background, 251, 285, 381, 400,  black_color);

   rectfill(background, 19, 19, 19+220, 19+440,  black_color);
   rectfill(virtual_screen, 19, 19, 19+220, 19+440,  black_color);

   rectfill(background, 397, 19, 397+220, 19+440,  black_color);
   rectfill(virtual_screen, 397, 19, 397+220, 19+440,  black_color);

   _init_game_variables();

   clear_keybuf();
   count_old[0] = _last_move[0] = _last_rotate[0] = tick_counter;
   count_old[1] = _last_move[1] = _last_rotate[1] = count_old[0];
   
   _dibuja_ficha(0);
   dibuja_tablero(0);
   _dibuja_ficha(1);
   dibuja_tablero(1);
   
   /* again, but for 2 players */
   while (!key[KEY_ESC] && _pierde_ambos < 2) {
      hot_keys();
      yield_timeslice();
      
      if (key[TECLA_PAUSE])
         pausa();

      _read_player_input(1);

      if (count_old[0]+_speed[0] < tick_counter && pierde[0] == 0) {
         _choque_abajo(0);
         count_old[0] = tick_counter;
      }

      _read_player_input(2);

      if (count_old[1]+_speed[1] < tick_counter && pierde[1] == 0) {
         _choque_abajo(1);
         count_old[1] = tick_counter;
      }
   }
   
   RECOVER_BACKGROUND();
   if (pierde[0] && pierde[1]) {
      clear_keybuf();
      count_old[0] = tick_counter+(30*7.5);
      while (count_old[0] > tick_counter)
         yield_timeslice();
   }
   
   while (1) {
      if (score[0] < high_scores[9].score && score[1] < high_scores[9].score) {
         print_high_scores(3);
         break;
      }
      if (score[0] >= high_scores[9].score && score[1] < high_scores[9].score) {
         got_score(1);
         print_high_scores(2);
         break;
      }
      if (score[0] >= high_scores[9].score && score[1] >= high_scores[9].score) {
         got_score(1);
         if (score[1] >= high_scores[9].score) {
            got_score(2);
            print_high_scores(0);
            break;
         } else {
            print_high_scores(2);
            break;
         }
      }
      if (score[0] < high_scores[9].score && score[1] >= high_scores[9].score) {
         got_score(2);
         print_high_scores(1);
         break;
      }
   }
   
   clear_keybuf();
   while (!any_input_used()) {
      hot_keys();
      yield_timeslice();
   }
   stop_sample(datafile[RISA].dat);

   show_mouse(screen);
}



static void _sube(int wp, int lin)
{
   int x, y, error = 0, other_player;

   other_player = wp+1;
   if (other_player > 1)
      other_player = 0;

   _last[wp] += lin;
   while (_last[wp] >= death_lines && !pierde[other_player]) {
      if (_last[wp] >= death_lines && !pierde[other_player]) {
         _last[wp] -= death_lines;
      
         play_sample(datafile[UNA_MAS].dat, 255, 128, 1000, 0);
         _borra_ficha(other_player);
      
         for (y = 0; y < 19; y++)
            for (x = 1;x < 11; x++)
               arena[other_player][y][x] = arena[other_player][y+1][x];
      
         for (x = 1; x < 11; x++)
            arena[other_player][19][x] = rand()%7;
            arena[other_player][19][rand()%10] = 0;
      
         for (y = 0; y < 4; y++)
            for (x = 0; x < 4; x++)
               if (ficha[other_player].now[_frame[other_player]][y][x] &&
                  arena[other_player][y+_ficha_y[other_player]][x+_ficha_x[other_player]])
                  error++;
      
         if (error) {
            _ficha_y[other_player]--;
            _dibuja_ficha(other_player);
            _lineas(other_player);
            _ficha_y[other_player] = 1;
            _ficha_x[other_player] = 5;
            _ficha_nueva(other_player);
            _frame[other_player] = 0;
            _dibuja_ficha(other_player);
            dibuja_tablero(other_player);
            play_sample(datafile[LINE].dat, 255, 128, 1000, 0);
         } else {
            _dibuja_ficha(other_player);
            dibuja_tablero(other_player);
         }
      }
   }
}

/* This gets a new block for the first player. */
static void _ficha_nueva(int wp)
{
   int f, yb, xb, ran, choq = 0;
   
   ran = _order_of_blocks[_random_counter[wp]];
   _random_counter[wp]++;
   if (_random_counter[wp] > 249999)
      _random_counter[wp] = 0;
   
   for (f = 0; f < 4; f++)
      for (yb = 0; yb < 4; yb++)
         for (xb = 0; xb < 4; xb++)
            ficha[wp].now[f][yb][xb] = ficha[wp].next[f][yb][xb];
   
   _frames_ficha[wp] = _frames_prox[wp];
   
   for (f = 0; f < 4; f++)
      for (yb = 0; yb < 4; yb++)
         for (xb = 0; xb < 4; xb++)
            ficha[wp].next[f][yb][xb] = blocks->line[yb][(ran*16)+(f*4)+xb];

   _frames_prox[wp] = 4;
   
   for (yb = 0; yb < 4; yb++)
      for (xb = 0; xb < 4; xb++)
         if (ficha[wp].now[0][yb][xb] &&
             arena[wp][yb+_ficha_y[wp]][xb+_ficha_x[wp]])
            choq++;
   
   if (choq && _starting_game_up) {
      pierde[wp]++;
      _pierde_ambos++;
   
      play_sample(datafile[RISA].dat, 255, 128, 1000, 0);
   }
}

/* We look if the player can rotate the piece,  this is,  change the _frame[0]. */
static void _choque_frame(int wp)
{
   int x, y, choque = 0, f;

   _borra_ficha(wp);
   if (_frame[wp] == _frames_ficha[wp]-1)
      f=0;
   else
      f = _frame[wp]+1;

   for (y = 0; y < 4; y++)
      for (x = 0; x < 4; x++)
         if (ficha[wp].now[f][y][x] && arena[wp][y+_ficha_y[wp]][x+_ficha_x[wp]])
            choque++;

   if (!choque) {
      _frame[wp] = f;
      _dibuja_ficha(wp);
      dibuja_tablero(wp);
      play_sample(datafile[ROTATE].dat, 255, 128, 1000, 0);
   } else {
      _dibuja_ficha(wp);
      dibuja_tablero(wp);
   }
}

/* We look if the player can move the piece left or right */
static void _choque_h(int wp)
{
   int x, y, choque = 0;

   _borra_ficha(wp);
   for (y = 0; y < 4; y++)
      for (x = 0; x < 4; x++)
         if (ficha[wp].now[_frame[wp]][y][x] &&
             arena[wp][y+_ficha_y[wp]][x+_ficha_x[wp]+_desp_x[wp]])
               choque++;

   if (!choque) {
      _ficha_x[wp] += _desp_x[wp];
      _dibuja_ficha(wp);
      dibuja_tablero(wp);
   } else {
      _dibuja_ficha(wp);
      dibuja_tablero(wp);
   }
}

/* This erases the block from the arena */
static void _borra_ficha(int wp)
{
   int x, y;

   for (y = 0; y < 4; y++)
      for (x = 0; x < 4; x++)
         if (ficha[wp].now[_frame[wp]][y][x])
            arena[wp][y+_ficha_y[wp]][x+_ficha_x[wp]] = 0;
}

/* We look if the block can fall more or has reached the bottom of the arena */
static void _choque_abajo(int wp)
{
   int x, y, choque = 0;

   _borra_ficha(wp);
   for (y = 0; y < 4; y++)
      for (x = 0; x < 4; x++)
         if (ficha[wp].now[_frame[wp]][y][x] &&
             arena[wp][y+_ficha_y[wp]+1][x+_ficha_x[wp]]!=0)
            choque++;

   if (choque) {
      _dibuja_ficha(wp);
      dibuja_tablero(wp);
      _lineas(wp);
      _ficha_y[wp] = 1;
      _ficha_x[wp] = 5;
      _ficha_nueva(wp);
      _frame[wp] = 0;
      _dibuja_ficha(wp);
      dibuja_tablero(wp);
      play_sample(datafile[LINE].dat, 255, 128, 1000, 0);
   } else {
      _ficha_y[wp]++;
      _dibuja_ficha(wp);
      dibuja_tablero(wp);
   }
}

/* Analog to _borra_ficha(),  this draws the piece on the arena 'buffer' */
static void _dibuja_ficha(int wp)
{
   int x, y;
   for (y = 0; y < 4; y++)
      for (x = 0; x < 4; x++)
         if (ficha[wp].now[_frame[wp]][y][x])
            arena[wp][y+_ficha_y[wp]][x+_ficha_x[wp]] =
               ficha[wp].now[_frame[wp]][y][x];
}

/* We look if the player made a full line and modify the score and so... */
static void _lineas(int wp)
{
   int x, y, linea[4][1] = {{0}, {0}, {0}, {0}}, lin = 0, bucle, other_player;
   
   for (y = 0; y < 20; y++) {
      if (arena[wp][y][1] && arena[wp][y][2] && arena[wp][y][3] &&
          arena[wp][y][4] && arena[wp][y][5] && arena[wp][y][6] &&
          arena[wp][y][7] && arena[wp][y][8] && arena[wp][y][9] &&
          arena[wp][y][10]) {
         linea[lin][0] = y;
         lin++;
      }
   }
   
   for (bucle = 0; bucle < lin; bucle++){
      for (x = 1; x < 11; x++)
         arena[wp][ linea[bucle][0] ][x] = 0;

      for (;linea[bucle][0] > 0; linea[bucle][0]--) {
         for (x = 1; x < 11; x++) {
            arena[wp][linea[bucle][0]][x] = arena[wp][linea[bucle][0]-1][x];
         }
      }
   }
   
   lines[wp] -= lin;
   total_lines[wp] += lin;
   switch (lin) {
      case 0: break;
      case 1: score[wp] += 10; break;
      case 2: score[wp] += 50; break;
      case 3: score[wp] += 125; break;
      case 4: score[wp] += 500; break;
      default: ASSERT(0); break;
   }
   
   if (lines[wp] <= 0) {
      lines[wp] += 10;
      level[wp]++;

      if (_speed[wp] < 5)
         _speed[wp]--;
      else
         _speed[wp] -= 4;

      score[wp] += 1000;
      play_sample(datafile[LEVELUP].dat, 255, 128, 1000, 0);

      /* Now we want to know if we can grow the arena of the other player */
      other_player = wp+1;
      if (other_player>1)
         other_player = 0;

      if (pierde[other_player] == 0 && death_match == 1) {
         level[other_player]++;
         lines[other_player] = 10;
         if (_speed[other_player] < 5)
            _speed[other_player]--;
         else
            _speed[other_player] -= 4;
         dibuja_tablero(other_player);
      }
   }
   
   if (score[wp] >= high_scores[0].score)
      next_score[wp] = 0;
   else {
      for (x = 0; x < 9; x++) {
         if (score[wp] >= high_scores[x+1].score) {
            next_score[wp] = high_scores[x].score;
            x += 10;
         }
      }
   }

   if (next_score[wp] == 0)
      uszprintf(next_score_buf[wp], NEXT_SCORE_BUF_SIZE,
         get_config_text("You are the best!!!"));
   else
      uszprintf(next_score_buf[wp], NEXT_SCORE_BUF_SIZE, "%s%d",
         get_config_text("Next High Score:"), next_score[wp]);

   switch(wp) {
      case 0:
         if (death_match == 1)
            _sube(wp, lin);
         acquire_screen();
         blit(virtual_screen, screen, 0, 0, 0, 0, 397, 320);
         blit(virtual_screen, screen, 0, 319, 0, 319, 262, 160);
         release_screen();
         if (lin != 0)
            play_sample(datafile[MAKELINE].dat, 255, 128, 1000, 0);
         if (lin == 4)
            play_sample(datafile[FOURLINES].dat, 255, 128, 1000, 0);
         break;
      case 1:
         if (death_match == 1)
            _sube(wp, lin);
         acquire_screen();
         blit(virtual_screen, screen, 397, 0, 397, 0, 242, 285);
         blit(virtual_screen, screen, 251, 285, 251, 285, 388, 286);
         release_screen();
         if (lin != 0)
            play_sample(datafile[MAKELINE].dat, 255, 128, 1000, 0);
         if (lin == 4)
            play_sample(datafile[FOURLINES].dat, 255, 128, 1000, 0);
         break;
      default:
         ASSERT(0);
         break;
      }
}

/* The following resets the variables we are going to use like the scores, 
 * the arena,  the random table,  etc...
 */

static void _init_game_variables(void)
{
   int yb, xb, g;

   _starting_game_up = 0;

   for (yb=0;yb < 4;yb++) {
      for (xb=0;xb < 4;xb++) {
         for (g=0;g < 4;g++) {
            ficha[0].next[yb][xb][g] = ficha[0].now[yb][xb][g] = 0;
            ficha[0].next[yb][xb][g] = ficha[1].now[yb][xb][g] = 0;
         }
      }
   }

   _frames_prox[0] = _frames_ficha[0] = _frames_prox[1] = _frames_ficha[1] = 0;

   /* We initialise the random table with values from 0-6. */
   /* srandom( time(NULL) ); */
   _random_counter[0] = _random_counter[1] = 0;
   for (xb = 0; xb < 250000; xb++)
      _order_of_blocks[xb] = rand()%7;

   /* First we initialise and erase the 'black' free space. */
   for (yb = 0; yb < 20; yb++) {
      for (xb=0;xb < 11;xb++) {
         /* Clean the arenas and the old buffers */
         arena[0][yb][xb] = 0;
         arena[1][yb][xb] = 0;
         old_arena[0][yb][xb] = 10;
         old_arena[1][yb][xb] = 10;
         arena[0][20][xb] = 10;
         arena[1][20][xb] = 10;
      }
      arena[0][yb][0] = 10;
      arena[1][yb][0] = 10;
      arena[0][yb][11] = 10;
      arena[1][yb][11] = 10;
   }
   
   /* We put some values to the default settings */

   for (xb = 0; xb < 2; xb++) {
      lines[xb] = total_lines[xb] = score[xb] = level[xb] = _last[xb] = 0;
      next_score[xb] = high_scores[xb].score;

      uszprintf(lines_buf[xb], LINES_BUF_SIZE, "%s %d",
         get_config_text("Lines to next level:"), lines[xb]);
      uszprintf(total_lines_buf[xb], TOTAL_LINES_BUF_SIZE, "%s %d",
         get_config_text("Total lines:"), total_lines[xb]);
      uszprintf(level_buf[xb], LEVEL_BUF_SIZE, "%s %d",
         get_config_text("Level:"),  level[xb]);
      uszprintf(score_buf[xb], SCORE_BUF_SIZE, "%s %d",
         get_config_text("Score:"),  score[xb]);
      uszprintf(next_score_buf[xb], NEXT_SCORE_BUF_SIZE, "%s%d",
         get_config_text("Next High Score:"), high_scores[9].score);
   }

   _ficha_nueva(0);
   _ficha_nueva(0);
   _ficha_nueva(1);
   _ficha_nueva(1);
   
   _ficha_x[0] = _ficha_x[1] = 5;
   _ficha_y[0] = _ficha_y[1] = 1;
   
   _pierde_ambos = 0;
   pierde[0] = pierde[1] = 0;
   _speed[0] = _speed[1] = 30;

   _starting_game_up = 2;
}

