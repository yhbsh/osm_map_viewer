#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

#define WIN_HEIGHT 600
#define WIN_WIDTH 800

typedef struct {
    int w, h;
    SDL_Texture *texture;
} Scene;

typedef struct {
    int x, y;
    Uint8 up, down, left, right;
} Camera;

int main(void) {
    const char *scene_filepath = "assets/globe.jpg";
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Camera", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = IMG_LoadTexture(renderer, scene_filepath);
    const Uint8 *keyboard = SDL_GetKeyboardState(NULL);

    Scene scene = {.texture = texture};
    Camera camera = {0};

    SDL_Event event;
    Uint8 quit = 0;
    while (quit == 0) {
        SDL_SetRenderDrawColor(renderer, 40, 100, 150, 255);
        SDL_RenderClear(renderer);

        SDL_QueryTexture(texture, NULL, NULL, &scene.w, &scene.h);
        SDL_Rect scene_rect = {.x = camera.x, .y = camera.y, .w = scene.w, .h = scene.h};
        SDL_RenderCopy(renderer, scene.texture, NULL, &scene_rect);

        while (SDL_PollEvent(&event)) {
            quit = keyboard[SDL_SCANCODE_Q];
            if (quit == 1)
                break;

            camera.up = keyboard[SDL_SCANCODE_UP];
            camera.down = keyboard[SDL_SCANCODE_DOWN];
            camera.left = keyboard[SDL_SCANCODE_LEFT];
            camera.right = keyboard[SDL_SCANCODE_RIGHT];
        }

        if (camera.up) {
            camera.y -= 20;
            if (camera.y <= WIN_HEIGHT - scene.h) {
                camera.y = WIN_HEIGHT - scene.h;
            }
        }

        if (camera.down) {
            camera.y += 20;
            if (camera.y >= 0) {
                camera.y = 0;
            }
        }

        if (camera.left) {
            camera.x -= 20;
            if (camera.x <= WIN_WIDTH - scene.w) {
                camera.x = WIN_WIDTH - scene.w;
            }
        }

        if (camera.right) {
            camera.x += 20;
            if (camera.x >= 0) {
                camera.x = 0;
            }
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
