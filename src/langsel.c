#ifdef MSS
#include <mss.h>
#endif
#include <allegro.h>

#include "langsel.h"

#include "error.h"
#include "gui.h"
#include "misc.h"
#include "multitet.h"

static char **_available_languages; /* This is because of the rsx version. */
static int _num_available_languages;


static void _retrieve_available_languages(void);
static void _free_available_languages(void);
static char *_get_lang_name_from_file(const char *file_name, char *lang_code);
static char *_lang_listbox_getter(int index, int *list_size);


static DIALOG _select_lang_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key) (flags)  (d1)  (d2)  (dp) */
   { d_list_proc,       220,  230,  200,  16*12,255,  0,    0,    D_EXIT,  0,    0,    _lang_listbox_getter },
   { d_ctext_proc,      320,  180,  300,  16,   255,  0,    0,    0,       0,    0,    "Select your language"},
   { d_ctext_proc,      320,  200,  300,  16,   255,  0,    0,    0,       0,    0,    "and press ENTER" },
   { NULL,              0,    0,    0,    0,    0,    0,    0,    0,       0,    0,    NULL }
};



/* Pops a dialog asking for language. Returns -1 if no object was selected.
 * Else returns zero and writes in string the ID of the selected language.
 */
void select_language(void)
{
   int f = -1;
   const char *lang = "en"; /* default language */
   
   set_palette(desktop_palette);
   clear_bitmap(virtual_screen);
   stretch_virtual_screen();
   _retrieve_available_languages();

   if (!_num_available_languages) {
      //alert("No languages", "to choose from!", "Will use english", "Oh", 0, 'o', 0);
      goto finish;
   }

   while ((f = virtual_do_dialog(_select_lang_dialog, 0)) < 0);
   lang = _available_languages[_select_lang_dialog[0].d1*2];

   finish:
   TRACE("select_language: '%s'\n", lang);
   set_config_string("system", "language", lang);
   reload_config_texts(0);
   text_mode(-1);
   _free_available_languages();

   clear_bitmap(virtual_screen);
   stretch_virtual_screen();
}



static void _free_available_languages(void)
{
   if (_available_languages) {
      int f;
      for (f = 0; f < _num_available_languages; f++) {
         free(_available_languages[f*2]);
         free(_available_languages[f*2+1]);
      }
      free(_available_languages);
   }
   _available_languages = 0;
   _num_available_languages = 0;
}



static void _retrieve_available_languages(void)
{
   int ret;
   struct al_ffblk info;

   _free_available_languages();
   _available_languages = m_xmalloc(sizeof(char*));

   TRACE("Entering retrieve section\n");
   if (!(ret = al_findfirst("??text.cfg", &info, 0))) {
      while (!ret) {
         char lang_code[3];
         char *lang_name = _get_lang_name_from_file(info.name, lang_code);
         TRACE("Found file %s\n", info.name);
         if (lang_name) {
            TRACE("  code %s, name %s\n", lang_code, lang_name);
            _available_languages = m_xrealloc(_available_languages,
               ((_num_available_languages + 1) * 2) * sizeof(char*));
            _available_languages[_num_available_languages*2] = m_strdup(lang_code);
            _available_languages[_num_available_languages*2+1] = lang_name;
            _num_available_languages++;
         }
         ret = al_findnext(&info);
      }
      al_findclose(&info);
   }
}


static char *_get_lang_name_from_file(const char *file_name, char *lang_code)
{
   char *line;
   char buf1[256], buf2[256];

   ASSERT(file_name);
   ASSERT(*file_name);
   ASSERT(lang_code);

   lang_code[0] = file_name[0];
   lang_code[1] = file_name[1];
   lang_code[2] = 0;

   get_executable_name(buf1, sizeof(buf1));
   replace_filename(buf2, buf1, file_name, sizeof(buf2));

   push_config_state();
   set_config_file(buf2);

   line = m_strdup(get_config_string(0, "language_name", ""));
   
   pop_config_state();

   if (*line)
      return line;

   free(line);
   return 0;
}



/* callback function to specify the contents of the language listbox */
static char *_lang_listbox_getter(int index, int *list_size)
{
   TRACE("Calling _lang_listbox_getter %d\n", index);
   if (index < 0) {
      TRACE("Returning number of languages %d\n", _num_available_languages);
      *list_size = _num_available_languages;
      return NULL;
   } else {
      TRACE("Returning language %s\n", _available_languages[index*2]);
      return _available_languages[index*2+1];
   }
}

// vim:tabstop=3 shiftwidth=3 softtabstop=3 expandtab
