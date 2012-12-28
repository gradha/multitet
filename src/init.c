#ifdef MSS
#include <mss.h>
#endif
#include "init.h"
#include "data.h"
#include "multitet.h"
#include "gamecore.h"
#include "score.h"
#include "misc.h"
#include "error.h"
#include "langsel.h"
#include <allegro.h>

static void _reserve_memory_for_bitmaps(void);
static void _process_blocks(void);
static void _inc_ticks(void);
static void _init_timer(void);
static void _init_scores(void);
static void _free_all_mem(void);
static void _create_pause_screen(void);

static RGB_MAP _back_table;
static COLOR_MAP _back_map;

/* Creates the pause screen with random pixels. */
static void _create_pause_screen(void)
{
   int x,y;
   
   /*We draw the pause screen with random pixels */
   for (y = 0; y < pause_screen->h; y++) {
      for (x = 0; x < pause_screen->w; x++) {
         pause_screen->line[y][x] = rand() % 256;
      }
   }
   
   /* We set now the 16 different grayscale colors we will use. */
   for (y = 0; y < 256; y++)
      pal_pause[y].r = pal_pause[y].g = pal_pause[y].b = rand()%256;
}

void init_allegro_n_other(int argc, char *argv[])
{
   report_progress(get_config_text("Allegro initialized."));
   report_progress("%s %s", get_config_text("Gogosoftware presents: Multitet"), GAME_NAME);
   report_progress("%s %s", get_config_text("This game was compiled with"), allegro_id);

   switch(os_type) {
      case OSTYPE_WIN3:
      case OSTYPE_WIN95:
      case OSTYPE_WIN98:
      case OSTYPE_WINME:
      case OSTYPE_WINNT:
      case OSTYPE_WIN2000:
      case OSTYPE_WINXP:
         report_progress(get_config_text("You are using Windows... urg!"));
         break;
      default:
         break;
   }

   report_progress(get_config_text("Initialization of game timers"));
   _init_timer();
   _init_scores();

   /* Now we will install the sound. We will allow Allegro to autodetect it. He
    * usually does it all right. If there's a mistake, we exit to DOS
    */
   
   if (install_sound(DIGI_NONE, MIDI_NONE, NULL) != 0) {
      da_error_grave("%s %s", get_config_text("Error initialising sound system"), allegro_error);
      exit(1);
   }

   report_progress(get_config_text("...sound and music drivers succesfully installed..."));

   set_volume(vol_fx, vol_midi);
   
   datafile = load_datafile("data.dat");

   if (!datafile) {
      da_error_grave(get_config_text("Error loading file 'DATA.DAT'!"));
      exit(1);
   }

   report_progress(get_config_text("...data file loaded"));
   report_progress(get_config_text("Starting game. Press a key..."));

   /* Create the screen buffers... */
   _reserve_memory_for_bitmaps();

   _process_blocks();

   create_rgb_table(&_back_table, datafile[PAL_MAIN].dat, NULL);
   rgb_map = &_back_table;
   create_trans_table(&_back_map, datafile[PAL_MAIN].dat, 160, 160, 160, NULL);
   color_map = &_back_map;

   black_color = makecol(0, 0, 0);
   white_color = makecol(255, 255, 255);
   red_color = makecol(255, 0, 0);
   menu_color = palette_color[8];

   fade_out(2);
   
   atexit(_free_all_mem);
}

/* This cleans up the graphic grid I used to draw the blocks */
static void _process_blocks(void)
{
   int x,y;

   blit(datafile[BLOCKS].dat,blocks,0,0,0,0,blocks->w,blocks->h);

   for (y = 0; y < blocks->h; y++)
      for (x = 0; x < blocks->w; x++)
         if (blocks->line[y][x] > 7)
            blocks->line[y][x] = 0;
}

static void _free_all_mem(void)
{
   destroy_bitmap(blocks);
   destroy_bitmap(pause_screen);
   destroy_bitmap(virtual_screen);
   destroy_bitmap(background);

   unload_datafile(datafile);
}

static void _reserve_memory_for_bitmaps(void)
{
   background = create_bitmap(640,480);
   virtual_screen = create_bitmap(640,480);
   pause_screen = create_bitmap(640,480);

   blocks = create_bitmap(((BITMAP*) datafile[BLOCKS].dat)->w,
      ((BITMAP*) datafile[BLOCKS].dat)->h);

   if (!background || !virtual_screen || !pause_screen || !blocks) {
      da_error_grave(get_config_text("ERROR!!! Couldn't reserve enough memory for the game!"));
      exit(1);
   }

   stretch_blit(datafile[BMP_MENU].dat, background, 0, 0,
      ((BITMAP*)datafile[BMP_MENU].dat)->w-1,
      ((BITMAP*)datafile[BMP_MENU].dat)->h-1,
      0, 0, background->w, background->h);
   _create_pause_screen();
}


