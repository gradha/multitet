#ifdef MSS
#include <mss.h>
#endif
#include "score.h"
#include "multitet.h"
#include "data.h"
#include "misc.h"
#include <allegro.h>

/* TODO: Error viendo next high score jugando solo con records recién iniciados */

int level[2],lines[2],score[2],next_score[2],total_lines[2];
char score_buf[2][SCORE_BUF_SIZE], lines_buf[2][LINES_BUF_SIZE],
   level_buf[2][LEVEL_BUF_SIZE], next_score_buf[2][NEXT_SCORE_BUF_SIZE],
   total_lines_buf[2][TOTAL_LINES_BUF_SIZE];

void print_high_scores(int number)
{
   int f;
   const char *player_text[2];
   player_text[0] = get_config_text("Player 1");
   player_text[1] = get_config_text("Player 2");
   rectfill(virtual_screen, 55, 106, 585, 367, black_color);

   outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat,
      get_config_text("Hall of fame"), 320, 110, menu_color, black_color);
   outline_textout(virtual_screen, font, get_config_text("NAME"), 135, 145, menu_color, black_color);
   outline_textout(virtual_screen, font, get_config_text("SCORE"), 415, 145, menu_color, black_color);
   outline_textout(virtual_screen, font, get_config_text("LEVEL"), 480, 145, menu_color, black_color);
   outline_textout(virtual_screen, font, get_config_text("LINES"), 535, 145, menu_color, black_color);

   for (f = 0; f < 10; f++) {
      uszprintf(lines_buf[0],  LINES_BUF_SIZE,  "%d.",  f+1);
      outline_textout(virtual_screen, font, lines_buf[0], 70, 170+(f*20), red_color, black_color);
      outline_textout(virtual_screen, font, high_scores[f].name, 120, 170+(f*20), red_color, black_color);
      uszprintf(lines_buf[0],  LINES_BUF_SIZE,  "%d",  high_scores[f].lines);
      uszprintf(level_buf[0],  LEVEL_BUF_SIZE,  "%d",  high_scores[f].level);
      uszprintf(score_buf[0],  SCORE_BUF_SIZE,  "%d",  high_scores[f].score);
      outline_textout(virtual_screen, font, score_buf[0], 415, 170+(f*20), red_color, black_color);
      outline_textout(virtual_screen, font, level_buf[0], 490, 170+(f*20), red_color, black_color);
      outline_textout(virtual_screen, font, lines_buf[0], 540, 170+(f*20), red_color, black_color);
   }

   ASSERT(number >= 0 && number <= 3);
   if (number == 1 || number == 2) {
      rectfill(virtual_screen, 55, 381, 585, 405, black_color);
      rectfill(virtual_screen, 55, 381, 585, 405, black_color);
      outline_textout(virtual_screen, font, player_text[number-1], 70, 390, red_color, black_color);
      uszprintf(score_buf[number-1], SCORE_BUF_SIZE, "%d",  score[number-1]);
      uszprintf(level_buf[number-1], LEVEL_BUF_SIZE, "%d",  level[number-1]);
      uszprintf(lines_buf[number-1], LINES_BUF_SIZE, "%d",  total_lines[number-1]);
      outline_textout(virtual_screen, font, score_buf[number-1], 415, 390, red_color, black_color);
      outline_textout(virtual_screen, font, level_buf[number-1], 490, 390, red_color, black_color);
      outline_textout(virtual_screen, font, lines_buf[number-1], 540, 390, red_color, black_color);
   } else if (number == 3) {
      int height[2] = { 390, 410 };

      rectfill(virtual_screen, 55, 381, 585, 405, black_color);
      rectfill(virtual_screen, 55, 381, 585, 405, black_color);
      if (score[1] >= score[0]) {
         height[0] = 410;
         height[1] = 390;
      }

      for (f = 0; f < 2; f++) {
         outline_textout(virtual_screen, font, player_text[f], 70, height[f],
            red_color, black_color);
         uszprintf(score_buf[f], SCORE_BUF_SIZE, "%d", score[f]);
         uszprintf(level_buf[f], LEVEL_BUF_SIZE, "%d", level[f]);
         uszprintf(lines_buf[f], LINES_BUF_SIZE, "%d", total_lines[f]);
         outline_textout(virtual_screen, font, score_buf[f], 415, height[f],
            red_color, black_color);
         outline_textout(virtual_screen, font, level_buf[f], 490, height[f],
            red_color, black_color);
         outline_textout(virtual_screen, font, lines_buf[f], 540, height[f],
            red_color, black_color);
      }
   }

   acquire_screen();
   vsync();
   blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
   release_screen();
}

