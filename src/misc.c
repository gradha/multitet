#ifdef MSS
#include <mss.h>
#endif
#include "misc.h"
#include "multitet.h"
#include "gamecore.h"
#include "data.h"
#include "gui.h"
#include "error.h"
#include <allegro.h>

#define _CHUNK    255

static char *_remove_backslashed_spaces(char *txt);
static void _capture_screen(void);
static void _show_help(void);
static void _switch_music(int p);

static char _temp[1024];
int black_color, white_color, menu_color, red_color, prog_y = 10;
int vol_fx, vol_midi, which_music = 0;

void report_progress(const char *txt, ...)
{
   va_list args;
   ASSERT(txt);

   va_start(args, txt);
   uvszprintf(_temp, sizeof(_temp), txt, args);
   va_end(args);

   printf("multitet:%s\n", _temp);
   outline_textout(virtual_screen, font, _temp, 10, prog_y,
		makecol(0,0,0), makecol(255,255,255));
   stretch_virtual_screen();
   prog_y += text_height(font);
}

/* This function is called to check if the player pressed one of the 'hot
 * keys' while playing or using the menus
 */
void hot_keys(void)
{
   if (key[KEY_F1])
      _show_help();
   if (key[KEY_F12] || key[KEY_F10]) {
      _capture_screen();
      while (key[KEY_F12] || key[KEY_F10])
         yield_timeslice();
   }

   if (midi_pos<0 || key[TECLA_MUSIC])
      music(0);

   if (key[KEY_F5] && active_dialog != sound_dlg) {
      vol_fx--;
      vol_fx = MAX(0, vol_fx);
      set_volume(vol_fx,vol_midi);
      stop_sample(datafile[MOVE].dat);
      play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
      clear_keybuf();
   }
   if (key[KEY_F6] && active_dialog != sound_dlg) {
      vol_fx++;
      vol_fx = MIN(vol_fx, 255);
      set_volume(vol_fx,vol_midi);
      stop_sample(datafile[MOVE].dat);
      play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
      clear_keybuf();
   }
   if (key[KEY_F7] && active_dialog != sound_dlg) {
      vol_midi--;
      vol_midi = MAX(0, vol_midi);
      set_volume(vol_fx,vol_midi);
      clear_keybuf();
   }
   if (key[KEY_F8] && active_dialog != sound_dlg) {
      vol_midi++;
      vol_midi = MIN(255, vol_midi);
      set_volume(vol_fx,vol_midi);
      clear_keybuf();
   }
}

/* This makes a delay, watching the keyboard if needed for a keypress.
 * If we call the function giving BOTH parameters a negative number, then the
 * function will wait for a keypress endlessly
 */
void multitet_wait(int wait_time)
{
   int old_counter;
   
   if (wait_time < 0) {
      while (any_input_used())
         yield_timeslice();
      while (!any_input_used())
         yield_timeslice();
   } else {
      old_counter = tick_counter + wait_time;
      while (!any_input_used() && old_counter > tick_counter)
         yield_timeslice();
   }
}

/* Checks for input of the keyboard or mouse and returns a value
 * if any was used(button pressed). Returns 1 for keyboard or 2 for mouse
 */

int any_input_used(void)
{
   if (keypressed()) {
      clear_keybuf();
      return 1;
   }

   if (mouse_b) {
      while (mouse_b);
      return 2;
   }

   return 0;
}


/* Looks for existent captured screens files. Then saves the actual screen
 * to a pcx file.
 */
static void _capture_screen(void)
{
   int d;
   char name[30];
   const int limit = 1000;

   for (d = 0; d < limit; d++) {
      uszprintf(name, sizeof(name), "multitet-capture-%02d.bmp",d);
      if (!exists(name)) {
         save_bmp(name, virtual_screen, datafile[PAL_MAIN].dat);
         play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
         d += limit;
      }
   }
}

/* Switches the music to the specified number and prints the correct text to
 * the char buffers.
 */

static void _switch_music(int p)
{
/* sprintf(buffer_music_num, "%s %d", get_config_text("MUSIC"), p); */
/*   sprintf(buffer_music_title,"%s %s",gametexts[20].buffer,gametexts[20+p].buffer);
*/
   play_midi(datafile[MUSIC1-1+p].dat, 0);
}

/* If we call this routine with a zero value, it will choose one music at ran-
 * dom. Else will set the specified music.
 */
void music(int p)
{
   int ran = rand()%8;

   if (!p) {
      while (which_music-1 == ran)
         ran = rand()%8;
   
      which_music = ran+1;
      _switch_music(which_music);
   }
   else {
      which_music = p;
      _switch_music(which_music);
/*
      if (p>0 && which_music<8) {
         which_music++;
         _switch_music(which_music);
         play_sample(datafile[MOVE].dat,255,128,1000,0);
      }
      if (p<0 && which_music>1){
         which_music--;
         _switch_music(which_music);
         play_sample(datafile[MOVE].dat,255,128,1000,0);
      }
*/
   }
}

