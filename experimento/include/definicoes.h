#ifndef DEFINICOES_H
#define DEFINICOES_H

#define N 1000
#define NSTEPS 10000
#define DT 0.001
#define EPSILON 1.0
#define SIGMA 1.0
#define MASS 1.0

typedef struct {
    double x, y, z;
} Vec3;
Vec3 pos[N], vel[N], force[N];
#endif