/* Timer routine. Have to lock it in main before using it. */
static void _inc_ticks(void)
{
   tick_counter++;
}
END_OF_FUNCTION(_inc_ticks);


static void _init_timer(void)
{
/* The following is used to set up the timer we will use in the game. It will
 * help us to control the speed at which the game will run. Thanks to this,
 * future users in the year 3000 will be able to play it without going crazy
 * trying to 'see' the individual frames */

   LOCK_VARIABLE(tick_counter);
   LOCK_FUNCTION(_inc_ticks);

   install_int_ex(_inc_ticks,BPS_TO_TIMER(SECOND));
   /* From now on, one second will be 30 'ticks' for us. */
}


/* Looks for a config file. Restores default setting if unsuccesful */
void init_config(void)
{
   /* NO VALEN LOS PREDEFINIDOS */

   TECLA_1_UP     = get_config_int(STR_KEYS, "1_UP", KEY_UP);
   TECLA_1_DOWN   = get_config_int(STR_KEYS, "1_DOWN", KEY_DOWN);
   TECLA_1_LEFT   = get_config_int(STR_KEYS, "1_LEFT", KEY_LEFT);
   TECLA_1_RIGHT  = get_config_int(STR_KEYS, "1_RIGHT", KEY_RIGHT);
   TECLA_2_UP     = get_config_int(STR_KEYS, "2_UP", KEY_W);
   TECLA_2_DOWN   = get_config_int(STR_KEYS, "2_DOWN", KEY_S);
   TECLA_2_LEFT   = get_config_int(STR_KEYS, "2_LEFT", KEY_A);
   TECLA_2_RIGHT  = get_config_int(STR_KEYS, "2_RIGHT", KEY_D);
   TECLA_MUSIC    = get_config_int(STR_KEYS, "CHANGE_MUSIC", KEY_M);
   TECLA_PAUSE    = get_config_int(STR_KEYS, "PAUSE",KEY_P);
   vol_fx         = MID(0, 255, get_config_int(STR_SOUND, "VOL_FX", 255));
   vol_midi       = MID(0, 255, get_config_int(STR_SOUND, "VOL_MUSIC", 225));
   death_match    = MID(0, 1, get_config_int(STR_GAME, "DEATH_MATCH", 1));
   death_lines    = MID(1, 9, get_config_int(STR_GAME, "DEATH_LINES", 4));
   visible_blocks = MID(0, 1, get_config_int(STR_GAME, "VISIBLE_BLOCKS", 1));

   if (!(*get_config_string("system", "language", "")))
      select_language();
}

/* Looks for a scores table file. Restores default setting if unsuccesful */
static void _init_scores(void)
{
   PACKFILE *file = pack_fopen("scores.cfg", "rb");
   
   if (file) {
      /* Now we read the scores. */
      pack_fread(&high_scores, sizeof(high_scores), file);
      pack_fclose(file);
   }
   else {
      int f;
      /*If there's no scores file, we introduce default scores. */
      for (f = 9; f >= 0; f--)
         high_scores[f].level = 10-f;
      high_scores[9].lines=10;
      for (f = 8; f >=0; f--)
         high_scores[f].lines=high_scores[f+1].lines+10;
      for (f = 9;f >=0; f--)
         high_scores[f].score = high_scores[f].lines*10+high_scores[f].level*1000;
   
      usprintf(high_scores[0].name, "Grzegorz Adam Hankiewicz......");
      usprintf(high_scores[1].name, "Aritz Albaizar................");
      usprintf(high_scores[2].name, "DJ Delorie....................");
      usprintf(high_scores[3].name, "Shawn Hargreaves..............");
      usprintf(high_scores[4].name, "Rober Hoehne..................");
      usprintf(high_scores[5].name, "Jagoba Sainz de Baranda.......");
      usprintf(high_scores[6].name, "Karolina Hankiewicz...........");
      usprintf(high_scores[7].name, "Wieslaw Hankiewicz............");
      usprintf(high_scores[8].name, "Enrique Cuenca Amigo..........");
      usprintf(high_scores[9].name, "Ion Toledo....................");

      report_progress("...couldn't read high scores file. Restoring default scores...");
      report_progress("...no se pudo leer la tabla de records. Recuperando valores por defecto...");
   }
}


