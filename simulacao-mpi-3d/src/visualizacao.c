#include "../include/visualizacao.h"
#include <stdio.h>
#include <math.h>

void draw_sphere(float x, float y, float z, float radius, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(r, g, b);
    
    GLUquadric *quadric = gluNewQuadric();
    gluSphere(quadric, radius, 16, 16);
    gluDeleteQuadric(quadric);
    
    glPopMatrix();
}

void draw_cube_wireframe(float size) {
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(2.0f);
    
    glBegin(GL_LINES);
    
    // Linhas horizontais inferiores
    glVertex3f(-size, -size, -size);
    glVertex3f(size, -size, -size);
    
    glVertex3f(-size, -size, -size);
    glVertex3f(-size, -size, size);
    
    glVertex3f(size, -size, -size);
    glVertex3f(size, -size, size);
    
    glVertex3f(-size, -size, size);
    glVertex3f(size, -size, size);
    
    // Linhas horizontais superiores
    glVertex3f(-size, size, -size);
    glVertex3f(size, size, -size);
    
    glVertex3f(-size, size, -size);
    glVertex3f(-size, size, size);
    
    glVertex3f(size, size, -size);
    glVertex3f(size, size, size);
    
    glVertex3f(-size, size, size);
    glVertex3f(size, size, size);
    
    // Linhas verticais
    glVertex3f(-size, -size, -size);
    glVertex3f(-size, size, -size);
    
    glVertex3f(size, -size, -size);
    glVertex3f(size, size, -size);
    
    glVertex3f(-size, -size, size);
    glVertex3f(-size, size, size);
    
    glVertex3f(size, -size, size);
    glVertex3f(size, size, size);
    
    glEnd();
}

int initialize_3d_graphics(SDL_Window **window, SDL_GLContext *gl_context) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 0;
    }
    
    // Configurações OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    *window = SDL_CreateWindow(
        "Simulacao MD 3D com MPI",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    
    if (!*window) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        return 0;
    }
    
    *gl_context = SDL_GL_CreateContext(*window);
    if (!*gl_context) {
        printf("Erro ao criar contexto OpenGL: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        return 0;
    }
    
    // Inicializa GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        printf("Erro ao inicializar GLEW: %s\n", glewGetErrorString(err));
        return 0;
    }
    
    // Configurações OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    
    // Configura luz
    GLfloat light_pos[] = {5.0f, 5.0f, 5.0f, 1.0f};
    GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    
    return 1;
}

void cleanup_3d_graphics(SDL_Window *window, SDL_GLContext gl_context) {
    if (gl_context) {
        SDL_GL_DeleteContext(gl_context);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

void draw_3d_scene(int step, int mpi_processes, Camera3D *camera) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Aplica transformações da câmera
    update_camera(camera);
    
    // Desenha a caixa 3D
    draw_cube_wireframe(BOX_SIZE/2);
    
    // Desenha as partículas como esferas
    for (int i = 0; i < N; i++) {
        // Calcula cor baseada na velocidade e posição
        double v_mag = sqrt(vel[i].x*vel[i].x + vel[i].y*vel[i].y + vel[i].z*vel[i].z);
        float red = (float)(v_mag * 2.0);
        float green = 0.5f;
        float blue = (float)(pos[i].z / BOX_SIZE * 0.8f) + 0.2f;
        
        if (red > 1.0f) red = 1.0f;
        
        // Tamanho baseado na profundidade (perspectiva)
        float radius = 0.1f + (float)(pos[i].z / BOX_SIZE * 0.0005f);
        
        // Desenha a partícula
        draw_sphere(
            (float)(pos[i].x - BOX_SIZE/2), 
            (float)(pos[i].y - BOX_SIZE/2), 
            (float)(pos[i].z - BOX_SIZE/2), 
            radius, red, green, blue
        );
    }
    
    SDL_GL_SwapWindow(SDL_GL_GetCurrentWindow());
}

void update_camera(Camera3D *camera) {
    glTranslatef(0.0f, 0.0f, -10.0f * camera->zoom);
    glRotatef(camera->angle_x, 1.0f, 0.0f, 0.0f);
    glRotatef(camera->angle_y, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -BOX_SIZE/2);
}

int handle_3d_input(Camera3D *camera, int *paused) {
    SDL_Event event;
    int should_quit = 0;
    int should_restart = 0;
    
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                should_quit = 1;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        should_quit = 1;
                        break;
                    case SDLK_SPACE:
                        *paused = !(*paused);
                        printf("%s\n", *paused ? "PAUSADO" : "CONTINUANDO");
                        break;
                    case SDLK_r:
                        should_restart = 1;
                        break;
                    case SDLK_w:
                        camera->zoom *= 0.9f;
                        break;
                    case SDLK_s:
                        camera->zoom *= 1.1f;
                        break;
                }
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    camera->rotating = 1;
                    camera->last_x = event.button.x;
                    camera->last_y = event.button.y;
                }
                break;
                
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    camera->rotating = 0;
                }
                break;
                
            case SDL_MOUSEMOTION:
                if (camera->rotating) {
                    camera->angle_y += (event.motion.x - camera->last_x) * 0.5f;
                    camera->angle_x += (event.motion.y - camera->last_y) * 0.5f;
                    camera->last_x = event.motion.x;
                    camera->last_y = event.motion.y;
                    
                    // Limita rotação vertical
                    if (camera->angle_x > 90.0f) camera->angle_x = 90.0f;
                    if (camera->angle_x < -90.0f) camera->angle_x = -90.0f;
                }
                break;
                
            case SDL_MOUSEWHEEL:
                camera->zoom *= (1.0f - event.wheel.y * 0.1f);
                if (camera->zoom < 0.5f) camera->zoom = 0.5f;
                if (camera->zoom > 3.0f) camera->zoom = 3.0f;
                break;
        }
    }
    
    if (should_quit) return -1;
    if (should_restart) return 1;
    return 0;
}