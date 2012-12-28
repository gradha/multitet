#ifdef MSS
#include <mss.h>
#endif
/* Functions to draw the menus using Allegro's gui funtions */

#include "gui.h"
#include "misc.h"
#include "multitet.h"
#include "gamecore.h"
#include "misc.h"
#include "score.h"
#include "data.h"
#include "error.h"
#include "intro.h"
#include "langsel.h"
#include <allegro.h>

#define _DUMMY_SIZE           80*4
#define _DEATH_STRING_SIZE    80*4

static int _cen_text_proc(int msg, DIALOG *d, int c);
static int _button_proc(int msg, DIALOG *d, int c);
static void _correct_menu_dialog(DIALOG *dialog);
static int _update_dialog_volume();
static int _menu_hot_key();
static void _save_config(void);

void *player;

static char _dummy[4][_DUMMY_SIZE]; /* Empty strings we will sprintf texts on */
static char _death_string[_DEATH_STRING_SIZE];

DIALOG game_dlg[] =
{
   /* (dialog proc) (x) (y)   (w) (h)   (fg)  (bg)  (key) (flags)  (d1)  (d2)  (dp) */
   { _button_proc,   0,  200,   0,  0,   255,   0,    0,    D_EXIT,  0,    0,  NULL },
   { _button_proc,   0,  235,   0,  0,   255,   0,    0,    D_EXIT,  0,    0,  NULL },
   { _button_proc,   0,  270,   0,  0,   255,   0,    0,    D_EXIT,  0,    0,  NULL },
   { _button_proc,   0,  305,   0,  0,   255,   0,    0,    D_EXIT,  0,    0,  NULL },
   { _button_proc,   0,  340,   0,  0,   255,   0,    0,    D_EXIT,  0,    0,  NULL },
   { d_keyboard_proc,0,  0,    0,  0,     0,   0,    0,    0,     KEY_F1, 0,  _menu_hot_key },
   { d_keyboard_proc,0,  0,    0,  0,     0,   0,    0,    0, KEY_F10,KEY_F12,_menu_hot_key },
   { d_keyboard_proc,0,  0,    0,  0,     0,   0,    0,    0, KEY_F5 ,KEY_F6 ,_menu_hot_key },
   { d_keyboard_proc,0,  0,    0,  0,     0,   0,    0,    0, KEY_F7 ,KEY_F8 ,_menu_hot_key },
   { NULL,          0,   0,    0,  0,   255,   0,    0,    0,       0,    0,  NULL }
};
int game_dlg_exit= 4; /* Indicates button to press to exit menu. */


DIALOG options_dlg[] =
{
   /* (dialog proc) (x) (y)   (w) (h)   (fg)  (bg)  (key) (flags)  (d1)  (d2)  (dp) */
   { _button_proc,   0,  165,   0,  0,   255,   0,    0,    D_EXIT,  0,    0,  NULL },
   { _button_proc,   0,  200,   0,  0,   255,   0,    0,    D_EXIT,  0,    0,  NULL },
   { _button_proc,   0,  235,   0,  0,   255,   0,    0,    D_EXIT,  0,    0,  _death_string },
   { _button_proc,   0,  270,   0,  0,   255,   0,    0,    D_EXIT,  0,    0,  NULL },
   { _button_proc,   0,  305,   0,  0,   255,   0,    0,    D_EXIT,  0,    0,  NULL },
   { _button_proc,   0,  340,   0,  0,   255,   0,    0,    D_EXIT,  0,    0,  NULL },
   { _button_proc,   0,  375,   0,  0,   255,   0,    0,    D_EXIT,  0,    0,  NULL },
   { d_keyboard_proc,0,  0,    0,  0,     0,   0,    0,    0,     KEY_F1, 0,  _menu_hot_key },
   { d_keyboard_proc,0,  0,    0,  0,     0,   0,    0,    0, KEY_F10,KEY_F12,_menu_hot_key },
   { d_keyboard_proc,0,  0,    0,  0,     0,   0,    0,    0, KEY_F5 ,KEY_F6 ,_menu_hot_key },
   { d_keyboard_proc,0,  0,    0,  0,     0,   0,    0,    0, KEY_F7 ,KEY_F8 ,_menu_hot_key },
   { NULL,          0,   0,    0,  0,   255,   0,    0,    0,       0,    0,  NULL }
};
int options_dlg_exit= 6; /* Indicates button to press to exit menu. */

