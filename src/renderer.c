#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_timer.h"

#include "microui.h"
#include "log.h"
#include "ui_atlas"

SDL_Window* rn_win;
SDL_Renderer* rn_ren;

mu_Context* rn_ui_ctx;
SDL_Texture *rn_ui_font;
int rn_quit;
unsigned char rn_scale = 2;

int rn_init() {
    log_info("Loading renderer");
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        log_fatal("Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    rn_win = SDL_CreateWindow("daw", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 850, 500, SDL_WINDOW_RESIZABLE);
    if (rn_win == NULL) {
        log_fatal("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    rn_ren = SDL_CreateRenderer(rn_win, -1, SDL_RENDERER_ACCELERATED);
    if (rn_ren == NULL) {
        log_fatal("Could not create renderer: %s\n", SDL_GetError());
        return 1;
    }
    SDL_GL_SetSwapInterval(-1);

    return 0;
}

void rn_deinit() {
    SDL_DestroyRenderer(rn_ren);
    SDL_DestroyWindow(rn_win);
    SDL_Quit();
}

void rn_ui_event(SDL_Event e);
void rn_loop() {
    //SDL_Rect squore = { .x = 0, .y = 0, .w = ATLAS_WIDTH, .h = ATLAS_HEIGHT};
    //SDL_RenderCopy(rn_ren, rn_ui_font, NULL, &squore);

    SDL_RenderPresent(rn_ren);
    SDL_RenderClear(rn_ren);
    SDL_SetRenderDrawBlendMode(rn_ren, SDL_BLENDMODE_BLEND);
    SDL_RenderSetScale(rn_ren, (float)rn_scale, (float)rn_scale);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        rn_ui_event(event);
        switch (event.type) {
            case SDL_QUIT: 
                log_info("Closing up");

                rn_deinit();
                exit(0);
                break;
        }
    };
}


const char rn_ui_button_map[256] = {
    [ SDL_BUTTON_LEFT   & 0xff ] =  MU_MOUSE_LEFT,
    [ SDL_BUTTON_RIGHT  & 0xff ] =  MU_MOUSE_RIGHT,
    [ SDL_BUTTON_MIDDLE & 0xff ] =  MU_MOUSE_MIDDLE,
};

const char rn_ui_key_map[256] = {
    [ SDLK_LSHIFT    & 0xff ] = MU_KEY_SHIFT,
    [ SDLK_RSHIFT    & 0xff ] = MU_KEY_SHIFT,
    [ SDLK_LCTRL     & 0xff ] = MU_KEY_CTRL,
    [ SDLK_RCTRL     & 0xff ] = MU_KEY_CTRL,
    [ SDLK_LALT      & 0xff ] = MU_KEY_ALT,
    [ SDLK_RALT      & 0xff ] = MU_KEY_ALT,
    [ SDLK_RETURN    & 0xff ] = MU_KEY_RETURN,
    [ SDLK_BACKSPACE & 0xff ] = MU_KEY_BACKSPACE,
};

void rn_ui_event(SDL_Event e) {
    switch (e.type) {
        case SDL_MOUSEMOTION: mu_input_mousemove(rn_ui_ctx, e.motion.x/rn_scale, e.motion.y/rn_scale); break;
        case SDL_MOUSEWHEEL: mu_input_scroll(rn_ui_ctx, 0, e.wheel.y * -30); break;
        case SDL_TEXTINPUT: mu_input_text(rn_ui_ctx, e.text.text); break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            int b = rn_ui_button_map[e.button.button & 0xff];

            if (b && e.type == SDL_MOUSEBUTTONDOWN)
                mu_input_mousedown(rn_ui_ctx, e.button.x/rn_scale, e.button.y/rn_scale, b);

            if (b && e.type ==   SDL_MOUSEBUTTONUP) 
                mu_input_mouseup(rn_ui_ctx, e.button.x/rn_scale, e.button.y/rn_scale, b);

            break;
        }

        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            int c = rn_ui_key_map[e.key.keysym.sym & 0xff];

            if (c && e.type == SDL_KEYDOWN) 
                mu_input_keydown(rn_ui_ctx, c);

            if (c && e.type ==   SDL_KEYUP) 
                mu_input_keyup(rn_ui_ctx, c);

            break;
        }
    }
}

int rn_ui_get_text_width(mu_Font _, const char *text, int len) {
    int res = 0;
    for (const char *p = text; *p && len--; p++) {
        if ((*p & 0xc0) == 0x80) { continue; }
        int chr = mu_min((unsigned char) *p, 127);

        res += atlas[ATLAS_FONT + chr].w;
    }
    return res;
}

int rn_ui_get_text_height(mu_Font _) {
  return 18;
}

