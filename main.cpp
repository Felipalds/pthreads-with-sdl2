#include <iostream>
#include <SDL2/SDL.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

using namespace std;

void MidpointCircleAlgorithm(
    SDL_Renderer *renderer, 
    int circleCenterX, 
    int circleCenterY, 
    int circleRadius
) {
    cout << "Rendering the circle with Midpoint\n";
    const int diameter = circleRadius * 2;
    int x = circleRadius - 1;
    int y = 0;
    int tx = 1; 
    int ty = 1;
    int error = tx - diameter;

    while(x >= y) {
        cout << x << " " << y << endl;
        cout << error << endl;

        SDL_RenderDrawPoint(renderer, circleCenterX + x, circleCenterY - y);
        SDL_RenderDrawPoint(renderer, circleCenterX + x, circleCenterY + y);
        SDL_RenderDrawPoint(renderer, circleCenterX - x, circleCenterY - y);
        SDL_RenderDrawPoint(renderer, circleCenterX - x, circleCenterY + y);
        SDL_RenderDrawPoint(renderer, circleCenterX + y, circleCenterY - x);
        SDL_RenderDrawPoint(renderer, circleCenterX + y, circleCenterY + x);
        SDL_RenderDrawPoint(renderer, circleCenterX - y, circleCenterY - x);
        SDL_RenderDrawPoint(renderer, circleCenterX - y, circleCenterY + x);
        if(error <= 0) {
            ++y;
            error += ty;
            ty += 2;
        }
        if(error > 0){
            --x;
            tx += 2;
            error += (tx-diameter);
        }
    }
}

void DrawCircle(SDL_Renderer *renderer) {
    const int circleCenterX = WINDOW_WIDTH / 2;
    const int circleCenterY = WINDOW_HEIGHT / 2;
    const int circleRadius = std::min(WINDOW_WIDTH, WINDOW_HEIGHT) / 2;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(
        renderer,
        circleCenterX - circleRadius,
        circleCenterY,
        circleCenterX + circleRadius,
        circleCenterY
    );
    SDL_RenderDrawLine(
        renderer,
        circleCenterX,
        circleCenterY - circleRadius,
        circleCenterX,
        circleCenterY + circleRadius
    );
    
    MidpointCircleAlgorithm(renderer, circleCenterX, circleCenterY, circleRadius);
}

int main(int argc, char *argv[]){
    
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }
    
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    
    window = SDL_CreateWindow(
        "PTHREADS and SDL2 for OS!", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if(window == nullptr) {
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );
    if(renderer == nullptr) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool quit = false;
    while(!quit) {
        SDL_Event event;
        while(SDL_PollEvent(&event) != 0) {
            if(event.type == SDL_QUIT) {
                quit = true;
            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderClear(renderer);

        DrawCircle(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}