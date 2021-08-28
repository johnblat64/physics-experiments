#ifndef util_h
#define util_h

#include<SDL2/SDL.h>
#include "shapes.h"

void drawCross(SDL_Renderer *renderer, int centerX, int centerY, int radius){
    SDL_RenderDrawLine(renderer, centerX - radius, centerY, centerX + radius, centerY);
    SDL_RenderDrawLine(renderer, centerX, centerY - radius, centerX, centerY + radius);
}

void drawPolygon(SDL_Renderer *renderer, CollisionPolygon *cPoly){
    for(int i = 0; i < 7; i++){
        SDL_RenderDrawLine(renderer, cPoly->trfmPts[i].x, cPoly->trfmPts[i].y, cPoly->trfmPts[(i+1) % 7].x, cPoly->trfmPts[(i+1)%7].y);
        // SDL_SetRenderDrawColor(renderer, 100,100,100,255);
        // drawCross(renderer, cPoly->trfmPts[i].x, cPoly->trfmPts[i].y, 5 );
    }
}

void 
drawGrid(SDL_Renderer *renderer, unsigned int startingCanvasX, unsigned int startingCanvasY, unsigned int rows, unsigned int cols, unsigned int cellSize, SDL_Color color)
{
    //float ratio = (float)((float)canvasWidth/(float)viewportWidth);

    int endingCanvasX = startingCanvasX + (cols * cellSize);
    int endingCanvasY = startingCanvasY + (rows * cellSize);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int x = startingCanvasX; x <= cols; x++)
    {
        SDL_RenderDrawLine(renderer, x * cellSize, startingCanvasY, x * cellSize, endingCanvasY);
    }
    for (int y = startingCanvasY; y <= rows; y++)
    {
        SDL_RenderDrawLine(renderer, startingCanvasX, y * cellSize, endingCanvasX, y * cellSize);
    }
}


void drawCircle(SDL_Renderer *renderer, Circle c){
    int x = 0;
    int y = c.radius;
    int d = 3 - 2 * c.radius;

    SDL_RenderDrawPoint( renderer, c.midPoint.x + x , c.midPoint.y +  y );
    SDL_RenderDrawPoint( renderer, c.midPoint.x - x , c.midPoint.y +  y );

    SDL_RenderDrawPoint( renderer, c.midPoint.x + x , c.midPoint.y -  y );
    SDL_RenderDrawPoint( renderer, c.midPoint.x - x , c.midPoint.y -  y );

    SDL_RenderDrawPoint( renderer, c.midPoint.x + y , c.midPoint.y +  x );
    SDL_RenderDrawPoint( renderer, c.midPoint.x - y , c.midPoint.y +  x );

    SDL_RenderDrawPoint( renderer, c.midPoint.x + y , c.midPoint.y -  x );
    SDL_RenderDrawPoint( renderer, c.midPoint.x - y , c.midPoint.y -  x );

    while( y >= x ) {
        x++;
        if( d > 0 ) {
            y--;
            d = d + 4 * ( x - y ) + 10;
        }
        else {
            d = d + 4 * x + 6;
        }
        SDL_RenderDrawPoint( renderer, c.midPoint.x + x , c.midPoint.y +  y );
        SDL_RenderDrawPoint( renderer, c.midPoint.x - x , c.midPoint.y +  y );

        SDL_RenderDrawPoint( renderer, c.midPoint.x + x , c.midPoint.y -  y );
        SDL_RenderDrawPoint( renderer, c.midPoint.x - x , c.midPoint.y -  y );

        SDL_RenderDrawPoint( renderer, c.midPoint.x + y , c.midPoint.y +  x );
        SDL_RenderDrawPoint( renderer, c.midPoint.x - y , c.midPoint.y +  x );

        SDL_RenderDrawPoint( renderer, c.midPoint.x + y , c.midPoint.y -  x );
        SDL_RenderDrawPoint( renderer, c.midPoint.x - y , c.midPoint.y -  x );

    }
}

#endif