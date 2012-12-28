/* Tetris clone started by Grzegorz Adam Hankiewicz in May of 1997.
 * You can find me at http://gradha.infierno.org/. Email: gradha@iname.com
 * Visit Gogosoftware at http://welcome.to/gogosoftware/
 *
 * This game would not exist if it was not for Allegro, the wonderful
 * game library made by Shawn Hargreaves. You can contact Shawn at
 * http://www.talula.demon.co.uk/ . The email is : shawn@talula.demon.co.uk
 *
 * The windows version has been made thanks to the effort of Stefan
 * Schimanski and George Foot. The first created Winallegro and the second
 * ported it to RSXNTDJ.
 *
 * More and precise credits will be given in the next version, this one was
 * written in a hurry.
 *
 * To rebuild the dos version you need Allegro and DJGPP
 * To rebuild the win version you need Winallegro, RSXNTDJ and George's patches
 *
 * To run the game you will need at least a 486/DX66, a SVGA card and like 3
 * or 4 megs or ram. That's for the dos version. For the Windows version, I
 * think you need DirectX 5 and well, Win :-) so you will _surely_ have more
 * hardware than required.
 *
 * The Tetris game is very well known, however, maybe you just woke up from
 * a cryogenig pod (like Conrad B. Hart :-) or have lost your memmory in a
 * Starship accident (Lev Arris, Privateer 2) this is it.
 *
 * You have a fixed screen, 10 units wide and 20 high. Some 'blocks' with
 * different forms fall, and you have to put them in such a way you 'fill'
 * a line. Then, this line will be deleted, allowing the player to continue
 * the game before the screen fills up. Every level, the 'blocks' will fall
 * faster and faster, until either the computer or the player explode ;-)
 *
 * We will start with a usual screen where you can configure the keys and
 * where you choose to play 1 or 2 players at the same time. There will be
 * cool? music and some FX.
 *
 * Ready? Then, fasten your seabelts :)
 */

#ifdef MSS
#include <mss.h>
#endif

#include "multitet.h"
#include "gamecore.h"
#include "init.h"
#include "intro.h"
#include "error.h"
#include "score.h"
#include "misc.h"
#include "error.h"
#include "gui.h"
#include "data.h"       /* Header file from the grabber */

#include <allegro.h> /* Needed header to use Allegro */


/* variable for _inc_ticks(); in init.c */
volatile int tick_counter;

DATAFILE *datafile;
BITMAP *virtual_screen, *pause_screen, *background, *blocks;
PALETTE pal_title, pal_pause;

/*
 * The most important functions are from where we call one of the
 * loops: one_player or two_player. There we will check the position of the
 * blocks and we will update the screen for the players.  is the last
 * routine of the source code.
 */

/* Draws the arena buffer to the screen. Notice that we use the dirty rectan-
 * gle technic
 */
