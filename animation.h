#ifndef ANIMATION_H
#define ANIMATION_H

#include "sprite.h"

#define MAX_RANDS 10

typedef struct faseAnimation
{
    faseSprite **sprites;
    int count;
    int duration;
    int start;
    int last;
    int randX[MAX_RANDS];
    int randY[MAX_RANDS];
    int bigw;
    int bigh;
} faseAnimation;

faseAnimation *faseAnimationCreate(faseSprite **sprites, int count);
void faseAnimationDestroy(faseAnimation *anim);
void faseAnimationHide(faseAnimation *anim);
void faseAnimationReset(faseAnimation *anim, int now);
int faseAnimationThink(faseAnimation *anim, int now);

#endif