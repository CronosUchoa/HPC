#include <stdio.h>
#include <mpi.h>
#include "../include/simulacao.h"
#include "../include/visualizacao.h"

int main(int argc, char *argv[]) {
    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    SDL_Window *window = NULL;
    SDL_GLContext gl_context = NULL;
    Camera3D camera = {0};
    camera.zoom = 1.5f;
    
    if (rank == 0) {
        if (!initialize_3d_graphics(&window, &gl_context)) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        
        printf("\n=== SIMULACAO MD 3D com MPI ===\n");
        printf("Processos MPI: %d\n", size);
        printf("Particulas: %d\n", N);
        printf("\nControles 3D:\n");
        printf("  Mouse + Botao Esquerdo: Rotacionar camera\n");
        printf("  Scroll do Mouse: Zoom\n");
        printf("  W/S: Zoom in/out\n");
        printf("  ESPACO: Pausar/Continuar\n");
        printf("  R: Reiniciar simulacao\n");
        printf("  ESC: Sair\n\n");
    }
    
    if (rank == 0) {
        init_particles();
    }
    
    broadcast_simulation_data(0, MPI_COMM_WORLD);
    
    int running = 1;
    int step = 0;
    int paused = 0;
    double start_time = MPI_Wtime();
    
    while (running) {
        if (rank == 0) {
            int input_result = handle_3d_input(&camera, &paused);
            if (input_result == -1) {
                running = 0;
            } else if (input_result == 1) {
                init_particles();
                step = 0;
                start_time = MPI_Wtime();
                printf("SIMULACAO REINICIADA\n");
            }
        }
        
        MPI_Bcast(&running, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&paused, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&step, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        if (!paused && step < NSTEPS && running) {
            compute_forces(rank, size);
            integrate_step();
            step++;
            
            if (rank == 0 && step % 100 == 0) {
                double ke = calculate_kinetic_energy();
                double elapsed = MPI_Wtime() - start_time;
                printf("Passo: %5d | Energia: %8.4f | FPS: %6.1f\n", 
                       step, ke, step / elapsed);
            }
        }
        
        if (rank == 0) {
            draw_3d_scene(step, size, &camera);
            SDL_Delay(16);
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
    }
    
    if (rank == 0) {
        double total_time = MPI_Wtime() - start_time;
        printf("\n=== SIMULACAO CONCLUIDA ===\n");
        printf("Tempo total: %.3f segundos\n", total_time);
        printf("Processos MPI: %d\n", size);
        printf("Passos simulados: %d\n", step);
        printf("Performance: %.1f passos/segundo\n", step / total_time);
        
        cleanup_3d_graphics(window, gl_context);
    }
    
    MPI_Finalize();
    return 0;
}