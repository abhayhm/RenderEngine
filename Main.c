#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"

triangle_t* triangles_to_render = NULL;

vect3_t camera_position = { .x = 0, .y = 0, .z = 0 };

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
    load_obj_file_data("./assets/models/cube.obj");
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
    
    previous_frame_time = (uint32_t)SDL_GetTicks;

    triangles_to_render = NULL;

    mesh.rotation.x += 0.01f;
    mesh.rotation.y += 0.01f;
    mesh.rotation.z += 0.01f;

    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];
        vect3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vect3_t transformed_vertices[3];

        for (int j = 0; j < 3; j++) {
            vect3_t transformed_vertex = face_vertices[j];
            transformed_vertex = vect3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vect3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vect3_rotate_z(transformed_vertex, mesh.rotation.z);

            transformed_vertex.z += 5;
            transformed_vertices[j] = transformed_vertex;
        }

        vect3_t vector_a = transformed_vertices[0]; /*    A    */
        vect3_t vector_b = transformed_vertices[1]; /*   / \   */
        vect3_t vector_c = transformed_vertices[2]; /*  C---B  */

        vect3_t vector_ab = vect3_sub(vector_b, vector_a);
        vect3_t vector_ac = vect3_sub(vector_c, vector_a);
        vect3_normalize(&vector_ab);
        vect3_normalize(&vector_ac);

        vect3_t face_normal = vect3_cross(vector_ab, vector_ac);  //order matters
        vect3_normalize(&face_normal);

        vect3_t vector_camera_ray = vect3_sub(camera_position, vector_a);

        float dot_normal_camera = vect3_dot(face_normal, vector_camera_ray);

        if (0 > dot_normal_camera) continue;

        triangle_t projected_triangle;

        for (int j = 0; j < 3; j++) {
            vect2_t projected_point = project(transformed_vertices[j]);

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
        draw_filled_triangle(triangles_to_render[i], 0xFFFFFFFF);
        draw_triangle(triangles_to_render[i], 0xFF000000);
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
