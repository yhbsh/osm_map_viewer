#include <math.h>
#include <stdio.h>

static int lng2tilex(double lon, int z) { return (int)(floor((lon + 180.0) / 360.0 * (1 << z))); }
static int lat2tiley(double lat, int z) { return (int)(floor((1.0 - asinh(tan(lat * M_PI / 180.0)) / M_PI) / 2.0 * (1 << z))); }

int main(void) {
    printf("%d %d\n", lng2tilex(0, 14), lat2tiley(0, 14));

    return 0;
}
