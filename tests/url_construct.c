#include <stdio.h>
#include <string.h>

#define URL_BUFFER_SIZE 1024

#define X 20
#define Y 40
#define Z 10

int main(void) {
  char url[URL_BUFFER_SIZE];
  snprintf(url, URL_BUFFER_SIZE, "https://tile.openstreetmap.org/%d/%d/%d", X, Y, Z);

  printf("URL = %s\n", url);

  return 0;
}