void rn_ui_draw_text(const char *text, mu_Vec2 pos, mu_Color color) {
    SDL_Rect dst = { pos.x, pos.y, 0, 0 };
    for (const char *p = text; *p; p++) {
        if ((*p & 0xc0) == 0x80) { continue; }
        int chr = mu_min((unsigned char) *p, 127);
        SDL_Rect *src = (SDL_Rect*) &atlas[ATLAS_FONT + chr];

        dst.w = src->w;
        dst.h = src->h;

        SDL_SetTextureColorMod(rn_ui_font, color.r, color.g, color.b);
        SDL_RenderCopy(rn_ren, rn_ui_font, src, &dst);
        
        dst.x += dst.w;
    }
};

void rn_ui_draw_icon(int id, mu_Rect rect, mu_Color color) {
    SDL_Rect *src = (SDL_Rect*) &atlas[id];
    SDL_Rect dst = {
        .x = rect.x + (rect.w - src->w) / 2,
        .y = rect.y + (rect.h - src->h) / 2,
        .w = src->w,
        .h = src->h
    };

    SDL_SetTextureColorMod(rn_ui_font, color.r, color.g, color.b);
    SDL_RenderCopy(rn_ren, rn_ui_font, src, &dst);
};

void rn_ui_set_clip_rect(mu_Rect rect) {
    SDL_Rect *_rect = (SDL_Rect*) &rect;
    if (rect.w == 0x1000000)
        _rect = NULL;

    SDL_RenderSetClipRect(rn_ren, _rect);
};

void rn_ui_draw_rect(mu_Rect rect, mu_Color color) {
    SDL_Rect *_rect = (SDL_Rect*) &rect;
    SDL_SetRenderDrawColor(rn_ren, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(rn_ren, _rect);
}

/* 32bit fnv-1a hash */
#define HASH_INITIAL 2166136261

static void hash(mu_Id *hash, const void *data, int size) {
    const unsigned char *p = data;
    while (size--) {
        *hash = (*hash ^ *p++) * 16777619;
    }
}

int rn_ui_draw() {
    mu_Command *cmd = NULL;
    while (mu_next_command(rn_ui_ctx, &cmd)) {
      switch (cmd->type) {
        case MU_COMMAND_TEXT: rn_ui_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color); break;
        case MU_COMMAND_RECT: rn_ui_draw_rect(cmd->rect.rect, cmd->rect.color); break;
        case MU_COMMAND_ICON: rn_ui_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color); break;
        case MU_COMMAND_CLIP: rn_ui_set_clip_rect(cmd->clip.rect); break;
      }
    }

    return 0;
}

void rn_ui_draw_frame(mu_Context *ctx, mu_Rect rect, int colorid) {
    mu_Color color;
    if (colorid >= MU_COLOR_MAX || colorid < 0) {
        color = mu_color(
            (colorid >> 24) & 0xFF, 
            (colorid >> 16) & 0xFF,
            (colorid >>  8) & 0xFF,
            colorid & 0xFF
        );
    } else {
        color = ctx->style->colors[colorid];
    }

    mu_Color border = ctx->style->colors[MU_COLOR_BORDER];
    
    if (!(colorid == MU_COLOR_SCROLLBASE  ||
        colorid == MU_COLOR_SCROLLTHUMB ||
        colorid == MU_COLOR_TITLEBG) || border.a) {

        mu_draw_rect(ctx, 
            mu_rect(rect.x-1, rect.y, rect.w+2, rect.h+1), 
            border
        );

        mu_draw_rect(ctx, 
            mu_rect(rect.x, rect.y-1, rect.w, rect.h+3), 
            border
        );
    }

    mu_draw_rect(ctx, rect, color);
    mu_draw_box(ctx, rect, mu_color(255, 255, 255, 10));
}

mu_Context *rn_ui_init() {
    rn_ui_font = SDL_CreateTexture(rn_ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, ATLAS_WIDTH, ATLAS_HEIGHT);
    SDL_UpdateTexture(rn_ui_font, NULL, atlas_texture, ATLAS_WIDTH*4);
    SDL_SetTextureBlendMode(rn_ui_font, SDL_BLENDMODE_BLEND);

    rn_ui_ctx = malloc(sizeof(mu_Context));
    mu_init(rn_ui_ctx);

    rn_ui_ctx->draw_frame  = rn_ui_draw_frame;
    rn_ui_ctx->text_width  = rn_ui_get_text_width;
    rn_ui_ctx->text_height = rn_ui_get_text_height;
    rn_ui_ctx->style->spacing = 5;
    rn_ui_ctx->style->padding = 4;
    rn_ui_ctx->style->colors[MU_COLOR_PANELBG] = mu_color(50,  50,  50,  255);

    return rn_ui_ctx;
}

mu_Rect rn_ui_get_window_rect() {
    int w, h;
    SDL_GetWindowSize(rn_win, &w, &h);

    return mu_rect(0, 0, w/rn_scale, h/rn_scale);
}