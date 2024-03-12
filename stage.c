#include <string.h>
#include "commons.h"
#include "main.h"
#include "utils.h"
#include "text.h"

static SDL_Texture* bulletTexture;
static SDL_Texture* enemyTexture;
static SDL_Texture* playerTexture;
static SDL_Texture* alienBulletTexture;
static float enemySpawnTimer;
static float stageResetTimer;
static int highscore;

static  void fireBullet(void) {
    struct Entity* bullet;

    bullet = malloc(sizeof(struct Entity));
    memset(bullet, 0, sizeof(struct Entity));
    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->x = player->x;
    bullet->y = player->y;
    bullet->dx = PLAYER_BULLET_SPEED;
    bullet->health = 1;
    bullet->texture = bulletTexture;
    bullet->side = SIDE_PLAYER;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->y += (player->h / 2) - (bullet->h / 2);

    player->reload = 0;
}

static void doPlayer() {
    if (player != NULL) {
        player->dx = player->dy = 0;

        if (player->reload > 0) {
            player->reload--;
        }

        if (app.keyboard[SDL_SCANCODE_UP]) {
            player->dy = -PLAYER_SPEED;
        }
        if (app.keyboard[SDL_SCANCODE_DOWN]) {
            player->dy = PLAYER_SPEED;
        }
        if (app.keyboard[SDL_SCANCODE_LEFT]) {
            player->dx = -PLAYER_SPEED;
        }
        if (app.keyboard[SDL_SCANCODE_RIGHT]) {
            player->dx = PLAYER_SPEED;
        }

        if (app.keyboard[SDL_SCANCODE_SPACE] && player->reload == 0) {
            fireBullet();
        }
    }
}

static void fireAlienBullet(struct Entity* e) {
    struct Entity* bullet;

    bullet = malloc(sizeof(struct Entity));
    memset(bullet, 0, sizeof(struct Entity));
    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->x = e->x;
    bullet->y = e->y;
    bullet->health = 1;
    bullet->texture = alienBulletTexture;
    bullet->side = SIDE_ALIEN;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->x += (e->w / 2) - (bullet->w / 2);
    bullet->y += (e->h / 2) - (bullet->h / 2);

    // calcSlope(e->x + (e->w / 2), e->y + (e->h / 2), player->x, player->y, &bullet->dx, &bullet->dy); 
    // that shoudl be just ALIEN_BULLET_SPEED
    bullet->dx = -10;//-ALIEN_BULLET_SPEED;
    bullet->dy = -10;//-ALIEN_BULLET_SPEED;

    e->reload = (rand() % FPS * 2);
}

static void doEnemies() {
    struct Entity *e;

    for (e = stage.fighterHead.next; e != NULL; e = e->next) {
        if (e != player && player != NULL && --e->reload <= 0) {
            fireAlienBullet(e);
        }
    }
}

static int bulletHitFighter(struct Entity* b) {
    struct Entity* e;

    for (e = stage.fighterHead.next; e != NULL; e = e->next) {

        if (e->side != b->side && collision(b->x, b->y, b->w, b->h, e->x, e->y, e->w, e->h)) {
            b->health = 0;
            e->health = 0;

            stage.score++;

            return 1;
        }
    }

    return 0;
}

static void doBullets(void) {
    struct Entity *b, *prev;

    prev = &stage.bulletHead;

    for (b = stage.bulletHead.next; b != NULL; b = b->next) {
        b->x += b->dx;
        // b->y += b->dy;

        if (bulletHitFighter(b) || b->x < -b->w || b->x > SCREEN_WIDTH || b->y > SCREEN_HEIGHT) {
            if (b == stage.bulletTail) {
                stage.bulletTail = prev;
            }

            prev->next = b->next;
            //free(b);
            b = prev;
        }

        // prev = b;
    }
}

static void doFighters(void) {
    struct Entity *e, *prev;

    prev = &stage.fighterHead;

    for (e = stage.fighterHead.next; e != NULL; e = e->next) {
        e->x += e->dx;
        e->y += e->dy;

        if (e != player && e->x < -e->w) {
            e->health = 0;
        }

        if (e->health == 0) {
            if (e == player) {
                player = NULL;
            }

            if (e == stage.fighterTail) {
                stage.fighterTail = prev;
            }

            prev->next = e->next;
            //free(e);
            e = prev;
        }

        prev = e;
    }
}

