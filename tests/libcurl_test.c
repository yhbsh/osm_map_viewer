#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (curl) {
    char url[1024];
    snprintf(url, 1024, "https://tile.openstreetmap.org/%d/%d/%d.png", 13, 5156, 3515);

    printf("url = %s\n", url);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.64.1");

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

    curl_easy_cleanup(curl);
  }

  return 0;
}
