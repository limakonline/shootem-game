#include "commons.h"
#include "main.h"

static SDL_Texture* bulletTexture;
static SDL_Texture* enemyTexture;
static float enemySpawnTimer;

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
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->y += (player->h / 2) - (bullet->h / 2);

    player->reload = 0;
}

static void doPlayer() {
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

static void doBullets(void) {
    struct Entity *b, *prev;

    prev = &stage.bulletHead;

    for (b = stage.bulletHead.next; b != NULL; b = b->next) {
        b->x += b->dx;
        b->y += b->dy;

        if (b->x > SCREEN_WIDTH) {
            if (b == stage.bulletTail) {
                stage.bulletTail = prev;
            }

            prev->next = b->next;
            free(b);
            b = prev;
        }

        prev = b;
    }
}

static void doFighters(void) {
    struct Entity *e, *prev;

    prev = &stage.fighterHead;

    for (e = stage.fighterHead.next; e != NULL; e = e->next) {
        e->x += e->dx;
        e->y += e->dy;

        if (e != player && e->x < -e->w) {
            if (e == stage.fighterTail) {
                stage.fighterTail = prev;
            }

            prev->next = e->next;
            free(e);
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
        enemy->texture = enemyTexture;
        SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);

        enemy->dx = -(2 + (rand() % 4));

        enemySpawnTimer = 30 + (rand() % 60);
    }
}

static void drawFighters(void) {
    struct Entity *e;

    for (e = stage.fighterHead.next; e != NULL; e = e->next) {
        blit(e->texture, e->x, e->y);
    }
}

static void logic(void) {
    doPlayer();

    doFighters();

    doBullets();

    spawnEnemies();
}

static void initPlayer(void) {
    player = malloc(sizeof(struct Entity));
    memset(player, 0, sizeof(struct Entity));
    stage.fighterTail->next = player;
    stage.fighterTail = player;

    player->x = 100;
    player->y = 100;
    player->texture = loadTexture("player.png");
    SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);
}

static void drawBullets(void) {
    struct Entity *b;

    for (b = stage.bulletHead.next; b != NULL; b = b->next) {
        blit(b->texture, b->x, b->y);
    }
}

static void draw(void) {
    drawFighters();
    drawBullets();
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
}

