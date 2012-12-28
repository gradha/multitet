#ifndef SCORE_H
#define SCORE_H

/*******************
 ***** score.c *****
 *******************/

/*****
 * Globals
 *****/

/* This is the high scores table. We will use a multidimensional struct of
 * characters. We will store 30 for the name, 2 for the level, 6 for
 * the score and 3 for the lines. We will keep up to 10 players. Remember we
 * HAVE to add 1 to the char buffers because of the "end-string" character.
 */

struct {
   char name[31];
   int level;
   int score;
   int lines;
   } high_scores[10];

#define NEXT_SCORE_BUF_SIZE      45
#define LINES_BUF_SIZE           60
#define TOTAL_LINES_BUF_SIZE     65
#define LEVEL_BUF_SIZE           40
#define SCORE_BUF_SIZE           40

extern int level[2],lines[2],score[2],next_score[2],total_lines[2];

extern char score_buf[2][SCORE_BUF_SIZE], lines_buf[2][LINES_BUF_SIZE],
   level_buf[2][LEVEL_BUF_SIZE], next_score_buf[2][NEXT_SCORE_BUF_SIZE],
   total_lines_buf[2][TOTAL_LINES_BUF_SIZE];

/*****
 * Prototypes
 *****/

void print_high_scores(int number);
void got_score(int player);
int save_scores(void);

#endif
