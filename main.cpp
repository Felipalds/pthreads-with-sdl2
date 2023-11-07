#include <iostream>
#include <SDL2/SDL.h>
#include <random>
#include <thread>
#include <chrono>
#include <vector>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int THREAD_AMOUNT = 4;
int IN = 0;
int N = 1000000;

using namespace std;

typedef struct Args {
    SDL_Renderer *renderer;
    int points;
    SDL_Color color;
    SDL_Rect quadrant;
    sem_t* semaphore;
} Args;


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

void DrawPoint(SDL_Renderer *renderer, int x, int y) {
    SDL_Rect rect = { x, y, 4, 4 };
    SDL_RenderFillRect(renderer, &rect);
}

bool verifyPI(SDL_Point p) {
    // se distância do ponto até o centro for maior que o raio, está fora!
    const int distance = sqrt((WINDOW_WIDTH/2)*(WINDOW_WIDTH/2) -(p.x * p.x) + (WINDOW_HEIGHT/2)*(WINDOW_HEIGHT/2)-(p.y * p.y));
    const int circleRadius = std::min(WINDOW_WIDTH, WINDOW_HEIGHT) / 2;
    return distance <= circleRadius;
}


void* DrawRandomPoints(void* _args){
    Args* args = (Args*)_args;
    SDL_Color color = args->color;
    SDL_Rect quadrant = args->quadrant;
    int npoints = args->points;

    sem_wait(args->semaphore);
    std::random_device rd;
    std::mt19937 gen(rd());
    sem_post(args->semaphore);

    std::uniform_int_distribution<> disx(quadrant.x, quadrant.x + quadrant.w);
    std::uniform_int_distribution<> disy(quadrant.y, quadrant.y + quadrant.h);

    for (int c = 0; c < npoints; c++) {
        sem_wait(args->semaphore);
        SDL_Point p = { disx(gen), disy(gen) };
        SDL_SetRenderDrawColor(args->renderer, color.r, color.g, color.b, color.a);
        DrawPoint(args->renderer, p.x, p.y);
        IN += verifyPI(p);
        sem_post(args->semaphore);
    }
    cout << "ended" << endl;

    return NULL;
}

int main(int argc, char *argv[]){
    sem_t semaphore;
    sem_init(&semaphore, 0, 1);
    cout << "Semaphore created!" << endl;

    SDL_Color color1 = {255, 0, 0, 255};
    SDL_Color color2 = {0, 255, 0, 255};
    SDL_Color color3 = {0, 0, 255, 255};
    SDL_Color color4 = {0, 0, 0, 255};
    int quad = WINDOW_WIDTH / 2;
    SDL_Rect q1 = {0, 0, quad, quad};
    SDL_Rect q2 = {400, 0, quad, quad};
    SDL_Rect q3 = {0, 400, quad, quad};
    SDL_Rect q4 = {400, 400, quad, quad};

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

    Args arg1 = {renderer, N/THREAD_AMOUNT, color1, q1, &semaphore};
    Args arg2 = {renderer, N/THREAD_AMOUNT, color2, q2, &semaphore};
    Args arg3 = {renderer, N/THREAD_AMOUNT, color3, q3, &semaphore};
    Args arg4 = {renderer, N/THREAD_AMOUNT, color4, q4, &semaphore};
    
    std::vector<Args> args_vector;
    args_vector.push_back(arg1);
    args_vector.push_back(arg2);
    args_vector.push_back(arg3);
    args_vector.push_back(arg4);

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderClear(renderer);
    DrawCircle(renderer);

    pthread_t threads [THREAD_AMOUNT];
    for (int t = 0; t < THREAD_AMOUNT; t++) {
        cout << "Creating a Thread! " << t << endl;
        int rc = pthread_create(&threads[t], NULL, DrawRandomPoints, (void*)&args_vector[t]);

        if (rc) {
            cerr << "Error creating a thread number " << t << endl;
            exit(-1);
        }
    }

    for(int t = 0; t < THREAD_AMOUNT; t++) {
        cout << "Joining a thread " << t << endl;
        pthread_join(threads[t], NULL);
    }
    SDL_RenderPresent(renderer);

    const float PI = float(IN) / float(N) * 4.0;
    cout << PI << endl;
    
    SDL_Event event;
    for (;;) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                goto fim;
            }
        }
    }
fim:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
