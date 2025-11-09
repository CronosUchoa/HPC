#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>


#define N 100             // número de partículas
#define NSTEPS 100        // número de timesteps
#define DT 0.001          // passo de tempo
#define EPSILON 1.0       // parâmetro do potencial LJ
#define SIGMA 1.0
#define MASS 1.0

// Estrutura para posição, velocidade e força
typedef struct {
    double x, y, z;
} Vec3;

Vec3 pos[N], vel[N], force[N];

// Inicializa posições e velocidades
void init_particles() {
     srand(time(NULL)); 
    for (int i = 0; i < N; i++) {
        pos[i].x = (double)rand()/RAND_MAX;
        pos[i].y = (double)rand()/RAND_MAX;
        pos[i].z = (double)rand()/RAND_MAX;
        vel[i].x = vel[i].y = vel[i].z = 0.0;
    }
}

// Calcula forças usando potencial de Lennard-Jones 
void compute_forces() {
    // Zera forças
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        force[i].x = force[i].y = force[i].z = 0.0;
    }

    // Calcula interações entre pares
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            double dx = pos[i].x - pos[j].x;
            double dy = pos[i].y - pos[j].y;
            double dz = pos[i].z - pos[j].z;
            double r2 = dx*dx + dy*dy + dz*dz;
            double r6 = r2 * r2 * r2;
            double f = 24 * EPSILON * (2*pow(SIGMA,12)/pow(r6,2) - pow(SIGMA,6)/r6) / r2;

            // Atualiza forças — cuidado com condição de corrida
            #pragma omp atomic
            force[i].x += f * dx;
            #pragma omp atomic
            force[i].y += f * dy;
            #pragma omp atomic
            force[i].z += f * dz;
            
            #pragma omp atomic
            force[j].x -= f * dx;
            #pragma omp atomic
            force[j].y -= f * dy;
            #pragma omp atomic
            force[j].z -= f * dz;
        }
    }
}

// Atualiza posições e velocidades
void integrate() {
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        vel[i].x += (force[i].x / MASS) * DT;
        vel[i].y += (force[i].y / MASS) * DT;
        vel[i].z += (force[i].z / MASS) * DT;

        pos[i].x += vel[i].x * DT;
        pos[i].y += vel[i].y * DT;
        pos[i].z += vel[i].z * DT;
    }
}

int main() {
    double start, end;
    init_particles();

    start = omp_get_wtime();
    for (int step = 0; step < NSTEPS; step++) {
        compute_forces();
        integrate();
    }
    end = omp_get_wtime();

    printf("Simulacao concluida em %.3f segundos\n", end - start);
    return 0;
}

