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

#define OFFSCREEN(MS)          { ANCHOR_LT, -1000, -1000, 0 },{ ANCHOR_LT, -1000, -1000, MS }
#define TELEPORT(ANCHOR, X, Y) { ANCHOR, X, Y, 0 }
#define MOVE(ANCHOR, X, Y, MS) { ANCHOR, X, Y, MS }
#define MOVE_X(ANCHOR, X, MS)  { ANCHOR, X, LAST_DST, MS }
#define MOVE_Y(ANCHOR, Y, MS)  { ANCHOR, LAST_DST, Y, MS }
#define WAIT(ANCHOR, MS)       { ANCHOR, LAST_DST, LAST_DST, MS }

#define ms +0

static const faseMovement sTrollfaceMoves[] = {
    TELEPORT  (ANCHOR_LB, RANDOM0, -250),
    MOVE_Y    (ANCHOR_LB, 0, 2000 ms),
    WAIT      (ANCHOR_LB, 4000 ms),
    MOVE_Y    (ANCHOR_LB, -250, 1000 ms),
    WAIT      (ANCHOR_LB, 2000 ms)
};

static const faseMovement sGlassesMoves[] = {
    TELEPORT  (ANCHOR_LT, RANDOM1, -250),
    MOVE      (ANCHOR_LB, RANDOM0, 0, 2000 ms),
    WAIT      (ANCHOR_LB, 4000 ms),
    MOVE_Y    (ANCHOR_LB, -250, 1000 ms),
    WAIT      (ANCHOR_LB, 2000 ms)
};

static const faseMovement sDealWithItMoves[] = {
    OFFSCREEN (2000 ms),
    TELEPORT  (ANCHOR_LB, RANDOM0, 0),
    WAIT      (ANCHOR_LB, 4000 ms),
    OFFSCREEN (2000 ms)
};

#undef ms

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
        for(i = 0; i < 1; ++i)
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
            for(i = 0; i < 1; ++i)
            {
                faseAnimationThink(anims[i], now);
            }
            Sleep(10);
        }
    }

    return (int) msg.wParam;
}
