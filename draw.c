#include "commons.h"
#include "main.h"

void prepareScene(void) {
    SDL_SetRenderDrawColor(app.renderer, 96, 128, 255, 255);
    SDL_RenderClear(app.renderer);
}

void presentScene(void) {
    SDL_RenderPresent(app.renderer);
}

SDL_Texture* loadTexture(char *filename) {
    SDL_Texture* texture;

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

    texture = IMG_LoadTexture(app.renderer, filename);

    return texture;
}

void blit(SDL_Texture* texture, int x, int y) { 
    SDL_Rect dest;
    SDL_Rect src;

    dest.x = x;
    dest.y = y;

    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
    SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}

void blit_frame(SDL_Texture* texture, int x, int y) { 
    SDL_Rect dest;
    SDL_Rect src;

    src.x = 16;
    src.y = 0;
    src.w = 16;
    src.h = 16;

    dest.x = x;
    dest.y = y;
    dest.w = 32;
    dest.h = 32;

    // SDL_QueryTexture(texture, NULL, NULL, NULL, &dest.h);
    SDL_RenderCopyEx(app.renderer, texture, &src, &dest, 90, NULL, SDL_FLIP_NONE);
}

void blit_rect(SDL_Texture* texture, SDL_Rect* src, int x, int y) {
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    dest.w = src->w;
    dest.h = src->h;

    SDL_RenderCopy(app.renderer, texture, src, &dest);
}
