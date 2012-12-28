#ifndef MISC_H
#define MISC_H

#include <allegro.h>

/******************
 ***** misc.h *****
 ******************/
 
/*****
 * Prototypes
 *****/

/* The values for the sound. */
extern int vol_fx, vol_midi, which_music;
extern int black_color, white_color, menu_color, red_color, prog_y;

void multitet_wait(int wait_time);
void hot_keys(void);
void report_progress (const char *txt, ...);
int any_input_used(void);
void music(int p);
void pausa(void);
void outline_textout(BITMAP *bmp, FONT *f, const char *s, int x, int y, int color, int border_color);
void outline_textout_centre(BITMAP *bmp, FONT *f, const char *s, int x, int y, int color, int border_color);
char **get_multiple_config_text(const char *mask);
const char *get_existant_config_text(const char *txt);

void *m_xmalloc(size_t size);
void *m_xrealloc(void *ptr, size_t new_size);
void m_abort(int code);
char *m_strdup(const char *text);



#endif

