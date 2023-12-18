#include <stdio.h>
#include <string.h>
#include "mesh.h"
#include "array.h"

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = { 0, 0, 0 }
};

void load_obj_file_data(char* filename) {
    FILE* fptr = NULL;
    fopen_s(&fptr, filename, "r");
    if (NULL == fptr) return;

    char line[1024];

    while (fgets(line, 1024, fptr)) {
        if (strncmp(line, "v ", 2) == 0) {
            vect3_t vertex;
            sscanf_s(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        }

        if(strncmp(line, "f ", 2) == 0) {
            face_t face;
            sscanf_s(line, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &face.a, &face.b, &face.c);
            array_push(mesh.faces, face);
        }
    }
}