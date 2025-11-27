#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "definicoes.h"

void init_particles() {
    srand(42); // Seed fixo para reprodutibilidade
    for (int i = 0; i < N; i++) {
        pos[i].x = (double)rand()/RAND_MAX;
        pos[i].y = (double)rand()/RAND_MAX;
        pos[i].z = (double)rand()/RAND_MAX;
        vel[i].x = vel[i].y = vel[i].z = 0.0;
    }
}

void compute_forces() {
    // Zera forças
    for (int i = 0; i < N; i++) {
        force[i].x = force[i].y = force[i].z = 0.0;
    }

    // Calcula interações entre pares
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            double dx = pos[i].x - pos[j].x;
            double dy = pos[i].y - pos[j].y;
            double dz = pos[i].z - pos[j].z;
            double r2 = dx*dx + dy*dy + dz*dz;
            
            if (r2 < 0.01) r2 = 0.01; // Evita divisão por zero
            
            double r6 = r2 * r2 * r2;
            double f = 24 * EPSILON * (2*pow(SIGMA,12)/pow(r6,2) - pow(SIGMA,6)/r6) / r2;

            force[i].x += f * dx;
            force[i].y += f * dy;
            force[i].z += f * dz;
            
            force[j].x -= f * dx;
            force[j].y -= f * dy;
            force[j].z -= f * dz;
        }
    }
}

void integrate() {
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
    clock_t start, end;
    double cpu_time_used;
    
    printf("=== SIMULAÇÃO MD - VERSÃO SERIAL ===\n");
    printf("Partículas: %d\n", N);
    printf("Timesteps: %d\n\n", NSTEPS);
    
    init_particles();

    start = clock();
    for (int step = 0; step < NSTEPS; step++) {
        compute_forces();
        integrate();
    }
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    printf("\n=== RESULTADOS ===\n");
    printf("Tempo total: %.3f segundos\n", cpu_time_used);
    printf("Steps/segundo: %.2f\n", NSTEPS / cpu_time_used);
    printf("Interações calculadas: %lld\n", (long long)N * (N-1) / 2 * NSTEPS);
    
    return 0;
}