void dibuja_tablero(int wp)
{
   int x,y,temp1,temp2;
   
   uszprintf(lines_buf[wp], LINES_BUF_SIZE, "%s %d",
      get_config_text("Lines to next level:"), lines[wp]);
   uszprintf(total_lines_buf[wp], TOTAL_LINES_BUF_SIZE, "%s %d",
      get_config_text("Total lines:"), total_lines[wp]);
   uszprintf(level_buf[wp], LEVEL_BUF_SIZE, "%s %d",
      get_config_text("Level:"), level[wp]);
   uszprintf(score_buf[wp], SCORE_BUF_SIZE, "%s %d",
      get_config_text("Score:"), score[wp]);

   temp2 = 19;
   for (y = 0; y < 20; y++){
      /* With the following if we decide if we draw the left or right board */
      if (wp==0)  temp1 = 19;
      else        temp1 = 397;

      for (x = 1; x < 11; x++){
         if (old_arena[wp][y][x] != arena[wp][y][x]) {
            if (arena[wp][y][x] == 0)
               blit(background, virtual_screen, temp1, temp2, temp1, temp2, 22, 22);
            else {
               draw_rle_sprite(virtual_screen,
                  datafile[B_0 + arena[wp][y][x]].dat,temp1,temp2);
            }
         }

         temp1 += 22;
      }
      temp2 += 22;
   }
   
   /* Now we draw the next block... if the player wants to see it... */
   if (visible_blocks == 1) {
      if (wp == 0)   temp2 = 140;
      else           temp2 = 412;

      for (y = 0; y < 2; y++) {
         temp1 = 261;

         for (x = 0; x < 4; x++) {

            if (ficha[wp].next[0][y][x] == 0)
               blit(background,virtual_screen,temp1,temp2,temp1,temp2,22,22);
            else draw_rle_sprite(virtual_screen,
                  datafile[ B_0+ficha[wp].next[0][y][x] ].dat,temp1,temp2);

            temp1 += 22;
         }
         temp2 += 22;
      }
   }
   
   /* Print the scores... */
   if (wp == 0) {
      temp1=0;
      temp2=0;
   } else {
      temp1=265;
      temp2=270;
   }

   /* Print the scores... */
   blit(background,virtual_screen,251,20+temp1,251,20+temp1,130,110);

   if (wp == 0)
      outline_textout(virtual_screen, datafile[BIG_FONT].dat,
         get_config_text("PLAY(er). 1"), 261, 20+temp2,
         white_color, black_color);
   else
      outline_textout(virtual_screen, datafile[BIG_FONT].dat,
         get_config_text("PLAY(er). 2"), 261, 20+temp2,
         white_color, black_color);

   outline_textout(virtual_screen, datafile[SMALL_FONT].dat, level_buf[wp],
      261, 50+temp2, white_color, black_color);
   outline_textout(virtual_screen, datafile[SMALL_FONT].dat, lines_buf[wp],
      261, 65+temp2, white_color, black_color);
   outline_textout(virtual_screen, datafile[SMALL_FONT].dat, total_lines_buf[wp],
      261, 80+temp2, white_color, black_color);
   outline_textout(virtual_screen, datafile[SMALL_FONT].dat, score_buf[wp],
      261, 95+temp2, white_color, black_color);
   outline_textout(virtual_screen, datafile[SMALL_FONT].dat, next_score_buf[wp],
      261,110+temp2, white_color, black_color);

   acquire_screen();

   temp2 = 19;
   for (y = 0; y < 20; y++) {

      /* With the following if we decide if we draw the left or right board */
      if (wp==0)  temp1 = 19;
      else        temp1 = 397;

      for (x = 1; x < 11; x++) {
         if (old_arena[wp][y][x] != arena[wp][y][x]) {
            blit(virtual_screen,screen,temp1,temp2,temp1,temp2,22,22);
            old_arena[wp][y][x] = arena[wp][y][x];
         }
         temp1 += 22;
      }
      temp2 += 22;
   }
   
   if (wp == 0)   temp1 = 20;
   else           temp1 = 285;

   /* Scoreboard */
   blit(virtual_screen, screen, 250, temp1, 250, temp1, 132, 180);

   release_screen();

   if (pierde[wp]!=0) {
      outline_textout_centre(screen, datafile[BIG_FONT].dat,
         get_config_text("GAME OVER !"), 132+(375*wp), 100,
         red_color, black_color);
   }
}

void draw_menu_back_in_buffer(void)
{
   blit(background, virtual_screen, 0, 0, 0, 0, background->w, background->h);
   draw_rle_sprite(virtual_screen, datafile[BMP_TETRIS].dat, 
                 (SCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TETRIS].dat)->w>>1), 
                  50);

   outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("Press F1 for help"), 
                          SCREEN_W>>1, SCREEN_H-50, menu_color, black_color);
   outline_textout_centre(virtual_screen, font, get_config_text("Made by Grzegorz Adam Hankiewicz"), 
                          SCREEN_W>>1, SCREEN_H-20, red_color, black_color);
   outline_textout_centre(virtual_screen, font, get_config_text("Special thanks to DJ Delorie,  Shawn Hargreaves and Robert Hoehne"), 
                          SCREEN_W>>1, SCREEN_H-10, red_color, black_color);

   show_mouse(NULL);
   acquire_screen();
   blit(virtual_screen, screen, 0, 0, 0, 0, virtual_screen->w, virtual_screen->h);
   release_screen();
   show_mouse(screen);
}


