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

static const faseMovement sMovesDWIFace[] = {
    TELEPORT  (ANCHOR_LB, RANDOM0, -250),
    MOVE_Y    (ANCHOR_LB, 0, 2000 ms),
    WAIT      (ANCHOR_LB, 4000 ms),
    MOVE_Y    (ANCHOR_LB, -250, 1000 ms),
    WAIT      (ANCHOR_LB, 2000 ms)
};
static const faseMovement sMovesDWIGlasses[] = {
    TELEPORT  (ANCHOR_LT, RANDOM1, -250),
    MOVE      (ANCHOR_LB, RANDOM0, 0, 2000 ms),
    WAIT      (ANCHOR_LB, 4000 ms),
    MOVE_Y    (ANCHOR_LB, -250, 1000 ms),
    WAIT      (ANCHOR_LB, 2000 ms)
};
static const faseMovement sMovesDWIText[] = {
    OFFSCREEN (2000 ms),
    TELEPORT  (ANCHOR_LB, RANDOM0, 0),
    WAIT      (ANCHOR_LB, 4000 ms),
    OFFSCREEN (2000 ms)
};

static const faseMovement sMovesPeekBottom[] = {
    TELEPORT  (ANCHOR_LB, RANDOM0, -250),
    MOVE_Y    (ANCHOR_LB, -150, 2000 ms),
    WAIT      (ANCHOR_LB, 4000 ms),
    MOVE_Y    (ANCHOR_LB, -250, 1000 ms),
    WAIT      (ANCHOR_LB, 2000 ms)
};

static const faseMovement sMovesPeekTop[] = {
    TELEPORT  (ANCHOR_LT, RANDOM0, -250),
    MOVE_Y    (ANCHOR_LT, -150, 2000 ms),
    WAIT      (ANCHOR_LT, 4000 ms),
    MOVE_Y    (ANCHOR_LT, -250, 1000 ms),
    WAIT      (ANCHOR_LT, 2000 ms)
};

static const faseMovement sMovesPeekBottomLeft[] = {
    TELEPORT  (ANCHOR_LB, -310, -265),
    MOVE      (ANCHOR_LB, -120, -120, 2000 ms),
    WAIT      (ANCHOR_LB, 4000 ms),
    MOVE      (ANCHOR_LB, -310, -265, 1000 ms),
    WAIT      (ANCHOR_LB, 2000 ms)
};

#undef ms

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    faseAnimation *anim;
    faseAnimation *anims[4];
    int animCount = 0;
    int curAnim = 0;

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    faseSpriteStartup(hInstance);

    // "Deal with it"
    {
        faseSprite **sprites = (faseSprite **)malloc(sizeof(faseSprite *) * 3);
        sprites[0] = faseSpriteCreate(IDB_FASE,       sMovesDWIFace,    sizeof(sMovesDWIFace)    / sizeof(faseMovement), NULL);
        sprites[1] = faseSpriteCreate(IDB_GLASSES,    sMovesDWIGlasses, sizeof(sMovesDWIGlasses) / sizeof(faseMovement), sprites[0]);
        sprites[2] = faseSpriteCreate(IDB_DEALWITHIT, sMovesDWIText,    sizeof(sMovesDWIText)    / sizeof(faseMovement), sprites[1]);
        anim = faseAnimationCreate(sprites, 3);
        faseAnimationReset(anim, (int)GetTickCount());
        anims[animCount++] = anim;
    }

    // "peek from bottom"
    {
        faseSprite **sprites = (faseSprite **)malloc(sizeof(faseSprite *) * 1);
        sprites[0] = faseSpriteCreate(IDB_FASE, sMovesPeekBottom, sizeof(sMovesPeekBottom) / sizeof(faseMovement), NULL);
        anim = faseAnimationCreate(sprites, 1);
        faseAnimationReset(anim, (int)GetTickCount());
        anims[animCount++] = anim;
    }

    // "peek from top"
    {
        faseSprite **sprites = (faseSprite **)malloc(sizeof(faseSprite *) * 1);
        sprites[0] = faseSpriteCreate(IDB_FASEFLIPPED, sMovesPeekTop, sizeof(sMovesPeekTop) / sizeof(faseMovement), NULL);
        anim = faseAnimationCreate(sprites, 1);
        faseAnimationReset(anim, (int)GetTickCount());
        anims[animCount++] = anim;
    }

    // "peek from bottom left" (start menu)
    {
        faseSprite **sprites = (faseSprite **)malloc(sizeof(faseSprite *) * 1);
        sprites[0] = faseSpriteCreate(IDB_FASELEFT, sMovesPeekBottomLeft, sizeof(sMovesPeekBottomLeft) / sizeof(faseMovement), NULL);
        anim = faseAnimationCreate(sprites, 1);
        faseAnimationReset(anim, (int)GetTickCount());
        anims[animCount++] = anim;
    }

    curAnim = rand() % animCount;

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
            if(!faseAnimationThink(anims[curAnim], (int)GetTickCount()))
            {
                int i;
                for(i = 0; i < animCount; ++i)
                {
                    faseAnimationHide(anims[i]);
                }

                curAnim = rand() % animCount;
                faseAnimationReset(anims[curAnim], (int)GetTickCount());
            }
            Sleep(10);
        }
    }

    return (int) msg.wParam;
}
