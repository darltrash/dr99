#include "renderer.h"
#include "audio.h"
#include "ui.h"

int main() {
    if (rn_init() > 0)
        return 1;

    if (au_init() > 0)
        return 1;

    mu_Context *mu_ctx = rn_ui_init();

    while (1) {
        rn_loop();

        ui_present(mu_ctx);
        rn_ui_draw();
    }
    return 0;
}