DIALOG sound_dlg[] =
{
   /* (dialog proc) (x) (y)   (w) (h)   (fg)  (bg)  (key) (flags)  (d1)  (d2)  (dp) */
   { d_slider_proc, 160,165,  320,20,     8,   0,    0,    0,       32,   0,  NULL, _update_dialog_volume },
   { d_slider_proc, 160,235,  320,20,     8,   0,    0,    0,       32,   0,  NULL, _update_dialog_volume },
   { _button_proc,   0,  270,   0,  0,   255,   0,    0,    D_EXIT,  0,    0,  _dummy[2] },
   { _button_proc,   0,  305,   0,  0,   255,   0,    0,    D_EXIT,  0,    0,  NULL },
   { _cen_text_proc, 320,130,   0,  0,     8,   -1,   0,    0,       0,    0,  _dummy[0] },
   { _cen_text_proc, 320,200,   0,  0,     8,   -1,   0,    0,       0,    0,  _dummy[1] },
   { _cen_text_proc, 320,365,   0,  0,     0,   0,    0,    0,       0,    0,  _dummy[3] },
   { d_keyboard_proc,0,  0,    0,  0,     0,   0,    0,    0,     KEY_F1, 0,  _menu_hot_key },
   { d_keyboard_proc,0,  0,    0,  0,     0,   0,    0,    0, KEY_F10,KEY_F12,_menu_hot_key },
   { d_keyboard_proc,0,  0,    0,  0,     0,   0,    0,    0, KEY_F5 ,KEY_F6 ,_menu_hot_key },
   { d_keyboard_proc,0,  0,    0,  0,     0,   0,    0,    0, KEY_F7 ,KEY_F8 ,_menu_hot_key },
   { NULL,          0,   0,    0,  0,   255,   0,    0,    0,       0,    0,  NULL }
};
int sound_dlg_exit= 3; /* Indicates button to press to exit menu. */

int actual_dlg_width;  /* This is the biggest text width in pixels
                        * of the actual dialog
                        */

static int _update_dialog_volume()
{
   vol_fx   = MIN(sound_dlg[0].d2<<3, 255);
   vol_midi = MIN(sound_dlg[1].d2<<3, 255);

   uszprintf(sound_dlg[4].dp, _DUMMY_SIZE, "%s: %d",
      get_config_text("FX volume"), vol_fx);
   uszprintf(sound_dlg[5].dp, _DUMMY_SIZE, "%s: %d",
      get_config_text("Music volume"), vol_midi);

   set_volume(vol_fx, vol_midi);

   return D_REDRAW;
}

/* This centers all the dialogs on the screen and makes their "bounding boxes"
 * big enough so that the mouse can use them to activate the text buttons.
 * And depending on the active dialog, it assigns the text strings.
 */

static void _correct_menu_dialog(DIALOG *dialog)
{
   int f=0,w=0;

   ASSERT(dialog);
   if (dialog == game_dlg) {
      game_dlg[0].dp =(void*)get_config_text("One player");
      game_dlg[1].dp =(void*)get_config_text("Two players");
      game_dlg[2].dp =(void*)get_config_text("Options");
      game_dlg[3].dp =(void*)get_config_text("View high scores");
      game_dlg[4].dp =(void*)get_config_text("Exit game");
   }
   else if (dialog == options_dlg) {
      options_dlg[0].dp =(void*)get_config_text("Sound options");
      options_dlg[4].dp =(void*)get_config_text("Redefine keys");
      options_dlg[5].dp =(void*)get_config_text("Change language");
      options_dlg[6].dp =(void*)get_config_text("Back");

      if (death_match)
         options_dlg[1].dp =(void*)get_config_text("DEATHMATCH Mode!!!");
      else
         options_dlg[1].dp =(void*)get_config_text("Normal Mode");
         
      if (visible_blocks)
         options_dlg[3].dp =(void*)get_config_text("Next block visible");
      else
         options_dlg[3].dp =(void*)get_config_text("Next block hidden");
         
      if (death_match)
         uszprintf(_death_string, _DEATH_STRING_SIZE, "%s %d",
            get_config_text("Death lines:"), death_lines);
      else
         uszprintf(_death_string, _DEATH_STRING_SIZE,
            "%s ?", get_config_text("Death lines:"));
   }
   else if (dialog == sound_dlg) {
      char get_music[] = "playing_music_x";
   
      sound_dlg[3].dp =(void*) get_config_text("Back");
      vol_fx ++; vol_midi++;
      sound_dlg[0].d2 = vol_fx >> 3;
      sound_dlg[1].d2 = vol_midi >> 3;
      vol_fx --; vol_midi--;

      get_music[14] = '0' + which_music;

      uszprintf(_dummy[2], _DUMMY_SIZE, "%s %d",
         get_config_text("MUSIC"), which_music);
      uszprintf(_dummy[3], _DUMMY_SIZE, "%s", get_config_text(get_music));
/*      sprintf(_dummy[3],"%s %s", get_config_text("PLAYING:"), gametexts[20+which_music]); */
   }
   else {
      ASSERT(0);
   }


   /* Search for greatest button width... */
   for (f=0; dialog[f].proc != NULL; f++) {
      if (dialog[f].proc == _button_proc &&
         text_length(datafile[BIG_FONT].dat,dialog[f].dp) > w) {
         w = text_length(datafile[BIG_FONT].dat,dialog[f].dp);
      }
   }

   f=0;

   /* Fount the greatest button width of the dialog */
   actual_dlg_width = w;

   while (dialog[f].proc != NULL) {
      /* Place the buttons on the screen according to the greatest width. */
      if (dialog[f].proc == _button_proc) {
         /* Our customized button found. Change it's properties. */
         dialog[f].h = text_height(datafile[BIG_FONT].dat);
         dialog[f].w = text_length(datafile[BIG_FONT].dat, dialog[f].dp);
         dialog[f].x =(SCREEN_W>>1) -(w>>1);
      }

      f++;
   }
}

