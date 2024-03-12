#ifndef STRUCTS_H_
#define STRUCTS_H_

typedef struct {
    void (*logic)(void);
    void (*draw)(void);
} Delegate;

typedef struct {
    SDL_Renderer* renderer;
    SDL_Window* window;
    Delegate delegate;
    int keyboard[MAX_KEYBOARD_KEYS];
} App;

struct Entity {
    float x;
    float y;
    int w;
    int h;
    float dx;
    float dy;
    int health;
    int reload;
    int side;
    SDL_Texture* texture;
    struct Entity *next;
};

typedef struct {
    struct Entity fighterHead, *fighterTail;
    struct Entity bulletHead, *bulletTail;
    int score;
} Stage;

#endif
