#include "perlin.h"
#define MAX_SIZE 256;

perlin::perlin()
{
    srand(time(NULL));

    permu = new int[256];
    Gx = new float[256];
    Gy = new float[256];
    Gz = new float[256];

    for(int i = 0; i < MAX_SIZE; i++)
    {
        permu[i] = i;
        Gx[i] = (float(rand()) / (MAX_RAND/2)) - 1.0f;
        Gy[i] = (float(rand()) / (MAX_RAND/2)) - 1.0f;
        Gz[i] = (float(rand()) / (MAX_RAND/2)) - 1.0f;
    }
}