static void spawnEnemies(void) {
    struct Entity *enemy;

    if (--enemySpawnTimer <= 0) {
        enemy = malloc(sizeof(struct Entity));
        memset(enemy, 0, sizeof(struct Entity));
        stage.fighterTail->next = enemy;
        stage.fighterTail = enemy;

        enemy->x = SCREEN_WIDTH;
        enemy->y = rand() % SCREEN_HEIGHT;
        enemy->side = SIDE_ALIEN;
        enemy->health = 1;
        enemy->texture = enemyTexture;
        SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);

        enemy->dx = -(2 + (rand() % 4));

        enemySpawnTimer = 30 + (rand() % 60);
        enemy->reload = FPS * (1 + (rand() % 3));
    }
}

static void drawFighters(void) {
    struct Entity *e;

    for (e = stage.fighterHead.next; e != NULL; e = e->next) {
        blit(e->texture, e->x, e->y);
    }
}

static void clipPlayer(void) {
    if (player != NULL) {
        if (player->x < 0) {
            player->x = 0;
        }

        if (player->y < 0) {
            player->y = 0;
        }

        if (player->x > SCREEN_WIDTH / 2) {
            player->x = SCREEN_WIDTH / 2;
        }

        if (player->y > SCREEN_HEIGHT - player->h) {
            player->y = SCREEN_HEIGHT - player->h;
        }
    }
}

static void initPlayer(void) {
    player = malloc(sizeof(struct Entity));
    memset(player, 0, sizeof(struct Entity));
    stage.fighterTail->next = player;
    stage.fighterTail = player;

    player->x = 100;
    player->y = 100;
    player->side = SIDE_PLAYER;
    player->health = 1;
    player->texture = playerTexture;
    SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);
}

static void resetStage(void) {
    struct Entity *e;

    while (stage.fighterHead.next) {
        e = stage.fighterHead.next;
        stage.fighterHead.next = e->next;
        free(e);
    }

    while (stage.bulletHead.next) {
        e = stage.bulletHead.next;
        stage.bulletHead.next = e->next;
        free(e);
    }

    memset(&stage, 0, sizeof(Stage));
    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail = &stage.bulletHead;
    initPlayer();
    enemySpawnTimer = 0;

    stageResetTimer = FPS * 2;
    stage.score = 0;
}

static void logic(void) {
    doPlayer();
    clipPlayer();

    doFighters();

    spawnEnemies();

    doEnemies();

    doBullets();

    if (player == NULL && --stageResetTimer <= 0) {
        resetStage();
    }
}

static void drawBullets(void) {
    struct Entity *b;

    for (b = stage.bulletHead.next; b != NULL; b = b->next) {
        blit(b->texture, b->x, b->y);
    }
}

static void drawHud(SDL_Renderer* renderer) {
    char *buffer;
    char *format;
    format = "SCORE: %3d";
    sprintf(buffer, format, stage.score);
    drawText(renderer, 10, 10, 255, 255, 255, buffer);

    if (stage.score > 0 && stage.score == highscore) {
        drawText(renderer, 960, 10, 0, 255, 0, "SCORE: 001");
    } else {
        drawText(renderer, 960, 10, 255, 255, 255, "SCORE: 002");
    }
}

static void draw(void) {
    drawFighters();
    drawBullets();
    drawHud(app.renderer);
}

void capFrameRate(long* then, float* reminder) {
    long wait, frameTime;

    wait = 16 + *reminder;

    *reminder -= (int) *reminder;

    frameTime = SDL_GetTicks() - *then;

    wait -= frameTime;

    if (wait < 1) {
        wait = 1;
    }

    SDL_Delay(wait);
    *reminder += 0.667;
    *then = SDL_GetTicks();
}


void initStage(void) {
    app.delegate.logic = logic;
    app.delegate.draw = draw;

    memset(&stage, 0, sizeof(Stage));
    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail = &stage.bulletHead;

    initPlayer();

    bulletTexture = loadTexture("bullet.png");
    enemyTexture = loadTexture("enemy.png");
    alienBulletTexture = loadTexture("alien_bullet.png");
    playerTexture = loadTexture("player.png");

    resetStage();
}

