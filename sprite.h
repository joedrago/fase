#ifndef SPRITE_H
#define SPRITE_H

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
    unsigned int start;
    int duration;
    int srcX;
    int srcY;
    int dstX;
    int dstY;
    faseSpriteState state;
} faseSprite;

faseSprite *faseSpriteCreate(int res);
void faseSpriteDestroy(faseSprite *sprite);
void faseSpriteStart(faseSprite *sprite);
void faseSpriteAnimate(faseSprite *sprite, int X, int fromY, int toY, int duration);
void faseSpriteThink(faseSprite *sprite);

void faseSpriteStartup(HINSTANCE hInstance);

#endif