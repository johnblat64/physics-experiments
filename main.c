#include <SDL2/SDL.h>
#include "v2d.h"
#include "util.h"
#include "darr.h"
#include "tween.h"
#include "shapes.h"


void flipModel(CollisionPolygon *cPoly) {
    for(int i = 0; i < lenDarr(cPoly->trfmPts); i++){
        cPoly->trfmPts[i] = v2d_f_scalar_mult(-1, cPoly->trfmPts[i]); 
    }
}

void polyDeepCopy(CollisionPolygon *src, CollisionPolygon *dest){
    dest->modelPts = initDarr(dest->modelPts, sizeof(v2d_f), capDarr(src->modelPts));
    dest->trfmPts = initDarr(dest->trfmPts, sizeof(v2d_f), capDarr(src->trfmPts));

    for(int i = 0; i < lenDarr(src->modelPts); i++){
        addDarr(dest->modelPts, src->modelPts[i], v2d_f);
        addDarr(dest->trfmPts, src->trfmPts[i], v2d_f);
    }

    dest->rotationRads = src->rotationRads;
    dest->pos = src->pos;
    dest->flip = src->flip;
    dest->lerp.step = src->lerp.step;
    dest->lerp.remainingTime = src->lerp.remainingTime;
    dest->lerp.value = src->lerp.value;

}

SDL_bool pointIntersectCircle(v2d_f pt, Circle circle){
    v2d_f ptToCirclev2d_f = v2d_f_sub(pt, circle.midPoint);
    float magPtToCircleCenter = mag_no_root_v2d_f(ptToCirclev2d_f);
    if(magPtToCircleCenter <= circle.radius*circle.radius){
        return SDL_TRUE;
    }
    return SDL_FALSE;
}

SDL_bool pointIntersectLine(v2d_f pt, LineSegment line){
    float magPtToLinePt1 = mag_v2d_f(v2d_f_sub(pt, line.pt1));
    float magPtToLinePt2 = mag_v2d_f(v2d_f_sub(pt, line.pt2));
    float magLine = mag_v2d_f(v2d_f_sub(line.pt1, line.pt2));
    float buffer = 0.1;
    if(magPtToLinePt1+magPtToLinePt2 >= magLine-buffer && magPtToLinePt1+magPtToLinePt2 <= magLine+buffer){
        return SDL_TRUE;
    }
    return SDL_FALSE;
}

SDL_bool lineIntersectCircle(LineSegment line, Circle circle){
    if(pointIntersectCircle(line.pt1, circle)){
        return SDL_TRUE;
    }
    else if(pointIntersectCircle(line.pt2, circle)) {
        return SDL_TRUE;
    }

    v2d_f linePt1ToCircle = v2d_f_sub(circle.midPoint, line.pt1);
    v2d_f linev2d_f = v2d_f_sub(line.pt2, line.pt1);
    float magLinev2d_f = mag_v2d_f(linev2d_f);
    v2d_f nomralizedLinev2d_f = v2d_f_unit(linev2d_f);

    //v2d_f linePt2ToCircle = v2d_f_sub(line.pt2, circle.midPoint);

    float dot = dot_product_v2d_f(linePt1ToCircle, nomralizedLinev2d_f);

    v2d_f ptOnVector = v2d_f_scalar_mult(dot, nomralizedLinev2d_f);
    v2d_f ptOnLine = v2d_f_add(ptOnVector, line.pt1);

    if(!pointIntersectLine(ptOnLine, line)){
        return SDL_FALSE;
    }

    float x = circle.midPoint.x - ptOnLine.x;
    float y = circle.midPoint.y - ptOnLine.y;
    if((x*x + y*y )<= (circle.radius*circle.radius)){
        return SDL_TRUE;
    }
    return SDL_FALSE;
    // if(pointIntersectLine(ptOnLine, line)){
    //     return SDL_TRUE;
    // }

    // return SDL_FALSE;

}

