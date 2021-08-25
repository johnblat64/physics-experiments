#include <SDL2/SDL.h>
#include "v2d.h"
#include "util.h"
#include "darr.h"
#include "tween.h"



typedef struct CollisionPolygon {
    v2d *modelPts;
    v2d *trfmPts;
    float rotationRads;
    v2d pos;
    SDL_bool flip;
    Lerp lerp;
} CollisionPolygon;


void flipModel(CollisionPolygon *cPoly) {
    for(int i = 0; i < lenDarr(cPoly->trfmPts); i++){
        cPoly->trfmPts[i] = v2d_scalar_mult(-1, cPoly->trfmPts[i]); 
    }
}

void polyDeepCopy(CollisionPolygon *src, CollisionPolygon *dest){
    dest->modelPts = initDarr(dest->modelPts, sizeof(v2d), capDarr(src->modelPts));
    dest->trfmPts = initDarr(dest->trfmPts, sizeof(v2d), capDarr(src->trfmPts));

    for(int i = 0; i < lenDarr(src->modelPts); i++){
        addDarr(dest->modelPts, src->modelPts[i], v2d);
        addDarr(dest->trfmPts, src->trfmPts[i], v2d);
    }

    dest->rotationRads = src->rotationRads;
    dest->pos = src->pos;
    dest->flip = src->flip;
    dest->lerp.step = src->lerp.step;
    dest->lerp.remainingTime = src->lerp.remainingTime;
    dest->lerp.value = src->lerp.value;

}

