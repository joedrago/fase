#ifndef ANIMATION_H
#define ANIMATION_H

#include "sprite.h"

typedef struct faseAnimation
{
    faseSprite **sprites;
    int count;
    int duration;
    int start;
} faseAnimation;

faseAnimation *faseAnimationCreate(faseSprite **sprites, int count);
void faseAnimationDestroy(faseAnimation *anim);
void faseAnimationReset(faseAnimation *anim);
void faseAnimationThink(faseAnimation *anim);

#endif