/* Stores the timer values, just in case this function was called when playing,
 * and shows the quick help text.
 */
static void _show_help(void)
{
   char **lines;
   int timer_difference_1,timer_difference_2;
   int black_color = makecol(0, 0, 0);
   
   timer_difference_1 = tick_counter-count_old[0];
   timer_difference_2 = tick_counter-count_old[1];

   drawing_mode(DRAW_MODE_TRANS,NULL,0,0);

   show_mouse(NULL);

   BITMAP *temp = create_bitmap(TSCREEN_W, TSCREEN_H);
   blit(virtual_screen, temp, 0, 0, 0, 0, TSCREEN_W, TSCREEN_H);
   rectfill(temp, 0, 0, TSCREEN_W-1, TSCREEN_H-1, makecol(0,0,0));
   
   outline_textout_centre(temp, datafile[BIG_FONT].dat,
      get_config_text("QUICK HELP SCREEN"), 320, 10,
      palette_color[21], black_color);

   lines = get_multiple_config_text("quick_help");
   if (!lines)
      outline_textout_centre(temp, font,
         get_config_text("Internal error reading multiple lines"),
         TSCREEN_W/2, TSCREEN_H/2, white_color, black_color);
   else {
      int f;
      for (f = 0; lines[f]; f++) {
         outline_textout(temp, font, lines[f], 0, 60+f*10,
            palette_color[8], black_color);
         free(lines[f]);
      }
      free(lines);
   }
   outline_textout_centre(temp, datafile[BIG_FONT].dat,
      get_config_text("PRESS ANY KEY TO EXIT FROM THIS SCREEN"),
      320, 450, palette_color[21], black_color);

   acquire_screen();
   stretch_blit(temp, screen, 0, 0,
      temp->w, temp->h, 0, 0, SCREEN_W, SCREEN_H);
   release_screen();
   destroy_bitmap(temp);
   clear_keybuf();

   while (!keypressed()){
      if (midi_pos<0 || key[TECLA_MUSIC])
         music(0);
   }
   clear_keybuf();

   if (active_dialog == sound_dlg)
      drawing_mode(DRAW_MODE_SOLID,NULL,0,0);

   stretch_virtual_screen();
/*   show_mouse(screen); */
   count_old[0] = tick_counter-timer_difference_1;
   count_old[1] = tick_counter-timer_difference_2;
}

/* Saves timer values, shows the pause screen and rotates the pallete */
void pausa(void)
{
   int c,sample_counter,timer_difference_1,timer_difference_2;
   RGB temp;
   
   timer_difference_1 = tick_counter-count_old[0];
   timer_difference_2 = tick_counter-count_old[1];
   
   set_pallete(black_pallete);
   acquire_screen();
   stretch_blit(pause_screen, screen, 0, 0,
      pause_screen->w, pause_screen->h, 0, 0, SCREEN_W, SCREEN_H);
   release_screen();
   while (key[TECLA_PAUSE])
      yield_timeslice();
   
   play_sample(datafile[RUIDO].dat,200,128,1000,1000);
   sample_counter = tick_counter;

   while (!key[TECLA_PAUSE] && !key[KEY_ESC]) {
      temp=pal_pause[255];

      for (c=255;c>0;c--)
         pal_pause[c]=pal_pause[c-1];

      pal_pause[0]=temp;
      set_pallete(pal_pause);
      if (sample_counter+3000<tick_counter){
         stop_sample(datafile[RUIDO].dat);
         adjust_sample(datafile[RUIDO].dat,200,128,1000,1000);
      }
      yield_timeslice();
   }

   stop_sample(datafile[RUIDO].dat);
   set_pallete(black_pallete);
   stretch_virtual_screen();
   while (key[TECLA_PAUSE] || key[KEY_ESC])
      yield_timeslice();
   
   count_old[0] = tick_counter-timer_difference_1;
   count_old[1] = tick_counter-timer_difference_2;
   set_pallete(datafile[PAL_MAIN].dat);
}


/* Outlined textout functions. Just like textout but draw a border
 * around the text using 4 extra textout functions. Since the textout could
 * be done on the screen, which is slow, we create first a memory bitmap
 * which acts as a double buffer.
 */

