#ifdef MSS
#include <mss.h>
#endif MSS
#include "intro.h"
#include "multitet.h"
#include "data.h"
#include "misc.h"
#include <allegro.h>

static void _create_title(void);

void intro(void)
{
   /* First logo of Gogosoftware... */
   stretch_blit(datafile[LOGO].dat, screen, 0, 0, 320, 240, 0, 0, 640, 480);
   fade_in(datafile[PAL_LOGO].dat, 4);
   
   multitet_wait(SECOND*10);
   fade_out(4);

   /* Presents... */
   acquire_screen();
   clear_bitmap(screen);
   textout_centre(screen, datafile[BIG_FONT].dat, get_config_text("PRESENTS..."),
      320, 220, palette_color[255]);
   release_screen();
   
   fade_in(datafile[PAL_TITLE].dat, 4);
   multitet_wait(SECOND*2);
   fade_out(4);

   /* Tetris by blah blah blah... */
   _create_title();

   draw_rle_sprite(virtual_screen, datafile[BMP_TITLE].dat, 
      (TSCREEN_W>>1)-(((RLE_SPRITE *)datafile[BMP_TITLE].dat)->w>>1),
      (TSCREEN_H>>1)-(((RLE_SPRITE *)datafile[BMP_TITLE].dat)->h>>1));
                   
   textout_centre(virtual_screen, font,
      get_config_text("Made by Grzegorz Adam Hankiewicz"),
      320, 460, palette_color[0]);
   textout_centre(virtual_screen, font,
      get_config_text("Special thanks to DJ Delorie, Shawn Hargreaves and Robert Hoehne"),
      320, 470, palette_color[0]);

   stretch_virtual_screen();
   
   play_sample(datafile[WELCOME].dat, 255, 128, 1000, 0);
   fade_in(datafile[PAL_TITLE].dat, 1);
   
   multitet_wait(SECOND*10);
   fade_out(1);
}

/* This draws the gold vertical lines of the intro screen. */
static void _create_title(void)
{
   int x = 0, color = 162, counter_color = 0;

   for (x = 0; x < 320; x++) {
      vline(virtual_screen, x, 0, 479, color);
      counter_color++;
      if (counter_color >= 2) {
         counter_color = 0;
         color--;
      }
   }

   for (x = 320; x < 640; x++) {
      vline(virtual_screen, x, 0, 479, color);
      counter_color++;
      if (counter_color >= 2) {
         counter_color = 0;
         color++;
      }
   }
}

void outro(void)
{
   char **lines;

   //printf("Hey!\n");
   exit(1);
   //remove_sound();

   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   show_mouse(0);

   acquire_screen();
   rectfill(screen, 0, 0, TSCREEN_W-1, TSCREEN_H-1, makecol(0, 0, 0));

   /* Show 'bye, bye' screen... */
   lines = get_multiple_config_text("bye_bye_message");
   if (!lines)
      outline_textout_centre(screen, datafile[BIG_FONT].dat,
         get_config_text("Internal error reading multiple lines"), 
         TSCREEN_W/2, TSCREEN_H/2, menu_color, black_color);
   else {
      int f;
      for (f = 0; lines[f]; f++) {
         outline_textout(screen, font, lines[f], 10, 100+f*12,
            menu_color, black_color);
         free(lines[f]);
      }
      free(lines);
   }
   while (any_input_used())
      yield_timeslice();
   while (!any_input_used())
      yield_timeslice();

}

// vim:tabstop=3 shiftwidth=3 softtabstop=3 expandtab
