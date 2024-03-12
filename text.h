#ifndef TEXT_H_
#define TEXT_H_

#define GLYPH_WIDTH 10
#define GLYPH_HEIGHT 15

void initFont(void);
void drawText(SDL_Renderer* renderer, int x, int y, int r, int g, int b, char* text);
void cleanFont(void);

#endif