/* A player got a high score. We read the keyboard input and modify the high
 * scores table.
 */
void got_score(int player)
{
   char txt[33];
   int ch,  l, sco, lev, lin;

   if (player==1) {
      sco=score[0];
      lev=level[0];
      lin=total_lines[0];
   } else {
      sco=score[1];
      lev=level[1];
      lin=total_lines[1];
   }

   /*We clear the txt buffer and put all '.' */
   for (l=0;l<33;l++) txt[l]='\0';
   for (l=0;l<30;l++) txt[l]='.';
   txt[0]='-';
   l=0;

   blit(background, virtual_screen, 0, 0, 0, 0, 640, 480);

   if (player==1)outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("Player 1"), 320, 160, menu_color, black_color);
   if (player==2)outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, get_config_text("Player 2"), 320, 160, menu_color, black_color);

   outline_textout_centre(virtual_screen, datafile[BIG_FONT].dat, txt, 320, 220, menu_color, black_color);
   clear_keybuf();
   acquire_screen();
   blit(virtual_screen, screen, 0, 0, 0, 0, 640, 480);
   release_screen();

   while (any_input_used());

   while (1==1) {
      if (keypressed()) {
         ch=readkey() & 0x00ff;
         if ((ch>31)&&(ch<127) && l<30) {
            txt[l+1]='-';
            txt[l]=ch;
            l++;
            play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
         } else if ((key[KEY_BACKSPACE]) && l > 0 ) {
            l--;
            txt[l+1]='.';
            txt[l]='-';
            play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
         }
         else {
            if (key[KEY_ENTER] || key[KEY_ESC])
               break;
            else {
               stop_sample(datafile[UNA_MAS].dat);
               play_sample(datafile[UNA_MAS].dat, 255, 128, 1000, 0);
            }
         }
   
         blit(background, virtual_screen, 0, 220, 0, 220, 640, 40);
         outline_textout_centre(virtual_screen,  datafile[BIG_FONT].dat, 
            txt, 320, 220, menu_color, black_color);
         acquire_screen();
         blit(virtual_screen, screen, 0, 220, 0, 220, 640, 40);
         release_screen();
      }
   }
   play_sample(datafile[MOVE].dat, 255, 128, 1000, 0);
   txt[l]='.';
   
   for (l = 8;; l--) {
      high_scores[l+1].score = high_scores[l].score;
      high_scores[l+1].level = high_scores[l].level;
      high_scores[l+1].lines = high_scores[l].lines;
      strcpy(high_scores[l+1].name, high_scores[l].name);
   
      if (sco < high_scores[l].score) {
         ustrzcpy(high_scores[l+1].name, 30, txt);
         high_scores[l+1].score = sco;
         high_scores[l+1].level = lev;
         high_scores[l+1].lines = lin;
         break;
      }
      if (l == 0) {
         ustrzcpy(high_scores[0].name, 30, txt);
         high_scores[0].score = sco;
         high_scores[0].level = lev;
         high_scores[0].lines = lin;
         break;
      }
   }

   while (any_input_used())
      yield_timeslice();
}

int save_scores(void)
{
   PACKFILE *file = pack_fopen("scores.cfg", "wb");

   if (file) {
      pack_fwrite(&high_scores, sizeof(high_scores), file);
      pack_fclose(file);
      return 0;
   }
   return 1;
}
