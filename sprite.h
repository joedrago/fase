#ifndef SPRITE_H
#define SPRITE_H

#define LAST_DST (100000)
#define RANDOM0  (200000)
#define RANDOM1  (200001)
#define RANDOM2  (200002)
#define RANDOM3  (200003)
#define RANDOM4  (200004)
#define RANDOM5  (200005)
#define RANDOM6  (200006)
#define RANDOM7  (200007)
#define RANDOM8  (200008)
#define RANDOM9  (200009)

struct faseAnimation;

enum
{
    // left, center, right, top, center, bottom
    ANCHOR_XL = (1 << 3),
    ANCHOR_XC = (1 << 4),
    ANCHOR_XR = (1 << 5),
    ANCHOR_YT = (1 << 0),
    ANCHOR_YC = (1 << 1),
    ANCHOR_YB = (1 << 2),

    ANCHOR_L  = (ANCHOR_XL | ANCHOR_YC),
    ANCHOR_R  = (ANCHOR_XR | ANCHOR_YC),
    ANCHOR_T  = (ANCHOR_XC | ANCHOR_YT),
    ANCHOR_B  = (ANCHOR_XC | ANCHOR_YB),
    ANCHOR_LT = (ANCHOR_XL | ANCHOR_YT),
    ANCHOR_LB = (ANCHOR_XL | ANCHOR_YB),
    ANCHOR_RT = (ANCHOR_XR | ANCHOR_YT),
    ANCHOR_RB = (ANCHOR_XR | ANCHOR_YB),

    ANCHOR_C = (ANCHOR_XC | ANCHOR_YC)
};

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
    int t; // how far into currentMove?

    int x;
    int y;

    struct faseAnimation *anim;
} faseSprite;

faseSprite *faseSpriteCreate(int res, const faseMovement *moves, int count, faseSprite *top); // top = "which sprite is the new sprite on top of?"
void faseSpriteDestroy(faseSprite *sprite);
void faseSpriteReset(faseSprite *sprite, int now);
void faseSpriteAnimate(faseSprite *sprite, int X, int fromY, int toY, int duration);
void faseSpriteThink(faseSprite *sprite, int dt);

void faseSpriteStartup(HINSTANCE hInstance);
void faseSpriteRand();

#endif