#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define WIDTH 640
#define HEIGHT 480
#define MAX_ITERATIONS 1000
#define ESCAPE_RADIUS 2

int main()
{
    int x, y, i;
    double complex z, c;
    int color;
    FILE *output = fopen("mandelbrot.ppm", "w");
    fprintf(output, "P3\n%d %d\n255\n", WIDTH, HEIGHT);

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            c = (3.5*(x-WIDTH/2.0)/(0.5*WIDTH)) + (2.0*(y-HEIGHT/2.0)/(0.5*HEIGHT))*I;
            z = 0;
            color = 0;
            for (i = 0; i < MAX_ITERATIONS; i++) {
                z = z*z + c;
                if (cabs(z) > ESCAPE_RADIUS) {
                    color = i;
                    break;
                }
            }
            fprintf(output, "%d %d %d ", color % 256, color % 256, color % 256);
        }
        fprintf(output, "\n");
    }
    fclose(output);
    return 0;
}
