#ifndef GUI_H
#define GUI_H

#include <allegro.h>

extern DIALOG sound_dlg[];

void menu(void);
void prepare_menu_graphically(void);
int virtual_do_dialog(DIALOG *dialog, int focus_obj);

#endif
