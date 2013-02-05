#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "resource.h"
#include "sprite.h"

int sVisitOnlyOnce = 0;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    const char *c;
    int i;

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

    for(i = 0; i < 3; ++i)
    {
        faseSprite *sprite = faseSpriteCreate(IDB_FASE);
        faseSpriteStart(sprite);
    }

    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int) msg.wParam;
}