SDL_bool isPolygonCircleCollision(CollisionPolygon *cPoly, Circle *circle){
    unsigned int numVertices = lenDarr(cPoly->trfmPts);
    for(int i = 0;i < numVertices; i++){
        v2d_f pt1, pt2;
        pt1 = cPoly->trfmPts[i];
        pt2 = cPoly->trfmPts[(i+1)%numVertices];
        LineSegment polygonLineSegment = {pt1, pt2};
        //is either pt inside circle?
        // v2d_f pt1ToCircleCenter, pt2ToCircleCenter;
        // float magPolygonLineSegment, magPt1ToCircleCenter, magPt2ToCircleCenter;

        // pt1ToCircleCenter = v2d_f_sub(pt1, circle->midPoint);
        // pt2ToCircleCenter = v2d_f_sub(pt2, circle->midPoint);

        // magPt1ToCircleCenter = mag_no_root_v2d_f(pt1ToCircleCenter);
        // magPt2ToCircleCenter = mag_no_root_v2d_f(pt2ToCircleCenter);

        // // one of the points in the line segment are inside the circle
        // if(magPt1ToCircleCenter <= circle->radius*circle->radius){
        //     return SDL_TRUE;
        // }
        // if(magPt2ToCircleCenter <= circle->radius*circle->radius){
        //     return SDL_TRUE;
        // }
        if(lineIntersectCircle(polygonLineSegment, *circle)){
            return SDL_TRUE;
        }
        

        // the line's points are not in the circle
        // is closest point on line segment
        //polygonLineSegment = v2d_f_sub(pt1, pt2);
        // float lineDotCircle = dot_product_v2d_f(polygonLineSegment, circle->midPoint);
        // v2d_f closestPoint = (v2d_f){
        //     pt1.x + (lineDotCircle * (pt2.x-pt1.x)), 
        //     pt1.y + (lineDotCircle * (pt2.y-pt1.y))
        // };
        
        // v2d_f closestPointToPt1, closestPointToPt2;
        // closestPointToPt1 = v2d_f_sub(closestPoint, pt1);
        // closestPointToPt2 = v2d_f_sub(closestPoint, pt2);

        // float magClosestPointToPt1, magClosestPointToPt2;
        // magClosestPointToPt1 = mag_no_root_v2d_f(closestPointToPt1);
        // magClosestPointToPt2 = mag_no_root_v2d_f(closestPointToPt2);

        
        // magPolygonLineSegment = mag_no_root_v2d_f(polygonLineSegment);

        // float buffer = 0.1f;

        // if(magClosestPointToPt1+magClosestPointToPt2 >= magPolygonLineSegment-buffer && magClosestPointToPt1+magClosestPointToPt2 <= magPolygonLineSegment-buffer){
        //     return SDL_TRUE;
        // }

        

        


        

        
        // pt1ToCircleCenter = v2d_f_sub(pt1, circle->midPoint);
        // float distanceV1Squared = mag_no_root_v2d_f(pt1ToCircleCenter); 
        // pt2ToCircleCenter= v2d_f_sub(pt2, circle->midPoint);
        // float distanceV2Squared = mag_no_root_v2d_f(pt2ToCircleCenter);

        // polygonLineSegment = v2d_f_sub(pt1, pt2);
        // float lengthLineSquared = mag_no_root_v2d_f(polygonLineSegment);

        // float buffer = 0.1;

        // if(distanceV1Squared+distanceV2Squared >= lengthLineSquared-buffer && distanceV1Squared+distanceV2Squared <= lengthLineSquared-buffer){
        //     return SDL_TRUE;
        // }


    }
    return SDL_FALSE;
}

