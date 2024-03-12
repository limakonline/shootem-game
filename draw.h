#ifndef DRAW_H_
#define DRAW_H_

void prepareScene(void);
void presentScene(void);
SDL_Texture* loadTexture(char *filename);
void blit(SDL_Texture* texture, int x, int y);
void blit_frame(SDL_Texture* texture, int x, int y);
void blit_rect(SDL_Texture* texture, SDL_Rect* src, int x, int y);

#endif