/* We redefine the keys. */
void redefine_keys(void)
{
   int counter_2;

   show_mouse(NULL);
   
   play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
   
   blit(background, virtual_screen, 0, 0, 0, 0, 640, 480);
   draw_rle_sprite(virtual_screen, datafile[BMP_TETRIS].dat, 
                 (SCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TETRIS].dat)->w>>1), 
                  50);
   
   outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("PRESS D TO USE DEFAULT KEYS"), 320, 200, menu_color,  black_color);
   outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("PRESS ANY OTHER TO REDEFINE KEYS"), 320, 240, menu_color,  black_color);

   acquire_screen();
   blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
   release_screen();
   
   while (any_input_used());

   if ((readkey() >> 8)==KEY_D) {
      TECLA_1_UP=KEY_UP, TECLA_1_DOWN=KEY_DOWN, TECLA_1_LEFT=KEY_LEFT, 
      TECLA_1_RIGHT=KEY_RIGHT, TECLA_2_UP=KEY_W, TECLA_2_DOWN=KEY_S, 
      TECLA_2_LEFT=KEY_A, TECLA_2_RIGHT=KEY_D, TECLA_MUSIC=KEY_M, 
      TECLA_PAUSE=KEY_P;
      
      blit(background, virtual_screen, 0, 0, 0, 0, 640, 480);
      draw_rle_sprite(virtual_screen, datafile[BMP_TETRIS].dat, 
                    (SCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TETRIS].dat)->w>>1), 
                     50);
      outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("DEFAULT KEYS SET"), 320, 220, menu_color,  black_color);
      acquire_screen();
      blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
      release_screen();
      play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
      counter_2=tick_counter;
      do { yield_timeslice();
      } while (!keypressed() && counter_2 + 60 > tick_counter);
   } else {
      play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
      blit(background, virtual_screen, 0, 0, 0, 0, 640, 480);
      draw_rle_sprite(virtual_screen, datafile[BMP_TETRIS].dat, 
                    (SCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TETRIS].dat)->w>>1), 
                     50);
      outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("PLAYER 1. PRESS A KEY FOR LEFT"), 320, 220, menu_color,  black_color);
      acquire_screen();
      blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
      release_screen();
      clear_keybuf();
      TECLA_1_LEFT=readkey() >> 8;
   
      play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
      blit(background, virtual_screen, 0, 0, 0, 0, 640, 480);
      draw_rle_sprite(virtual_screen, datafile[BMP_TETRIS].dat, 
                    (SCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TETRIS].dat)->w>>1), 
                     50);
      outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("PLAYER 1. PRESS A KEY FOR RIGHT"), 320, 220, menu_color,  black_color);
      acquire_screen();
      blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
      release_screen();
      clear_keybuf();
      TECLA_1_RIGHT=readkey() >> 8;
   
      play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
      blit(background, virtual_screen, 0, 0, 0, 0, 640, 480);
      draw_rle_sprite(virtual_screen, datafile[BMP_TETRIS].dat, 
                    (SCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TETRIS].dat)->w>>1), 
                     50);
      outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("PLAYER 1. PRESS A KEY FOR ROTATE"), 320, 220, menu_color,  black_color);
      acquire_screen();
      blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
      release_screen();
      clear_keybuf();
      TECLA_1_UP=readkey() >> 8;
   
      play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
      blit(background, virtual_screen, 0, 0, 0, 0, 640, 480);
      draw_rle_sprite(virtual_screen, datafile[BMP_TETRIS].dat, 
                    (SCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TETRIS].dat)->w>>1), 
                     50);
      outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("PLAYER 1. PRESS A KEY FOR DOWN"), 320, 220, menu_color,  black_color);
      acquire_screen();
      blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
      release_screen();
      clear_keybuf();
      TECLA_1_DOWN=readkey() >> 8;
   
      play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
      blit(background, virtual_screen, 0, 0, 0, 0, 640, 480);
      draw_rle_sprite(virtual_screen, datafile[BMP_TETRIS].dat, 
                    (SCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TETRIS].dat)->w>>1), 
                     50);
      outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("PLAYER 2. PRESS A KEY FOR LEFT"), 320, 220, menu_color,  black_color);
      acquire_screen();
      blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
      release_screen();
      clear_keybuf();
      TECLA_2_LEFT=readkey() >> 8;
   
      play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
      blit(background, virtual_screen, 0, 0, 0, 0, 640, 480);
      draw_rle_sprite(virtual_screen, datafile[BMP_TETRIS].dat, 
                    (SCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TETRIS].dat)->w>>1), 
                     50);
      outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("PLAYER 2. PRESS A KEY FOR RIGHT"), 320, 220, menu_color,  black_color);
      acquire_screen();
      blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
      release_screen();
      clear_keybuf();
      TECLA_2_RIGHT=readkey() >> 8;
   
      play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
      blit(background, virtual_screen, 0, 0, 0, 0, 640, 480);
      draw_rle_sprite(virtual_screen, datafile[BMP_TETRIS].dat, 
                    (SCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TETRIS].dat)->w>>1), 
                     50);
      outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("PLAYER 2. PRESS A KEY FOR ROTATE"), 320, 220, menu_color,  black_color);
      acquire_screen();
      blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
      release_screen();
      clear_keybuf();
      TECLA_2_UP=readkey() >> 8;
   
      play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
      blit(background, virtual_screen, 0, 0, 0, 0, 640, 480);
      draw_rle_sprite(virtual_screen, datafile[BMP_TETRIS].dat, 
                    (SCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TETRIS].dat)->w>>1), 
                     50);
      outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("PLAYER 2. PRESS A KEY FOR DOWN"), 320, 220, menu_color,  black_color);
      acquire_screen();
      blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
      release_screen();
      clear_keybuf();
      TECLA_2_DOWN=readkey() >> 8;
   
      play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
      blit(background, virtual_screen, 0, 0, 0, 0, 640, 480);
      draw_rle_sprite(virtual_screen, datafile[BMP_TETRIS].dat, 
                    (SCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TETRIS].dat)->w>>1), 
                     50);
      outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("PRESS A KEY FOR SWITCHING MUSIC"), 320, 220, menu_color,  black_color);
      acquire_screen();
      blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
      release_screen();
      clear_keybuf();
      TECLA_MUSIC=readkey() >> 8;
   
      play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
      blit(background, virtual_screen, 0, 0, 0, 0, 640, 480);
      draw_rle_sprite(virtual_screen, datafile[BMP_TETRIS].dat, 
                    (SCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TETRIS].dat)->w>>1), 
                     50);
      outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("PRESS A KEY FOR PAUSE"), 320, 220, menu_color,  black_color);
      acquire_screen();
      blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
      release_screen();
      clear_keybuf();
      TECLA_PAUSE=readkey() >> 8;
   
      play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
   }

   blit(background, virtual_screen, 0, 0, 0, 0, 640, 480);
   draw_rle_sprite(virtual_screen, datafile[BMP_TETRIS].dat, 
                 (SCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TETRIS].dat)->w>>1), 
                  50);
   acquire_screen();
   blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
   release_screen();
   show_mouse(screen);
}

