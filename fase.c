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

int sVisitOnlyOnce = 0;

static const faseMovement sTrollfaceMoves[] = {
    { 6,   RANDOM,        0,    0 },
    { 6, LAST_DST,      100, 2000 },
    { 6, LAST_DST,      100, 4000 },
    { 6, LAST_DST,        0, 4000 },
    { 6, LAST_DST, LAST_DST, 4000 }
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    const char *c;
    faseAnimation *anim;

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    c = lpCmdLine;
    for(; *c; ++c)
    {
        if(*c == '-')
        {
            for(; *c && (*c != ' ') && (*c != '\t'); ++c)
            {
                switch(*c)
                {
                case 'o':
                    sVisitOnlyOnce = 1;
                };
            }
        }
    }

    faseSpriteStartup(hInstance);

    {
        int i;
        faseSprite *sprites[3];
        for(i = 0; i < 3; ++i)
        {
            sprites[i] = faseSpriteCreate(IDB_FASE, sTrollfaceMoves, sizeof(sTrollfaceMoves) / sizeof(faseMovement));
        }
        anim = faseAnimationCreate(sprites, 3);
        faseAnimationReset(anim);
    }

    while(1)
    {
        if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
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
            faseAnimationThink(anim);
            Sleep(10);
        }
    }

    return (int) msg.wParam;
}