void outline_textout(BITMAP *bmp, FONT *f, const char *s, int x, int y, int color, int border_color)
{
   BITMAP *tempstring;
   int xo,yo;

   tempstring = create_bitmap(text_length(f,s)+2,text_height(f)+2);
   if (!tempstring)
      return;   /* AAWWOOOOOGGGAAA!! Not enough memory */

   clear_bitmap(tempstring);

   xo = 1; yo=0;
   textout(tempstring, f, s, xo,yo, border_color );
   yo+=2;
   textout(tempstring, f, s, xo,yo, border_color );
   yo--;xo--;
   textout(tempstring, f, s, xo,yo, border_color );
   xo+=2;
   textout(tempstring, f, s, xo,yo, border_color );
   xo--;
   textout(tempstring, f, s, xo,yo, color );

   if (is_screen_bitmap(bmp)) acquire_screen();
   draw_sprite(bmp, tempstring, x-1, y-1);
   if (is_screen_bitmap(bmp)) release_screen();

   destroy_bitmap(tempstring);
}

void outline_textout_centre(BITMAP *bmp, FONT *f, const char *s, int x, int y, int color, int border_color)
{
   BITMAP *tempstring;
   int xo,yo;

   tempstring = create_bitmap(text_length(f,s)+2,text_height(f)+2);
   if (!tempstring)
      return;   /* AAWWOOOOOGGGAAA!! Not enough memory */

   clear_bitmap(tempstring);

   xo = 1; yo=0;
   textout(tempstring, f, s, xo,yo, border_color );
   yo+=2;
   textout(tempstring, f, s, xo,yo, border_color );
   yo--;xo--;
   textout(tempstring, f, s, xo,yo, border_color );
   xo+=2;
   textout(tempstring, f, s, xo,yo, border_color );
   xo--;
   textout(tempstring, f, s, xo,yo, color );

   draw_sprite(bmp, tempstring,(x-(tempstring->w>>1))-1, y-1);
   destroy_bitmap(tempstring);
}

char **get_multiple_config_text(const char *mask)
{
   char *buffer, *number;
   const char *readline;
   char **lines = 0;
   int size = 0;

   ASSERT(mask);

   buffer = malloc(strlen(mask) + 3);
   if (!buffer)
      return 0;
      
   strcpy(buffer, mask);
   number = ustrchr(buffer, 0);

   lines = realloc(lines, sizeof(char*) *(size + 1));
   if (!lines) {
      free(buffer);
      return 0;
   }
   lines[size] = 0;

   usprintf(number, "%02d", size);
   while (size < 99 &&(readline = get_existant_config_text(buffer))) {
      char **aux = realloc(lines, sizeof(char*) *(size + 2));
      if (!aux) {
         size = 0;
         goto our_exit;
      }

      lines = aux;
      lines[size++] = _remove_backslashed_spaces(m_strdup(readline));
      lines[size] = 0;
      usprintf(number, "%02d", size);
   }

   our_exit:
   free(buffer);

   if (!size) {
      free(lines);
      return 0;
   }

   return lines;
}

static char *_remove_backslashed_spaces(char *txt)
{
   int f;
   ASSERT(txt);

   f = 0;
   while (f < ustrlen(txt)) {
      if (ugetat(txt, f) == '\\') {
         int val = ugetat(txt, f+1);
         if (val == ' ' || !val) {
            uremove(txt, f);
            continue;
         }
      }
      f++;
   }

   return txt;
}

const char *get_existant_config_text(const char *txt)
{
   const char *read;
   ASSERT(txt);
   read = get_config_text(txt);

   if (strcmp(read, txt))
      return read;
   return 0;
}

/* Revisar de aquí para abajo quitando innecesarias. */


/* m_xmalloc:
 * Returns the requested chunk of memory. If there's not enough
 * memory, the program will abort.
 */
void *m_xmalloc(size_t size)
{
   void *p = malloc(size);
   if (!p) m_abort(1);
   return p;
}



/* m_xrealloc:
 * Wrapper around real realloc call. Returns the new chunk of memory or
 * aborts execution if it couldn't realloc it.
 */
void *m_xrealloc(void *ptr, size_t new_size)
{
   if (!ptr)
      return m_xmalloc(new_size);
   ptr = realloc(ptr, new_size);
   if (!ptr) m_abort(1);
   return ptr;
}



/* m_abort:
 * Aborts execution with a hopefully meaningful message. If code is less
 * than 1, an undefined exit will happen. Available error codes:
 * 1: insufficient memory
 */
void m_abort(int code)
{
   switch(code) {
      case 1:  da_error_grave("Aborting due to insuficcient memory\n"); break;
      default: da_error_grave("An undefined error caused abnormal termination\n");
   }
}



/* m_strdup:
 * Safe wrapper around strdup, always returns the duplicated string.
 */
char *m_strdup(const char *text)
{
   char *p = m_xmalloc(strlen(text)+1);
   return strcpy(p, text);
}

// vim:tabstop=3 shiftwidth=3 softtabstop=3 expandtab
