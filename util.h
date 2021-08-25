#ifndef util_h
#define util_h

#include<SDL2/SDL.h>

void drawCross(SDL_Renderer *renderer, int centerX, int centerY, int radius){
    SDL_RenderDrawLine(renderer, centerX - radius, centerY, centerX + radius, centerY);
    SDL_RenderDrawLine(renderer, centerX, centerY - radius, centerX, centerY + radius);
}

#endif