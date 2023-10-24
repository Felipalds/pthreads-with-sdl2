#include <iostream>
#include <SDL2/SDL.h>
#include <random>
#include <thread>
#include <chrono>
#include <vector>
#include <pthread.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int THREAD_AMOUNT = 4;
int IN;
int N = 300;

using namespace std;

typedef struct Point {
    int x;
    int y;
} Point;

typedef struct Color {
    int r;
    int g;
    int b;
} Color;

typedef struct Quadrant {
    int x0;
    int x1;
    int y0;
    int y1;
} Quadrant;

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

void DrawPoint(SDL_Renderer *renderer, int x, int y){
    const int radius = 5;

    for(int dx = -radius; dx <= radius; dx++) {
        for(int dy = -radius; dy <= radius; dy++) {
            if(dx * dx + dy * dy <= radius * radius) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}



int getRandom(int maxX, int maxY) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(maxX, maxY);
    return dis(gen);
}

void animateForVitor(std::vector<Point>& points) {
    for(Point& point : points) {
        point.x += 10;
        point.y += 10;
    }
}

void verifyPI(Point p) {
    // se distância do ponto até o centro for maior que o raio, está fora!
    const int distance = sqrt((WINDOW_WIDTH/2)*(WINDOW_WIDTH/2) -(p.x * p.x) + (WINDOW_HEIGHT/2)*(WINDOW_HEIGHT/2)-(p.y * p.y));
    const int circleRadius = std::min(WINDOW_WIDTH, WINDOW_HEIGHT) / 2;


    printf("distance: %d \n", distance);
    if(distance <= circleRadius) {
        IN++;
    }
}

void DrawRandomPoints(SDL_Renderer *renderer, std::vector<Point>& points, Color color, Quadrant quadrant){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

    Point p;
    p.x = getRandom(quadrant.x0, quadrant.x1);
    p.y = getRandom(quadrant.y0, quadrant.y1);
    cout << p.x << " " << p.y << endl;
    points.push_back(p);

    for(const Point& point : points) {
        DrawPoint(renderer, point.x, point.y);
    }

    verifyPI(p);
}



int main(int argc, char *argv[]){

    std::vector<Point> points1;
    std::vector<Point> points2;
    std::vector<Point> points3;
    std::vector<Point> points4;
    Color color1 = {255, 0, 0};
    Color color2 = {0, 255, 0};
    Color color3 = {0, 0, 255};
    Color color4 = {0, 0, 0};
    Quadrant q1 = {0, 400, 0, 400};
    Quadrant q2 = {400, 800, 0, 400};
    Quadrant q3 = {0, 400, 400, 800};
    Quadrant q4 = {400, 800, 400, 800};

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
    for(int x = 0; x < N; x++) {
        SDL_Event event;
        while(SDL_PollEvent(&event) != 0) {
            if(event.type == SDL_QUIT) {
                quit = true;
            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderClear(renderer);
        DrawCircle(renderer);
        //animateForVitor(points1);
        // animateForVitor(points2);
        // animateForVitor(points3);
        // animateForVitor(points4);

        
        //pthreads!!!
        DrawRandomPoints(renderer, points1, color1, q1);
        DrawRandomPoints(renderer, points2, color2, q2);
        DrawRandomPoints(renderer, points3, color3, q3);
        DrawRandomPoints(renderer, points4, color4, q4);
        SDL_RenderPresent(renderer);

        //std::this_thread::sleep_for(std::chrono::seconds(1));

        const float PI = float(IN)/float(N);
        cout << PI << endl;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}