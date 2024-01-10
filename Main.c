#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "matrix.h"

triangle_t* triangles_to_render = NULL;

vect3_t camera_position = { .x = 0, .y = 0, .z = 0 };

float fov_factor = 640;

bool is_running = false;
uint32_t previous_frame_time = 0;

int compare_avg_z(const void *a, const void *b) {
    if (((triangle_t*)a)->avg_depth < ((triangle_t*)b)->avg_depth) {
        return 1;
    }
    else {
        return 0;
    }
}

void setup(void) {
    reneder_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE;

    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    load_cube_mesh_data();
    //load_obj_file_data("./assets/models/cube.obj");
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            is_running = false;
            break;
        }
        else if (event.key.keysym.sym == SDLK_1) {
            reneder_method = RENDER_WIRE_VERTEX;
            break;
        }
        else if (event.key.keysym.sym == SDLK_2) {
            reneder_method = RENDER_WIRE;
            break;
        }
        else if (event.key.keysym.sym == SDLK_3) {
            reneder_method = RENDER_FILL_TRIANGLE;
            break;
        }
        else if (event.key.keysym.sym == SDLK_4) {
            reneder_method = RENDER_FILL_TRIANGLE_WIRE;
            break;
        }
        else if (event.key.keysym.sym == SDLK_c) {
            cull_method = CULL_BACKFACE;
            break;
        }
        else if (event.key.keysym.sym == SDLK_d) {
            cull_method = CULL_NONE;
            break;
        }
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

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;
    mesh.scale.x += 0.002;
    mesh.scale.y += 0.001;
    mesh.translation.x += 0.01;
    mesh.translation.z = 5.0;

    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];
        vect3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vect4_t transformed_vertices[3];

        for (int j = 0; j < 3; j++) {
            vect4_t transformed_vertex = vect4_from_vect3(face_vertices[j]);
            
            mat4_t world_matrix = mat4_identity();

            world_matrix = mat4_mul_matt4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_matt4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_matt4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_matt4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_matt4(translation_matrix, world_matrix);

            transformed_vertex = mat4_mul_vect4(world_matrix, transformed_vertex);
            //transformed_vertex = mat4_mul_vect4(scale_matrix, transformed_vertex);
            //transformed_vertex = mat4_mul_vect4(rotation_matrix_x, transformed_vertex);
            //transformed_vertex = mat4_mul_vect4(rotation_matrix_y, transformed_vertex);
            //transformed_vertex = mat4_mul_vect4(rotation_matrix_z, transformed_vertex);
            //transformed_vertex = mat4_mul_vect4(translation_matrix, transformed_vertex);

            transformed_vertices[j] = transformed_vertex;
        }

        if (CULL_BACKFACE == cull_method) {
            vect3_t vector_a = vect3_from_vect4(transformed_vertices[0]); /*    A    */
            vect3_t vector_b = vect3_from_vect4(transformed_vertices[1]); /*   / \   */
            vect3_t vector_c = vect3_from_vect4(transformed_vertices[2]); /*  C---B  */

            vect3_t vector_ab = vect3_sub(vector_b, vector_a);
            vect3_t vector_ac = vect3_sub(vector_c, vector_a);
            vect3_normalize(&vector_ab);
            vect3_normalize(&vector_ac);

            vect3_t face_normal = vect3_cross(vector_ab, vector_ac);  //order matters
            vect3_normalize(&face_normal);

            vect3_t vector_camera_ray = vect3_sub(camera_position, vector_a);

            float dot_normal_camera = vect3_dot(face_normal, vector_camera_ray);

            if (0 > dot_normal_camera) continue;
        }

        vect2_t projected_points[3];

        for (int j = 0; j < 3; j++) {
            projected_points[j] = project(vect3_from_vect4(transformed_vertices[j]));

            projected_points[j].x += (window_width / 2);
            projected_points[j].y += (window_height / 2);
        }

        float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;

        triangle_t projected_triangle = {
            .points = {
                { projected_points[0].x, projected_points[0].y },
                { projected_points[1].x, projected_points[1].y },
                { projected_points[2].x, projected_points[2].y }
            },
            .color = mesh_face.color,
            .avg_depth = avg_depth
        };
        array_push(triangles_to_render, projected_triangle);
    }
    if (NULL != triangles_to_render) {
        qsort(
            triangles_to_render,
            array_length(triangles_to_render),
            sizeof(triangles_to_render[0]),
            compare_avg_z
        );
    }
}

void render(void) {
    draw_grid(0xFF444444);

    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++) {
        if (RENDER_FILL_TRIANGLE == reneder_method || RENDER_FILL_TRIANGLE_WIRE == reneder_method) {
            draw_filled_triangle(triangles_to_render[i]);
        }
        if (RENDER_WIRE_VERTEX == reneder_method) {
            for (int j = 0; j < 3; j++) {
                draw_rect(triangles_to_render[i].points[j].x - 2, triangles_to_render[i].points[j].y - 2, 4, 4, 0xFFFF0000);
            }
        }
        if (RENDER_WIRE == reneder_method || RENDER_WIRE_VERTEX == reneder_method || RENDER_FILL_TRIANGLE_WIRE == reneder_method) {
            draw_triangle(triangles_to_render[i], 0xFFFFFFFF);
        }     
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
