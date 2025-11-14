#ifndef SIMULACAO_H
#define SIMULACAO_H

#include <mpi.h>

#define N 50
#define NSTEPS 100000
#define DT 0.05
#define EPSILON 5
#define SIGMA 0.5
#define MASS 1.0
#define BOX_SIZE 5.0

typedef struct {
    double x, y, z;
} Vec3;

// Variáveis globais da simulação
extern Vec3 pos[N];
extern Vec3 vel[N];
extern Vec3 force[N];

// Funções da simulação
void init_particles();
void compute_forces(int rank, int size);
void integrate_step();
double calculate_kinetic_energy();
void broadcast_simulation_data(int root, MPI_Comm comm);

#endif