/*
 * Here is the  part of the program. From here,  we control the menu of
 * the game.
 */

int main(int argc, char *argv[])
{
   int f;
   
   /* Initialise Allegro and its components */
   install_allegro(SYSTEM_AUTODETECT, &errno, atexit);
   set_config_file("multitet.cfg");
   reload_config_texts(0);
   install_timer();      /* Installs the Allegro timer interrupt handler. You must do */
   install_keyboard();   /* this before installing any user timer routines,  and also */
   install_mouse();      /* before displaying a mouse pointer,  playing FLI anima- */
                         /* tions or MIDI music,  and using any of the GUI routines. */

/*
    Look for the "-lang" switch at command line.
   for (x=1; x<argc; x++)
      if (ustricmp(argv[x],  "-lang") == 0)
         load_language ++;
*/

   if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 640, 480))
      if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 640, 480))
         da_error_grave(allegro_error);

   set_palette(desktop_palette);
   clear_bitmap(screen);

   srand(time(NULL));
   init_config();

   init_allegro_n_other(argc,  argv);

   clear_bitmap(screen);
   set_palette(datafile[PAL_MAIN].dat);
   show_mouse(0);
   set_mouse_sprite(0);
   set_palette(black_palette);
   text_mode(-1);

   /* Detect if we want to see the wonderful intro... */
   for (f = argc-1; f > 0; f--)
      if (ustricmp(argv[f], "-nointro") == 0)
         break;
   if (!f)
      intro();
      
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);

   /* The menu stuff comes here... */
   prepare_menu_graphically(); /* Pre-draws it on the screen before fading */

   show_mouse(screen);
   fade_in(datafile[PAL_MAIN].dat, 1);

   clear_keybuf();

   menu();

   return 0;
   /*End of game */
}
END_OF_MAIN();
