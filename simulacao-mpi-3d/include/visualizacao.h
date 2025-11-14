#ifndef VISUALIZACAO_H
#define VISUALIZACAO_H

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "simulacao.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

// Estrutura para controle de câmera 3D
typedef struct {
    float angle_x, angle_y;
    float zoom;
    float last_x, last_y;
    int rotating;
} Camera3D;

// Funções de visualização 3D
int initialize_3d_graphics(SDL_Window **window, SDL_GLContext *gl_context);
void cleanup_3d_graphics(SDL_Window *window, SDL_GLContext gl_context);
void draw_3d_scene(int step, int mpi_processes, Camera3D *camera);
int handle_3d_input(Camera3D *camera, int *paused);
void update_camera(Camera3D *camera);

#endif