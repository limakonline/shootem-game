#include "commons.h"
#include "draw.h"
#include "input.h"
#include "init.h"
#include "main.h"
#include "stage.h"
#include "text.h"

int main(void) {

    long then;
    float remainder; 
    memset(&app, 0, sizeof(App));

    initSDL();
    atexit(cleanup);

    initStage();
    initFont();
    then = SDL_GetTicks();
    remainder = 0;

    while (1) {

        prepareScene();
        doInput();

        app.delegate.logic();
        app.delegate.draw();

        presentScene();
        capFrameRate(&then, &remainder);
    }

    cleanFont();

    return 0;
}


