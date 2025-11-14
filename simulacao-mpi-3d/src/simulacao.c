#include "../include/simulacao.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

Vec3 pos[N], vel[N], force[N];

void init_particles() {
    srand(42);
    int particles_per_side = (int)ceil(pow(N, 1.0/3.0));
    double spacing = BOX_SIZE / (particles_per_side + 1);
    
    int idx = 0;
    for (int i = 0; i < particles_per_side && idx < N; i++) {
        for (int j = 0; j < particles_per_side && idx < N; j++) {
            for (int k = 0; k < particles_per_side && idx < N; k++) {
                pos[idx].x = (i + 1) * spacing;
                pos[idx].y = (j + 1) * spacing;
                pos[idx].z = (k + 1) * spacing;
                
                vel[idx].x = ((double)rand()/RAND_MAX - 0.5) * 0.01;
                vel[idx].y = ((double)rand()/RAND_MAX - 0.5) * 0.01;
                vel[idx].z = ((double)rand()/RAND_MAX - 0.5) * 0.01;
                idx++;
            }
        }
    }
}

void compute_forces(int rank, int size) {
    for (int i = 0; i < N; i++) {
        force[i].x = force[i].y = force[i].z = 0.0;
    }
    
    int particles_per_proc = N / size;
    int start = rank * particles_per_proc;
    int end = (rank == size - 1) ? N : start + particles_per_proc;
    
    Vec3 local_force[N];
    for (int i = 0; i < N; i++) {
        local_force[i].x = local_force[i].y = local_force[i].z = 0.0;
    }
    
    for (int i = start; i < end; i++) {
        for (int j = i + 1; j < N; j++) {
            double dx = pos[i].x - pos[j].x;
            double dy = pos[i].y - pos[j].y;
            double dz = pos[i].z - pos[j].z;
            
            if (dx > BOX_SIZE/2) dx -= BOX_SIZE;
            if (dx < -BOX_SIZE/2) dx += BOX_SIZE;
            if (dy > BOX_SIZE/2) dy -= BOX_SIZE;
            if (dy < -BOX_SIZE/2) dy += BOX_SIZE;
            if (dz > BOX_SIZE/2) dz -= BOX_SIZE;
            if (dz < -BOX_SIZE/2) dz += BOX_SIZE;
            
            double r2 = dx*dx + dy*dy + dz*dz;
            
            if (r2 < 0.01) r2 = 0.01;
            if (r2 > 4.0) continue;
            
            double r = sqrt(r2);
            double r6 = pow(SIGMA/r, 6);
            double r12 = r6 * r6;
            
            double f = 24 * EPSILON * (2*r12 - r6) / r2;
            
            local_force[i].x += f * dx;
            local_force[i].y += f * dy;
            local_force[i].z += f * dz;
            
            local_force[j].x -= f * dx;
            local_force[j].y -= f * dy;
            local_force[j].z -= f * dz;
        }
    }
    
    MPI_Allreduce(local_force, force, 3*N, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
}

void integrate_step() {
    double max_force = 10.0;
    
    for (int i = 0; i < N; i++) {
        double f_mag = sqrt(force[i].x*force[i].x + force[i].y*force[i].y + force[i].z*force[i].z);
        if (f_mag > max_force) {
            double scale = max_force / f_mag;
            force[i].x *= scale;
            force[i].y *= scale;
            force[i].z *= scale;
        }
        
        vel[i].x += (force[i].x / MASS) * DT;
        vel[i].y += (force[i].y / MASS) * DT;
        vel[i].z += (force[i].z / MASS) * DT;
        
        double v_mag = sqrt(vel[i].x*vel[i].x + vel[i].y*vel[i].y + vel[i].z*vel[i].z);
        if (v_mag > 1.0) {
            double scale = 1.0 / v_mag;
            vel[i].x *= scale;
            vel[i].y *= scale;
            vel[i].z *= scale;
        }
        
        pos[i].x += vel[i].x * DT;
        pos[i].y += vel[i].y * DT;
        pos[i].z += vel[i].z * DT;
        
        while (pos[i].x < 0) pos[i].x += BOX_SIZE;
        while (pos[i].x >= BOX_SIZE) pos[i].x -= BOX_SIZE;
        while (pos[i].y < 0) pos[i].y += BOX_SIZE;
        while (pos[i].y >= BOX_SIZE) pos[i].y -= BOX_SIZE;
        while (pos[i].z < 0) pos[i].z += BOX_SIZE;
        while (pos[i].z >= BOX_SIZE) pos[i].z -= BOX_SIZE;
    }
}

double calculate_kinetic_energy() {
    double ke = 0.0;
    for (int i = 0; i < N; i++) {
        double v2 = vel[i].x*vel[i].x + vel[i].y*vel[i].y + vel[i].z*vel[i].z;
        ke += 0.5 * MASS * v2;
    }
    return ke;
}

void broadcast_simulation_data(int root, MPI_Comm comm) {
    MPI_Bcast(pos, 3*N, MPI_DOUBLE, root, comm);
    MPI_Bcast(vel, 3*N, MPI_DOUBLE, root, comm);
}