int
main() {
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Polygon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640,480, 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |SDL_RENDERER_PRESENTVSYNC );

    CollisionPolygon cPoly_1;
    cPoly_1.modelPts = initDarr(cPoly_1.modelPts, sizeof(v2d_f), 7);
    cPoly_1.trfmPts = initDarr(cPoly_1.trfmPts, sizeof(v2d_f), 7);
    addDarr(cPoly_1.modelPts, ((v2d_f){10,-20}), v2d_f);
    addDarr(cPoly_1.modelPts, ((v2d_f){-10,-20}), v2d_f);
    addDarr(cPoly_1.modelPts, ((v2d_f){-20,-5}), v2d_f);
    addDarr(cPoly_1.modelPts, ((v2d_f){-12,25}), v2d_f);
    addDarr(cPoly_1.modelPts, ((v2d_f){100,80}), v2d_f);
    addDarr(cPoly_1.modelPts, ((v2d_f){110,75}), v2d_f);
    addDarr(cPoly_1.modelPts, ((v2d_f){110,68}), v2d_f);
    for(int i = 0; i < lenDarr(cPoly_1.modelPts); i++){
        addDarr(cPoly_1.trfmPts, ((v2d_f){0,0}), v2d_f);
    }
    cPoly_1.rotationRads = 0.0f;
    cPoly_1.pos = (v2d_f){200,200};
    cPoly_1.flip = SDL_FALSE;
    cPoly_1.lerp = lerpInit(0,0,0);

    CollisionPolygon cPoly_2;
    polyDeepCopy(&cPoly_1, &cPoly_2);

    Circle circle_1;
    circle_1.radius = 40;
    circle_1.midPoint = (v2d_f){300,150};


    // v2d_f relPts[7];
    // relPts[0] = (v2d_f){10,-20};
    // relPts[1] = (v2d_f){-10,-20};
    // relPts[2] = (v2d_f){-20,-5};
    // relPts[3] = (v2d_f){-12,25};
    // relPts[4] = (v2d_f){100,80};
    // relPts[5] = (v2d_f){110,75};
    // relPts[6] = (v2d_f){110,68};


    v2d_f trfPts[7];

    

    SDL_bool quit = SDL_FALSE;
    SDL_Event event;
    SDL_bool leftFlipperJustPressed = SDL_FALSE;
    SDL_bool leftFlipperPressed = SDL_FALSE;
    SDL_bool rightFlipperJustPressed = SDL_FALSE;
    SDL_bool rightFlipperPressed = SDL_FALSE;
    SDL_bool lmbPressed = SDL_FALSE;

    float max_delta_time = 1 / 60.0;
    float previous_frame_ticks = SDL_GetTicks() / 1000.0;

    while(!quit) {
        float current_frame_ticks = SDL_GetTicks() / 1000.0;
        float deltaTime = current_frame_ticks - previous_frame_ticks;
        previous_frame_ticks = current_frame_ticks;
        // adjust for any pauses, debugging breaks, etc
        deltaTime = deltaTime < max_delta_time ?  deltaTime : max_delta_time;

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
                    case SDLK_UP:
                        cPoly_1.pos.y -= 30;
                        break;
                    case SDLK_DOWN:
                        cPoly_1.pos.y += 30;
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
                        lerpSet(&cPoly_1.lerp,0,cPoly_1.rotationRads, 1 );
                        //cPoly_1.rotationRads = 0;
                        break;
                    case SDLK_LSHIFT:
                        leftFlipperPressed = SDL_FALSE;
                        lerpSet(&cPoly_2.lerp,0,cPoly_2.rotationRads, .033 );
                        //cPoly_2.rotationRads = 0;
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
                        //lerpSet(&cPoly_2.lerp, -1, cPoly_2.rotationRads, 500 );
                        break;
                }
            }
        }

        // flippers presed. Set the lerp
        if(leftFlipperJustPressed) {
            lerpSet(&cPoly_2.lerp, -1, cPoly_2.rotationRads, .033 );
        }
        else if(rightFlipperJustPressed){
            lerpSet(&cPoly_1.lerp, -1, cPoly_1.rotationRads, cPoly_1.lerp.remainingTime > 0.0f ? cPoly_1.lerp.remainingTime : 0.0f );
        }

        // process the lerp
        if(cPoly_1.lerp.remainingTime > 0.0f){
            interpolate(&cPoly_1.lerp, deltaTime);
            cPoly_1.rotationRads = cPoly_1.lerp.value;
        }

        if(cPoly_2.lerp.remainingTime > 0.0f){
            interpolate(&cPoly_2.lerp, deltaTime);
            cPoly_2.rotationRads = cPoly_2.lerp.value;
        }
        

        // position
        for(int i = 0; i < 7; i++){
            cPoly_1.trfmPts[i] = v2d_f_add(cPoly_1.modelPts[i], cPoly_1.pos);
        }
        for(int i = 0; i < 7; i++){
            cPoly_2.trfmPts[i] = v2d_f_add(cPoly_2.modelPts[i], cPoly_2.pos);
        }

        // rotation
        float s = sin(cPoly_1.rotationRads);
        float c = cos(cPoly_1.rotationRads);
        for(int i = 0; i < 7; i++){
            v2d_f pivotPt = cPoly_1.pos;
            v2d_f subtracted = v2d_f_sub(cPoly_1.trfmPts[i], pivotPt);
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
            v2d_f pivotPt = cPoly_2.pos;
            v2d_f subtracted = v2d_f_sub(cPoly_2.trfmPts[i], pivotPt);
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
        
        drawGrid(renderer, 0,0,100,100,10, (SDL_Color){75,75,75,255} );

        if(isPolygonCircleCollision(&cPoly_1, &circle_1)) {
            SDL_SetRenderDrawColor(renderer, 0,255,100,255);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 200,0,0,255);
        }
        drawCircle(renderer, circle_1);

        SDL_SetRenderDrawColor(renderer, 255,0,0,255);
        drawPolygon(renderer, &cPoly_1);
        SDL_SetRenderDrawColor(renderer, 0,255,0,255);
        drawPolygon(renderer, &cPoly_2);

        // for(int i = 0; i < 7; i++){
        //     SDL_SetRenderDrawColor(renderer, 100,100,100,255);
        //     drawCross(renderer, cPoly_1.trfmPts[i].x, cPoly_1.trfmPts[i].y, 5 );

            
        //     SDL_RenderDrawLine(renderer, cPoly_2.trfmPts[i].x, cPoly_2.trfmPts[i].y, cPoly_2.trfmPts[(i+1) % 7].x, cPoly_2.trfmPts[(i+1)%7].y);
        //     SDL_SetRenderDrawColor(renderer, 100,100,100,255);
        //     drawCross(renderer, cPoly_2.trfmPts[i].x, cPoly_2.trfmPts[i].y, 5 );

        // }
        SDL_SetRenderDrawColor(renderer, 100,100,100,255);
        drawCross(renderer, cPoly_1.pos.x, cPoly_1.pos.y, 7 );
        SDL_RenderPresent(renderer);

    }

    exit(0);
}