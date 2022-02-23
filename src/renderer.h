#include "microui.h"
#include "SDL2/SDL.h"

int rn_init();
void rn_loop();
void rn_deinit();

mu_Context *rn_ui_init();
mu_Rect rn_ui_get_window_rect();
int rn_ui_draw();