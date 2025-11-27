#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "../include/definicoes.h"

void init_particles() {
    srand(42);
    for (int i = 0; i < N; i++) {
        pos[i].x = (double)rand()/RAND_MAX;
        pos[i].y = (double)rand()/RAND_MAX;
        pos[i].z = (double)rand()/RAND_MAX;
        vel[i].x = vel[i].y = vel[i].z = 0.0;
    }
}

void compute_forces(int rank, int size) {
    // Zera forças
    for (int i = 0; i < N; i++) {
        force[i].x = force[i].y = force[i].z = 0.0;
    }
    
    // Divide trabalho entre processos MPI
    int particles_per_proc = N / size;
    int start = rank * particles_per_proc;
    int end = (rank == size - 1) ? N : start + particles_per_proc;
    
    // Cada processo calcula forças locais
    Vec3 local_force[N];
    for (int i = 0; i < N; i++) {
        local_force[i].x = local_force[i].y = local_force[i].z = 0.0;
    }
    
    // Calcula interações para partículas deste processo
    for (int i = start; i < end; i++) {
        for (int j = i + 1; j < N; j++) {
            double dx = pos[i].x - pos[j].x;
            double dy = pos[i].y - pos[j].y;
            double dz = pos[i].z - pos[j].z;
            double r2 = dx*dx + dy*dy + dz*dz;
            
            if (r2 < 0.01) r2 = 0.01;
            
            double r6 = r2 * r2 * r2;
            double f = 24 * EPSILON * (2*pow(SIGMA,12)/pow(r6,2) - pow(SIGMA,6)/r6) / r2;

            local_force[i].x += f * dx;
            local_force[i].y += f * dy;
            local_force[i].z += f * dz;
            
            local_force[j].x -= f * dx;
            local_force[j].y -= f * dy;
            local_force[j].z -= f * dz;
        }
    }
    
    // Combina forças de todos os processos usando MPI_Allreduce
    MPI_Allreduce(local_force, force, 3*N, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
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

int main(int argc, char *argv[]) {
    int rank, size;
    double start, end;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        printf("=== SIMULAÇÃO MD - VERSÃO MPI ===\n");
        printf("Partículas: %d\n", N);
        printf("Timesteps: %d\n", NSTEPS);
        printf("Processos MPI: %d\n\n", size);
    }
    
    // Apenas o processo 0 inicializa
    if (rank == 0) {
        init_particles();
    }
    
    // Broadcast das posições e velocidades iniciais para todos os processos
    MPI_Bcast(pos, 3*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(vel, 3*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // Sincroniza antes de começar
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    
    for (int step = 0; step < NSTEPS; step++) {
        compute_forces(rank, size);
        integrate();
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    
    if (rank == 0) {
        double time_taken = end - start;
        
        printf("\n=== RESULTADOS ===\n");
        printf("Tempo total: %.3f segundos\n", time_taken);
        printf("Steps/segundo: %.2f\n", NSTEPS / time_taken);
        printf("Processos MPI: %d\n", size);
        printf("Interações calculadas: %lld\n", (long long)N * (N-1) / 2 * NSTEPS);
        printf("Comunicações MPI: %d Allreduce por step\n", NSTEPS);
    }
    
    MPI_Finalize();
    return 0;
}