#ifdef MSS
#include <mss.h>
#endif
#include "error.h"
#include "misc.h"
#include <allegro.h>



static char _temp[1024];

/**[txh]********************************************************************

  Description: Reports an error message in graphic mode, waits for a
  keypress and aborts execution. In text mode prints message and aborts
  immediately.
  
***************************************************************************/

void da_error_grave(const char *txt, ...)
{
   va_list args;
   ASSERT(txt);
   
   va_start(args, txt);
   uvszprintf(_temp, sizeof(_temp), txt, args);
   va_end(args);

   /* detect if we are in graphic mode */
   if (screen) {
      text_mode(black_color);
      textout(screen, font, _temp, 10, 0, white_color);
      textout(screen, font, get_config_text("Press any key to exit game"),
         10, 20, white_color);
      clear_keybuf();
      readkey();
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
   } else {
      allegro_message("%s", txt);
   }

   allegro_exit();
   abort();
}

