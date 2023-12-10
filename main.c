#include "SDL2/SDL_rwops.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <curl/curl.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#define URL_BUF_SIZE 1024
#define TILE_COUNT 9

#define ZOOM 13
#define LAT 24.719634
#define LNG 46.624539
#define WIDTH 800
#define HEIGHT 800

static int long2tilex(double lon, int z) { return (int)(floor((lon + 180.0) / 360.0 * (1 << z))); }
static int lat2tiley(double lat, int z) { return (int)(floor((1.0 - asinh(tan(lat * M_PI / 180.0)) / M_PI) / 2.0 * (1 << z))); }

typedef struct {
    char *ptr;
    size_t len;
} buffer;

typedef struct {
    SDL_Texture *texture;
    SDL_Rect rect;
} Tile;

static size_t write_callback(void *data, size_t size, size_t nmemb, void *user_data) {
    size_t real_size = size * nmemb;
    buffer *s = (buffer *)user_data;

    size_t new_size = s->len + real_size;
    s->ptr = realloc(s->ptr, new_size + 1);

    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(1);
    }
    memcpy(s->ptr + s->len, data, real_size);
    s->ptr[new_size] = '\0';
    s->len = new_size;

    return real_size;
}

static buffer tile_from_url(const char *url) {
    CURL *curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "[ERROR]: curl_easy_init() failed: %s\n", strerror(errno));
        exit(1);
    }

    buffer s = {0};

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&s);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.64.1");

    CURLcode code = curl_easy_perform(curl);

    if (code != CURLE_OK) {
        fprintf(stderr, "[ERROR]: curl_easy_perform() failed: %s\n", curl_easy_strerror(code));
        exit(1);
    }

    curl_easy_cleanup(curl);

    return s;
}

int main(void) {
    const int tx = long2tilex(LNG, ZOOM); // convert longiture to tile x coordinate
    const int ty = lat2tiley(LAT, ZOOM);  // convert latitude to tile y coordinate

    char tiles_urls[TILE_COUNT][URL_BUF_SIZE];
    snprintf(tiles_urls[0], URL_BUF_SIZE, "https://tile.openstreetmap.org/%d/%d/%d.png", ZOOM, tx, ty);
    snprintf(tiles_urls[1], URL_BUF_SIZE, "https://tile.openstreetmap.org/%d/%d/%d.png", ZOOM, tx + 1, ty);
    snprintf(tiles_urls[2], URL_BUF_SIZE, "https://tile.openstreetmap.org/%d/%d/%d.png", ZOOM, tx - 1, ty);
    snprintf(tiles_urls[3], URL_BUF_SIZE, "https://tile.openstreetmap.org/%d/%d/%d.png", ZOOM, tx, ty + 1);
    snprintf(tiles_urls[4], URL_BUF_SIZE, "https://tile.openstreetmap.org/%d/%d/%d.png", ZOOM, tx, ty - 1);
    snprintf(tiles_urls[5], URL_BUF_SIZE, "https://tile.openstreetmap.org/%d/%d/%d.png", ZOOM, tx + 1, ty + 1);
    snprintf(tiles_urls[6], URL_BUF_SIZE, "https://tile.openstreetmap.org/%d/%d/%d.png", ZOOM, tx - 1, ty + 1);
    snprintf(tiles_urls[7], URL_BUF_SIZE, "https://tile.openstreetmap.org/%d/%d/%d.png", ZOOM, tx + 1, ty - 1);
    snprintf(tiles_urls[8], URL_BUF_SIZE, "https://tile.openstreetmap.org/%d/%d/%d.png", ZOOM, tx - 1, ty - 1);

    buffer buffers[TILE_COUNT];
    for (int i = 0; i < TILE_COUNT; i++) {
        buffers[i] = tile_from_url(tiles_urls[i]);
    }

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    SDL_Window *window = SDL_CreateWindow("Map Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *textures[TILE_COUNT];
    SDL_RWops *rw;
    for (int i = 0; i < TILE_COUNT; i++) {
        rw = SDL_RWFromConstMem(buffers[i].ptr, buffers[i].len);
        textures[i] = IMG_LoadTexture_RW(renderer, rw, 0);
    }

    Tile tiles[TILE_COUNT];
    int tileWidth = WIDTH / 3;
    int tileHeight = HEIGHT / 3;

    // Initialize the SDL_Rects for each tile
    for (int i = 0; i < TILE_COUNT; ++i) {
        tiles[i].texture = textures[i];
        tiles[i].rect.w = tileWidth;
        tiles[i].rect.h = tileHeight;

        // Determine the position of each tile
        // The tiles are ordered as follows: center, right, left, down, up, down-right, down-left, up-right, up-left
        int dx[9] = {0, 1, -1, 0, 0, 1, -1, 1, -1};
        int dy[9] = {0, 0, 0, 1, -1, 1, 1, -1, -1};

        tiles[i].rect.x = (dx[i] + 1) * tileWidth;
        tiles[i].rect.y = (dy[i] + 1) * tileHeight;
    }

    SDL_Event event;
    bool quit = false;

    while (!quit) {
        SDL_RenderClear(renderer);

        // Render all tiles
        for (int i = 0; i < TILE_COUNT; ++i) {
            SDL_RenderCopy(renderer, tiles[i].texture, NULL, &tiles[i].rect);
        }

        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_RenderPresent(renderer);
    }

    // Cleanup textures and buffers
    for (int i = 0; i < TILE_COUNT; ++i) {
        SDL_DestroyTexture(textures[i]);
        free(buffers[i].ptr);
    }
    SDL_DestroyRenderer(renderer);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
