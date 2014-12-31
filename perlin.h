#ifndef PERLIN_H
#define PERLIN_H
#include <QTimer>

class perlin
{
public:
    perlin();
    float noise(float samx, float samy, float samz);
private:
    int *permu;
    float *Gx;
    float *Gy;
    float *Gz;
};

#endif // PERLIN_H
