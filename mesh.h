#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define N_MEST_VERTICES 8
extern vect3_t mesh_vertices[N_MEST_VERTICES];

#define N_MESH_FACES (6 * 2) // 6 cube faces, 2 triangles for each
extern face_t mesh_faces[N_MESH_FACES];

#endif