int
main() {
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Polygon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640,480, 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED );

    CollisionPolygon cPoly_1;
    cPoly_1.modelPts = initDarr(cPoly_1.modelPts, sizeof(v2d), 7);
    cPoly_1.trfmPts = initDarr(cPoly_1.trfmPts, sizeof(v2d), 7);
    addDarr(cPoly_1.modelPts, ((v2d){10,-20}), v2d);
    addDarr(cPoly_1.modelPts, ((v2d){-10,-20}), v2d);
    addDarr(cPoly_1.modelPts, ((v2d){-20,-5}), v2d);
    addDarr(cPoly_1.modelPts, ((v2d){-12,25}), v2d);
    addDarr(cPoly_1.modelPts, ((v2d){100,80}), v2d);
    addDarr(cPoly_1.modelPts, ((v2d){110,75}), v2d);
    addDarr(cPoly_1.modelPts, ((v2d){110,68}), v2d);
    for(int i = 0; i < lenDarr(cPoly_1.modelPts); i++){
        addDarr(cPoly_1.trfmPts, ((v2d){0,0}), v2d);
    }
    cPoly_1.rotationRads = 0.0f;
    cPoly_1.pos = (v2d){200,200};
    cPoly_1.flip = SDL_FALSE;
    cPoly_1.lerp = lerpInit(0,0,0);

    CollisionPolygon cPoly_2;
    polyDeepCopy(&cPoly_1, &cPoly_2);

    // v2d relPts[7];
    // relPts[0] = (v2d){10,-20};
    // relPts[1] = (v2d){-10,-20};
    // relPts[2] = (v2d){-20,-5};
    // relPts[3] = (v2d){-12,25};
    // relPts[4] = (v2d){100,80};
    // relPts[5] = (v2d){110,75};
    // relPts[6] = (v2d){110,68};


    v2d trfPts[7];

    

    SDL_bool quit = SDL_FALSE;
    SDL_Event event;
    SDL_bool leftFlipperJustPressed = SDL_FALSE;
    SDL_bool leftFlipperPressed = SDL_FALSE;
    SDL_bool rightFlipperJustPressed = SDL_FALSE;
    SDL_bool rightFlipperPressed = SDL_FALSE;
    SDL_bool lmbPressed = SDL_FALSE;

    while(!quit) {
        leftFlipperJustPressed = SDL_FALSE;
        rightFlipperJustPressed = SDL_FALSE;

        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                quit = SDL_TRUE;
            }
            if(event.type == SDL_KEYUP){
                switch(event.key.keysym.sym){
                    case SDLK_LEFT:
                        cPoly_1.pos.x -= 30;
                        break;
                    case SDLK_RIGHT:
                        cPoly_1.pos.x += 30;
                        break;
                    case SDLK_q:
                        cPoly_1.rotationRads += .2;
                        break;
                    case SDLK_e:
                        cPoly_1.rotationRads -= .2;
                        break;
                    case SDLK_f:
                        cPoly_1.flip = !cPoly_1.flip;
                    case SDLK_RSHIFT:
                        rightFlipperPressed = SDL_FALSE;
                        lerpSet(&cPoly_1.lerp,0,cPoly_1.rotationRads, 500);
                        //cPoly_1.rotationRads = 0;
                        break;
                    case SDLK_LSHIFT:
                        leftFlipperPressed = SDL_FALSE;
                        lerpSet(&cPoly_2.lerp,0,cPoly_2.rotationRads, 500);
                        cPoly_2.rotationRads = 0;
                        break;
                    default:
                        break;
                    
                }
                
            } 
            else if(event.type == SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    case SDLK_RSHIFT:
                        if(!rightFlipperPressed)
                            rightFlipperJustPressed = SDL_TRUE;
                        rightFlipperPressed = SDL_TRUE;
                        //cPoly_1.rotationRads = -1;
                        break;
                    case SDLK_LSHIFT:
                        if(!leftFlipperPressed)
                            leftFlipperJustPressed = SDL_TRUE;
                        leftFlipperJustPressed = SDL_TRUE;
                        lerpSet(&cPoly_2.lerp, -1, cPoly_2.rotationRads, 500 );
                        break;
                }
            }
        }

        // flippers presed. Set the lerp
        if(leftFlipperJustPressed) {
            lerpSet(&cPoly_2.lerp, -1, cPoly_1.rotationRads, 500 );
        }
        else if(rightFlipperJustPressed){
            lerpSet(&cPoly_1.lerp, -1, cPoly_1.rotationRads, 500 );
        }

        // process the lerp
        if(cPoly_1.lerp.remainingTime > 0.0f){
            interpolate(&cPoly_1.lerp, 1);
            cPoly_1.rotationRads = cPoly_1.lerp.value;
        }

        if(cPoly_2.lerp.remainingTime > 0.0f){
            interpolate(&cPoly_2.lerp, 1);
            cPoly_2.rotationRads = cPoly_2.lerp.value;
        }
        

        // position
        for(int i = 0; i < 7; i++){
            cPoly_1.trfmPts[i] = v2d_add(cPoly_1.modelPts[i], cPoly_1.pos);
        }
        for(int i = 0; i < 7; i++){
            cPoly_2.trfmPts[i] = v2d_add(cPoly_2.modelPts[i], cPoly_2.pos);
        }

        // rotation
        float s = sin(cPoly_1.rotationRads);
        float c = cos(cPoly_1.rotationRads);
        for(int i = 0; i < 7; i++){
            v2d pivotPt = cPoly_1.pos;
            v2d subtracted = v2d_sub(cPoly_1.trfmPts[i], pivotPt);
            v2d_f p;
            p.x = subtracted.x;
            p.y = subtracted.y;

            float xnew = p.x * c - p.y * s;
            float ynew = p.x * s + p.y * c;

            p.x = xnew + pivotPt.x;
            p.y = ynew + pivotPt.y;

            cPoly_1.trfmPts[i].x = p.x;
            cPoly_1.trfmPts[i].y = p.y;
            
        }

        s = sin(cPoly_2.rotationRads);
        c = cos(cPoly_2.rotationRads);
        for(int i = 0; i < 7; i++){
            v2d pivotPt = cPoly_2.pos;
            v2d subtracted = v2d_sub(cPoly_2.trfmPts[i], pivotPt);
            v2d_f p;
            p.x = subtracted.x;
            p.y = subtracted.y;

            float xnew = p.x * c - p.y * s;
            float ynew = p.x * s + p.y * c;

            p.x = xnew + pivotPt.x;
            p.y = ynew + pivotPt.y;

            cPoly_2.trfmPts[i].x = p.x;
            cPoly_2.trfmPts[i].y = p.y;
            
        }

        //flip
        if(cPoly_1.flip){
            for(int i = 0; i < lenDarr(cPoly_1.trfmPts);i++){
                cPoly_1.trfmPts[i].x = cPoly_1.pos.x + (cPoly_1.pos.x - cPoly_1.trfmPts[i].x);
            }
        }
        if(cPoly_2.flip){
            for(int i = 0; i < lenDarr(cPoly_2.trfmPts);i++){
                cPoly_2.trfmPts[i].x = cPoly_2.pos.x + (cPoly_2.pos.x - cPoly_2.trfmPts[i].x);
            }
        }
        
        
        // render
        SDL_SetRenderDrawColor(renderer, 20,20,20,255);
        SDL_RenderClear(renderer);

        for(int i = 0; i < 7; i++){
            SDL_SetRenderDrawColor(renderer, 255,0,0,255);
            SDL_RenderDrawLine(renderer, cPoly_1.trfmPts[i].x, cPoly_1.trfmPts[i].y, cPoly_1.trfmPts[(i+1) % 7].x, cPoly_1.trfmPts[(i+1)%7].y);
            SDL_SetRenderDrawColor(renderer, 100,100,100,255);
            drawCross(renderer, cPoly_1.trfmPts[i].x, cPoly_1.trfmPts[i].y, 5 );

            SDL_SetRenderDrawColor(renderer, 0,255,0,255);
            SDL_RenderDrawLine(renderer, cPoly_2.trfmPts[i].x, cPoly_2.trfmPts[i].y, cPoly_2.trfmPts[(i+1) % 7].x, cPoly_2.trfmPts[(i+1)%7].y);
            SDL_SetRenderDrawColor(renderer, 100,100,100,255);
            drawCross(renderer, cPoly_2.trfmPts[i].x, cPoly_2.trfmPts[i].y, 5 );

        }
        SDL_SetRenderDrawColor(renderer, 100,100,100,255);
        drawCross(renderer, cPoly_1.pos.x, cPoly_1.pos.y, 7 );
        SDL_RenderPresent(renderer);

    }

    exit(0);
}