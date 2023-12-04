#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <curl/curl.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define URL_BUF_SIZE 1024

#define ZOOM 7
#define LAT 24.719634
#define LNG 46.624539
#define WIDTH 800
#define HEIGHT 800

static int long2tilex(double lon, int z) { return (int)(floor((lon + 180.0) / 360.0 * (1 << z))); }

static int lat2tiley(double lat, int z) { return (int)(floor((1.0 - asinh(tan(lat * M_PI / 180.0)) / M_PI) / 2.0 * (1 << z))); }

typedef struct {
<<<<<<< HEAD
  char *ptr;
  size_t len;
} string;

static void init_string(string *s) {
  s->len = 0;
  s->ptr = malloc(s->len + 1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(1);
  }
  s->ptr[0] = '\0';
}

static size_t write_callback(void *data, size_t size, size_t nmemb, void *user_data) {
  size_t real_size = size * nmemb;
  string *s = (string *)user_data;
=======
  char *ptr;
  size_t len;
} buffer;

static size_t write_callback(void *data, size_t size, size_t nmemb, buffer *s) {
  size_t real_size = size * nmemb;
>>>>>>> refs/remotes/origin/main

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

<<<<<<< HEAD
static string curl_get_png_data(const char *url) {
  CURL *curl = curl_easy_init();
=======
static buffer curl_tile_from_url(const char *url) {
  CURL *curl = curl_easy_init();
>>>>>>> refs/remotes/origin/main

  if (!curl) {
    fprintf(stderr, "[ERROR]: curl_easy_init() failed: %s\n", strerror(errno));
    exit(1);
  }

<<<<<<< HEAD
  string s;
  init_string(&s);
=======
  buffer s = {0};
>>>>>>> refs/remotes/origin/main

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
  const int tx = long2tilex(LNG, ZOOM);  // convert longiture to tile x coordinate
  const int ty = lat2tiley(LAT, ZOOM);   // convert latitude to tile y coordinate

<<<<<<< HEAD
  char url[URL_BUF_SIZE];
  snprintf(url, URL_BUF_SIZE, "https://tile.openstreetmap.org/%d/%d/%d.png", ZOOM, tx, ty);

  string png_data = curl_get_png_data(url);
=======
  char url[URL_BUF_SIZE];
  char url2[URL_BUF_SIZE];
  snprintf(url, URL_BUF_SIZE, "https://tile.openstreetmap.org/%d/%d/%d.png", ZOOM, tx, ty);
  snprintf(url2, URL_BUF_SIZE, "https://tile.openstreetmap.org/%d/%d/%d.png", ZOOM, tx + 20, ty + 20);

  buffer tile_data = curl_tile_from_url(url);
  buffer tile_data2 = curl_tile_from_url(url2);
>>>>>>> refs/remotes/origin/main

  // Initialisation
  SDL_Init(SDL_INIT_VIDEO);
  IMG_Init(IMG_INIT_PNG);

<<<<<<< HEAD
  SDL_Window *window = SDL_CreateWindow("Map Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RWops *rw = SDL_RWFromConstMem(png_data.ptr, png_data.len);
  SDL_Texture *texture = IMG_LoadTexture_RW(renderer, rw, 1);

  // Event loop
  SDL_Event event;
  bool quit = false;
  while (!quit) {
    SDL_RenderClear(renderer);
    SDL_Rect rect = {};
    SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    while (SDL_PollEvent(&event) != 0) {
      quit = event.type == SDL_QUIT;
=======
  SDL_Window *window = SDL_CreateWindow("Map Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RWops *rw = SDL_RWFromConstMem(tile_data.ptr, tile_data.len);
  SDL_RWops *rw2 = SDL_RWFromConstMem(tile_data2.ptr, tile_data2.len);
  SDL_Texture *texture = IMG_LoadTexture_RW(renderer, rw, 1);
  SDL_Texture *texture2 = IMG_LoadTexture_RW(renderer, rw2, 1);

  // Event loop
  SDL_Event event;
  bool quit = false;

  SDL_Rect rect = {};
  SDL_Rect rect2 = {};

  while (!quit) {
    SDL_RenderClear(renderer);
    SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
    SDL_QueryTexture(texture2, NULL, NULL, &rect2.w, &rect2.h);
    rect2.x += rect.w;
    rect2.y += rect.h;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_RenderCopy(renderer, texture2, NULL, &rect2);

    while (SDL_PollEvent(&event) != 0) {
      quit = event.type == SDL_QUIT;
    }

    SDL_RenderPresent(renderer);
>>>>>>> refs/remotes/origin/main
    }

    SDL_RenderPresent(renderer);
  }

<<<<<<< HEAD
  // Cleanup
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  IMG_Quit();
  SDL_Quit();
=======
free(tile_data.ptr);
tile_data.ptr = NULL;
tile_data.len = 0;
>>>>>>> refs/remotes/origin/main

  free(png_data.ptr);
  png_data.ptr = NULL;
  png_data.len = 0;

  return 0;
}
