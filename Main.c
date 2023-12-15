#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"

triangle_t* triangles_to_render = NULL;

vect3_t camera_position = { .x = 0, .y = 0, .z = -5 };

float fov_factor = 640;

bool is_running = false;
uint32_t previous_frame_time = 0;

void setup(void) {
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );
    load_cube_mesh_data();
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
            is_running = false;
        break;
    }
}

vect2_t project(vect3_t point) {
    vect2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * point.y) / point.z
    };

    return projected_point;
}

void update(void) {

    uint32_t time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    
    previous_frame_time = SDL_GetTicks;

    triangles_to_render = NULL;

    mesh.rotation.x += 0.01f;
    mesh.rotation.y += 0.01f;
    mesh.rotation.z += 0.01f;

    int num_faces = array_length(mesh.faces);
    for (unsigned int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];
        vect3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        triangle_t projected_triangle;

        for (unsigned int j = 0; j < 3; j++) {
            vect3_t transformed_vertex = face_vertices[j];
            transformed_vertex = vect3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vect3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vect3_rotate_z(transformed_vertex, mesh.rotation.z);

            transformed_vertex.z -= camera_position.z;

            vect2_t projected_point = project(transformed_vertex);

            projected_point.x += (window_width / 2);
            projected_point.y += (window_height / 2);

            projected_triangle.points[j] = projected_point;
        }
        array_push(triangles_to_render, projected_triangle);
    }
}

void render(void) {
    draw_grid(0xFF444444);

    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++) {
        draw_triangle(triangles_to_render[i], 0xFFFFFF00);
    }

    array_free(triangles_to_render);

    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}

void free_resources(void) {
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
}

int main(int argc, char* args[]) {
    is_running = initialize_window();

    setup();

    while (is_running) {
        process_input();
        update();
        render();
    }

    free_resources();
    destroy_window();

    return 0;
}
