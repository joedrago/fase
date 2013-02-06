#ifndef SPRITE_H
#define SPRITE_H

#define LAST_DST (-100000)
#define RANDOM   (-100001)

typedef struct faseMovement
{
    int anchor; // 0-8, clockwise from top left, 8=center
    int x;
    int y;
    int duration;
} faseMovement;

typedef enum faseSpriteState
{
    FSS_ENTERING = 0,
    FSS_SHOWING,
    FSS_LEAVING,
    FSS_WAITING,

    FSS_COUNT
} faseSpriteState;

typedef struct faseSprite
{
    HWND hwnd;
    HBITMAP hbmp;
    int bmpw;
    int bmph;

    const faseMovement *moves;
    int count;
    int duration;
    int currentMove;

    int x;
    int y;

    unsigned int start;
} faseSprite;

faseSprite *faseSpriteCreate(int res, const faseMovement *moves, int count);
void faseSpriteDestroy(faseSprite *sprite);
void faseSpriteReset(faseSprite *sprite);
void faseSpriteAnimate(faseSprite *sprite, int X, int fromY, int toY, int duration);
void faseSpriteThink(faseSprite *sprite);

void faseSpriteStartup(HINSTANCE hInstance);

#endif