/* _cen_text_proc:
 *  A text object. Just textouts the string centered and outlined, using
 *  the values of d1 if >= 0.
 */

static int _cen_text_proc(int msg, DIALOG *d, int c)
{
   c = text_length(datafile[BIG_FONT].dat, d->dp) + 40;

   switch(msg) {
      case MSG_DRAW:

         acquire_screen();
         blit(virtual_screen, screen, d->x-(c>>1), d->y, d->x-(c>>1), d->y,
            c, text_height(datafile[BIG_FONT].dat));
         outline_textout_centre(screen, datafile[BIG_FONT].dat,
                                 d->dp, d->x, d->y, 8, black_color);
         release_screen();
         break;

      case MSG_START:

         show_mouse(NULL);
         acquire_screen();
         blit(virtual_screen, screen, d->x-(c>>1), d->y, d->x-(c>>1), d->y,
            c, text_height(datafile[BIG_FONT].dat));
         outline_textout_centre(screen, datafile[BIG_FONT].dat,
                                 d->dp, d->x, d->y, 8, black_color);
         release_screen();
         show_mouse(screen);
         break;

      case MSG_END:
   
         show_mouse(NULL);
         acquire_screen();
         blit(virtual_screen, screen, d->x-(c>>1), d->y, d->x-(c>>1), d->y,
            c, text_height(datafile[BIG_FONT].dat));
         release_screen();
         show_mouse(screen);
         break;
   }

   return D_O_K;
}


/* _button_proc:
 *  A button object(the dp field points to the text string). This object
 *  can be selected by clicking on it with the mouse or by pressing its 
 *  keyboard shortcut. If the D_EXIT flag is set, selecting it will close 
 *  the dialog, otherwise it will toggle on and off.
 *  Ripped from Allegro's sources & modified
 */
