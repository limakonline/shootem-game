#include <SDL2/SDL_ttf.h>
#include <stdarg.h>

#include "commons.h"

#include "text.h"
#include "draw.h"

TTF_Font* font;

void initFont(void) {
    TTF_Init();

    font = TTF_OpenFont("main.ttf", 24);

    if(font == NULL) {
        printf("NO FONT %s\n", TTF_GetError());
    } else {
        printf("I GOT FONT\n");
    }
}

void drawText(SDL_Renderer* renderer, int x, int y, int r, int g, int b, char* text) {
    int w;
    int h;

    SDL_Color color = {r, g, b};
    SDL_Surface* surfMessage = TTF_RenderText_Solid(font, text, color);

    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfMessage);

    TTF_SizeText(font, text, &w, &h);

    SDL_Rect message_rect;
    message_rect.x = x;
    message_rect.y = y;
    message_rect.w = w;
    message_rect.h = h;
    SDL_RenderCopy(renderer, message, NULL, &message_rect);

}

void cleanFont(void) {
    TTF_CloseFont(font);
}
