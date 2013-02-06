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

faseAnimation *faseAnimationCreate(faseSprite **sprites, int count)
{
    int i;
    faseAnimation *anim = (faseAnimation *)malloc(sizeof(faseAnimation));
    anim->sprites = sprites; // take ownership
    anim->count = count;
    anim->duration = 0;

    for(i = 0; i < anim->count; ++i)
    {
        if(anim->sprites[i]->duration > anim->duration)
        {
            anim->duration = anim->sprites[i]->duration;
        }
    }
    return anim;
}

void faseAnimationDestroy(faseAnimation *anim)
{
    int i;
    for(i = 0; i < anim->count; ++i)
    {
        faseSpriteDestroy(anim->sprites[i]);
    }
    free(anim->sprites);
    free(anim);
}

void faseAnimationReset(faseAnimation *anim)
{
    int i;
    for(i = 0; i < anim->count; ++i)
    {
        faseSpriteReset(anim->sprites[i]);
    }
    anim->start = GetTickCount();
}

void faseAnimationThink(faseAnimation *anim)
{
    int now = (int)GetTickCount();
    int dt = now - anim->start;

    if(dt <= anim->duration)
    {
        int i;
        for(i = 0; i < anim->count; ++i)
        {
            faseSpriteThink(anim->sprites[i]);
        }
    }
    else
    {
        faseAnimationReset(anim);
    }
}