static int _button_proc(int msg, DIALOG *d, int c)
{
   int state1, state2,text_col;
   int swap,p_he,p_w,p_h;

   switch(msg) {

      case MSG_GOTFOCUS:
         stop_sample(datafile[MOVE].dat);
         play_sample(datafile[MOVE].dat,255,128,1000,0);
         break;

      case MSG_END:

         show_mouse(NULL);
         acquire_screen();

         blit(virtual_screen, screen, d->x, d->y, d->x, d->y,
              text_length(datafile[BIG_FONT].dat, d->dp),
              text_height(datafile[BIG_FONT].dat));

         if (d->flags & D_GOTFOCUS) {
            p_w = ((RLE_SPRITE *)datafile[POINTER].dat)->w;
            p_h = ((RLE_SPRITE *)datafile[POINTER].dat)->h;
            p_he = (d->h/2) -(p_h /2);

            blit(virtual_screen, screen, -p_w+d->x-p_w, d->y+p_he,
               -p_w+d->x-p_w, d->y+p_he, p_w, p_h);
            blit(virtual_screen, screen, d->x+actual_dlg_width+p_w, d->y+p_he,
               d->x+actual_dlg_width+p_w, d->y+p_he, p_w,p_h);
         }
         release_screen();
         show_mouse(screen);
         break;

      case MSG_DRAW:

         text_col = 8;
         if (d->flags & D_SELECTED) {
            text_col = 1;
         }

         text_mode(-1);

         p_w = ((RLE_SPRITE *)datafile[POINTER].dat)->w;
         p_h = ((RLE_SPRITE *)datafile[POINTER].dat)->h;
         p_he = (d->h/2) -(p_h /2);


         acquire_screen();
         outline_textout(screen, datafile[BIG_FONT].dat, d->dp, d->x, d->y,
            8, black_color);

         if (d->flags & D_GOTFOCUS) {
            draw_rle_sprite(screen, datafile[POINTER].dat,
               -p_w+d->x-p_w, d->y+p_he);
            draw_rle_sprite(screen, datafile[POINTER].dat,
               d->x+actual_dlg_width+p_w, d->y+p_he);
         }
         else {
            blit(virtual_screen, screen, -p_w+d->x-p_w, d->y+p_he,
               -p_w+d->x-p_w, d->y+p_he, p_w, p_h);
            blit(virtual_screen, screen, d->x+actual_dlg_width+p_w, d->y+p_he,
               d->x+actual_dlg_width+p_w, d->y+p_he, p_w, p_h);
         }
         release_screen();

         break;

      case MSG_WANTFOCUS:
          return D_WANTFOCUS;

      case MSG_KEY:
          /* close dialog? */
          if (d->flags & D_EXIT) {
             return D_CLOSE;
          }
      
          /* or just toggle */
          d->flags ^= D_SELECTED;
          
          scare_mouse();
          SEND_MESSAGE(d, MSG_DRAW, 0);
          unscare_mouse();
          break;
      
      case MSG_CLICK:
          /* what state was the button originally in? */
          state1 = d->flags & D_SELECTED;
          if (d->flags & D_EXIT)
             swap = FALSE;
          else
             swap = state1;
      
          /* track the mouse until it is released */
          while (gui_mouse_b()) {
             state2 =((gui_mouse_x() >= d->x) && (gui_mouse_y() >= d->y) &&
                 (gui_mouse_x() < d->x + d->w) && (gui_mouse_y() < d->y + d->h));
             if (swap)
                state2 = !state2;
      
             /* redraw? */
             if (((state1) &&(!state2)) ||((state2) &&(!state1))) {
                d->flags ^= D_SELECTED;
                state1 = d->flags & D_SELECTED;
                scare_mouse();
                SEND_MESSAGE(d, MSG_DRAW, 0);
                unscare_mouse();
             }
      
             /* let other objects continue to animate */
             broadcast_dialog_message(MSG_IDLE, 0);
          }
      
          /* should we close the dialog? */
          if ((d->flags & D_SELECTED) &&(d->flags & D_EXIT)) {
             d->flags ^= D_SELECTED;
             return D_CLOSE;
          }
          break; 
   }

   return D_O_K;
   c=c;
}


