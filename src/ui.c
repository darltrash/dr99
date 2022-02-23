#include "renderer.h"
#include "microui.h"
#include "log.h"

int ui_coloured_button(mu_Context *ctx, const char *label, int color, int opt) {
    int res = 0;
    mu_Id id = mu_get_id(ctx, label, strlen(label));
    mu_Rect r = mu_layout_next(ctx);
    mu_update_control(ctx, id, r, 0);

    int highlight = (
                    0.2126*((float)((color >> 24) & 0xFF)/255) + 
                    0.7152*((float)((color >> 16) & 0xFF)/255) + 
                    0.0722*((float)((color >>  8) & 0xFF)/255)
                ) ? 0: 255;

    if (!(opt & MU_OPT_NOFRAME))
        ctx->draw_frame(ctx, r, color);

    if (ctx->mouse_pressed == MU_MOUSE_LEFT && ctx->focus == id) {
        res |= MU_RES_SUBMIT;
    }

    mu_draw_rect(ctx, r, mu_color(highlight, highlight, highlight, 
        ctx->focus == id ? 100 : ctx->hover == id ? 50 : 0
    ));

    mu_Vec2 pos;
    mu_Font font = ctx->style->font;
    int tw = ctx->text_width(font, label, -1);
    mu_push_clip_rect(ctx, r);
        pos.y = r.y + (r.h - ctx->text_height(font)) / 2;

        if (opt & MU_OPT_ALIGNCENTER) {
            pos.x = r.x + (r.w - tw) / 2;
        } else if (opt & MU_OPT_ALIGNRIGHT) {
            pos.x = r.x + r.w - tw - ctx->style->padding;
        } else {
            pos.x = r.x + ctx->style->padding;
        }

        mu_draw_text(ctx, font, label, -1, pos, 
            mu_color(highlight, highlight, highlight, 255)
        );
    mu_pop_clip_rect(ctx);

    return res;
}

int ui_present(mu_Context *ctx) {
    mu_begin(ctx);

    if (mu_begin_window_ex(ctx, "Style Editor", mu_rect(1, 1, 4, 4), MU_OPT_NOTITLE | MU_OPT_NORESIZE)) {
        mu_Container *ctn = mu_get_current_container(ctx);
        ctn->rect = rn_ui_get_window_rect();

        mu_layout_row(ctx, 2, (int[]) { 84, -1 }, -1);
        mu_layout_begin_column(ctx); {
            mu_layout_row(ctx, 1, (int[]) { -1 }, 20);

            if (ui_coloured_button(ctx, "Hello!", 0x8debaaff, 0)) {
                log_debug("hiiii :)");
            }
            
            if (mu_button(ctx, "Hello2!"))
                log_debug("hiiii :)");

            if (mu_button(ctx, "Hello3!"))
                log_debug("hiiii :)");

            if (mu_button(ctx, "Hello4!"))
                log_debug("hiiii :)");

        } mu_layout_end_column(ctx);

        mu_begin_panel(ctx, "space"); {
            mu_button(ctx, "sus");
        
        } mu_end_panel(ctx);

        mu_end_window(ctx);
    }

    mu_end(ctx);
    return 0;
}