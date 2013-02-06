#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "resource.h"
#include "sprite.h"
#include "animation.h"

#define OFFSCREEN(MS)          { 6, 0, 0, 0 },{ 6, 0, 0, MS }
#define START(ANCHOR, X, Y)    { ANCHOR, X, Y, 0 }
#define MOVE(ANCHOR, X, Y, MS) { ANCHOR, X, Y, MS }
#define MOVE_X(ANCHOR, X, MS)  { ANCHOR, X, LAST_DST, MS }
#define MOVE_Y(ANCHOR, Y, MS)  { ANCHOR, LAST_DST, Y, MS }
#define WAIT(ANCHOR, MS)       { ANCHOR, LAST_DST, LAST_DST, MS }

static const faseMovement sTrollfaceMoves[] = {
    START (6, RANDOM0,         0      ),
    MOVE_Y(6,     150,            2000),
    WAIT  (6,                     4000),
    MOVE_Y(6,                  0, 1000),
    WAIT  (6,                     2000)
};

static const faseMovement sGlassesMoves[] = {
    START (0, RANDOM1,      -100      ),
    MOVE  (6, RANDOM0,       150, 2000),
    WAIT  (6,                     4000),
    MOVE_Y(6,            0,       1000),
    WAIT  (6,                     2000)
};

static const faseMovement sDealWithItMoves[] = {
    { 6,  RANDOM0,        0,    0 },
    { 6,  RANDOM0,        0, 2000 },
    { 6,  RANDOM0,      150,    0 },
    { 6, LAST_DST, LAST_DST, 4000 },
    { 6,  RANDOM0,        0,    0 },
    { 6, LAST_DST, LAST_DST, 2000 }
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    int i;
    MSG msg;
    faseAnimation *anim;
    faseAnimation *anims[2];
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    faseSpriteStartup(hInstance);

    {
        faseSprite *prevSprite = NULL;
        for(i = 0; i < 2; ++i)
        {
            faseSprite **sprites = (faseSprite **)malloc(sizeof(faseSprite *) * 3);
            sprites[0] = faseSpriteCreate(IDB_FASE,       sTrollfaceMoves,  sizeof(sTrollfaceMoves)  / sizeof(faseMovement), prevSprite);
            sprites[1] = faseSpriteCreate(IDB_GLASSES,    sGlassesMoves,    sizeof(sGlassesMoves)    / sizeof(faseMovement), sprites[0]);
            sprites[2] = faseSpriteCreate(IDB_DEALWITHIT, sDealWithItMoves, sizeof(sDealWithItMoves) / sizeof(faseMovement), sprites[1]);
            anim = faseAnimationCreate(sprites, 3);
            faseAnimationReset(anim, (int)GetTickCount());
            anims[i] = anim;
            prevSprite = sprites[2];
        }
    }

    while(1)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
            {
                break;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            int now = (int)GetTickCount();
            for(i = 0; i < 2; ++i)
            {
                faseAnimationThink(anims[i], now);
            }
            Sleep(10);
        }
    }

    return (int) msg.wParam;
}