void menu(void)
{
   int ret,old_ret;

   ret = 0; old_ret = -1;

   shutdown_dialog(player);

   /* Ok, so we start the dialog thing. */
   while (ret != game_dlg_exit) {
      _correct_menu_dialog(game_dlg);
      hot_keys();    /* Ugly fix to update the music */
      ret = do_dialog(game_dlg,old_ret);

      switch (ret) {
         case 0:  /* User selected one player game */
            one_player();
            clear_keybuf();
            draw_menu_back_in_buffer();
            break;

         case 1:  /* User selected two player game */
            two_player();
            clear_keybuf();
            draw_menu_back_in_buffer();
            break;

         case 2:  /* User selected submenu. Prepare it and run it... */

            ret = old_ret = 0;
            while (ret != options_dlg_exit) {

               _correct_menu_dialog(options_dlg);
               hot_keys();    /* Ugly fix to update the music */
               ret = do_dialog(options_dlg, old_ret);

               switch(ret) {

                  case 0:  /* Enters the sound options... */

                     _correct_menu_dialog(sound_dlg);
                     _update_dialog_volume();
         
                     ret = old_ret = 0;
                     while (ret != sound_dlg_exit) {
         
                        _correct_menu_dialog(sound_dlg);

                        drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

                        hot_keys();    /* Ugly fix to update the music */
                        ret = do_dialog(sound_dlg, old_ret);
         
                        switch(ret) {

                           case 2:
                              if (which_music<8)
                                 music(which_music+1);
                              else
                                 music(1);
                              draw_menu_back_in_buffer();
                              break;
         
                           case -1:
                              ret = sound_dlg_exit;
                              break;
                           default:
                              break;
                        }
                        old_ret = ret;
         
                     }
         
                     /* Restore the previous menu data */
                     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
                     draw_menu_back_in_buffer();
                     clear_keybuf();
         
                     ret = old_ret = 0;
                     break;

                  case 1: /* Sets the death mode on/off */
                     if (death_match) death_match = 0;
                     else death_match = 1;
                     _correct_menu_dialog(options_dlg);
                     break;

                  case 2: /* Sets the number of death lines */
                     if (death_match) {
                        death_lines++;
                        if (death_lines >9)
                           death_lines = 1;

                        _correct_menu_dialog(options_dlg);
                     }
                     break;

                  case 3: /* Sets the blocks visible/hidden */
                     visible_blocks = !visible_blocks;
                     _correct_menu_dialog(options_dlg);
                     break;

                  case 4: /* Redefine keys */

                     redefine_keys();
                     clear_keybuf();
                     break;

                  case 5: /* Selects the language */

                     show_mouse(0);

                     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
                     select_language();
                
                     /* Restore the previous menu data */
                     show_mouse(0);
                     set_mouse_sprite(0);
                     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
                     draw_menu_back_in_buffer();
                     show_mouse(screen);
                     clear_keybuf();
                     break;

                  case -1:
                     ret = options_dlg_exit;
                     break;
                  default:
                     break;
               }
               old_ret = ret;

            }

            clear_keybuf();

            ret = old_ret = 2;
            break;

         case 3:  /* User wants to know the high scores */
            show_mouse(NULL);
            print_high_scores(0);
            clear_keybuf();

            do {
               hot_keys();
               yield_timeslice();
            } while (!any_input_used());
            while (mouse_b)
               yield_timeslice();  /* Wait for quick mouse buttons */

            clear_keybuf();
            draw_menu_back_in_buffer();
            show_mouse(NULL);
            break;

         case 4:
         case -1:
            _save_config();
            save_scores();
            outro();
            ret = game_dlg_exit;
            break;

         default:
            break;
      }
      old_ret = ret;
   }

   fade_out(10);
};

/* Inside the menu the only chance of detecting one hot key is to call this
 * function, which actually calls the real hot_keys function which detects
 * the actual keypress and reacts to it.
 */

static int _menu_hot_key()
{
   hot_keys();

   show_mouse(screen);


   return D_REDRAW;
}

/* This functions pre-draws the menu on the screen. Called from main();
 * It's just a sloppy trick. It probably won't work if my objects were doing
 * special things at startup/shutdown like allocating memory or displaying
 * different graphical states.
 */
void prepare_menu_graphically(void)
{
   draw_menu_back_in_buffer();
   _correct_menu_dialog(game_dlg);

   show_mouse(screen);

   player = init_dialog(game_dlg, -1);
   
   update_dialog(player);
}

static void _save_config(void)
{
   set_config_int(STR_KEYS,"1_UP", TECLA_1_UP);
   set_config_int(STR_KEYS,"1_DOWN", TECLA_1_DOWN);
   set_config_int(STR_KEYS,"1_LEFT", TECLA_1_LEFT);
   set_config_int(STR_KEYS,"1_RIGHT", TECLA_1_RIGHT);
   set_config_int(STR_KEYS,"2_UP", TECLA_2_UP);
   set_config_int(STR_KEYS,"2_DOWN", TECLA_2_DOWN);
   set_config_int(STR_KEYS,"2_LEFT", TECLA_2_LEFT);
   set_config_int(STR_KEYS,"2_RIGHT", TECLA_2_RIGHT);
   set_config_int(STR_SOUND,"VOL_FX",vol_fx);
   set_config_int(STR_SOUND,"VOL_MUSIC",vol_midi);
   set_config_int(STR_GAME,"DEATH_MATCH",death_match);
   set_config_int(STR_GAME,"DEATH_LINES",death_lines);
   set_config_int(STR_GAME,"VISIBLE_BLOCKS",visible_blocks);
}

