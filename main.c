#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <curl/curl.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ZOOM 13
#define LAT 24.719634
#define LNG 46.624539
#define WIDTH 800
#define HEIGHT 600

static char url[200] = "https://tile.openstreetmap.org/";

static int long2tilex(double lon, int z) { return (int)(floor((lon + 180.0) / 360.0 * (1 << z))); }

static int lat2tiley(double lat, int z) {
  double latrad = lat * M_PI / 180.0;
  return (int)(floor((1.0 - asinh(tan(latrad)) / M_PI) / 2.0 * (1 << z)));
}

static const char *create_url(int x, int y, int z) {
  static const char base_url[] = "https://tile.openstreetmap.org/";
  strcpy(url, base_url); // Reset the URL to base

  static char zoom[5], x_str[10], y_str[10];

  sprintf(zoom, "%d", z);
  sprintf(x_str, "%d", x);
  sprintf(y_str, "%d", y);

  strcat(url, zoom);
  strcat(url, "/");
  strcat(url, x_str);
  strcat(url, "/");
  strcat(url, y_str);
  strcat(url, ".png");

  return url;
}

// Callback function for writing data
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  char **response_ptr = (char **)userp;
  size_t response_length = *response_ptr ? strlen(*response_ptr) : 0;

  char *new_buffer = realloc(*response_ptr, response_length + realsize + 1);

  if (new_buffer == NULL) {
    // out of memory
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  *response_ptr = new_buffer;
  memcpy(&((*response_ptr)[response_length]), contents, realsize);
  (*response_ptr)[response_length + realsize] = '\0';

  return realsize;
}

size_t curl_get_image(const char *url, uint8_t **response) {
  curl_global_init(CURL_GLOBAL_DEFAULT);
  CURL *curl = curl_easy_init();

  if (!curl) {
    fprintf(stderr, "[ERROR]: could not initialize curl: %s\n", strerror(errno));
    exit(1);
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.64.1");

  CURLcode code = curl_easy_perform(curl);
  if (code != CURLE_OK) {
    fprintf(stderr, "[ERROR]: could not perform the request: %s\n", curl_easy_strerror(code));
    exit(1);
  }

  curl_easy_cleanup(curl);
  curl = NULL;
  curl_global_cleanup();

  return strlen((const char *)*response);
}

int main(void) {
  // const int tx = long2tilex(LNG, ZOOM);
  // const int ty = lat2tiley(LAT, ZOOM);
  // const char *url = create_url(tx, ty, ZOOM);

  // uint8_t *response = NULL;
  // size_t response_len = curl_get_image(url, &response);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "[ERROR]: could not initialize sdl: %s\n", SDL_GetError());
    return 1;
  }

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    fprintf(stderr, "[ERROR]: could not initialize sdl_image: %s\n", IMG_GetError());
    SDL_Quit();
    return 1;
  }

  // SDL_RWops *rw = SDL_RWFromConstMem(response, response_len);
  const char *image_path = "image.png";

  SDL_Window *window = SDL_CreateWindow("Map Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    fprintf(stderr, "[ERROR]: could not create sdl window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    fprintf(stderr, "[ERROR]: could not create sdl renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_Surface *surface = IMG_Load(image_path); // 1 means SDL will free the RWops for us
  if (surface == NULL) {
    fprintf(stderr, "[ERROR]: could not load image %s: %s\n", image_path, IMG_GetError());
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == NULL) {
    fprintf(stderr, "[ERROR]: could not create texture from surface %s: %s", image_path, SDL_GetError());
    return 1;
  }

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  // Cleanup
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();

  // free(response);
  // response = NULL;

  return 0;
}
