#include <math.h>
#include "vector.h"

float vect2_length(vect2_t v) {
	return (float)(sqrt((v.x * v.x) + (v.y * v.y)));
}

vect2_t vect2_add(vect2_t a, vect2_t b) {
	vect2_t v = {
		.x = a.x + b.x,
		.y = a.y + b.y 
	};
	return v;
}

vect2_t vect2_sub(vect2_t a, vect2_t b) {
	vect2_t v = {
		.x = a.x - b.x,
		.y = a.y - b.y
	};
	return v;
}

vect2_t vect2_mul(vect2_t v, float f) {
	vect2_t result = {
		.x = v.x * f,
		.y = v.y * f
	};
	return result;
}

vect2_t vect2_div(vect2_t v, float f) {
	vect2_t result = {
		.x = v.x / f,
		.y = v.y / f
	};
	return result;
}

float vect2_dot(vect2_t a, vect2_t b) {
	return (float)((a.x * b.x) + (a.y * b.y));
}

void vect2_normalize(vect2_t* v) {
	float vector_magnitude = vect2_length(*v);

	v->x /= vector_magnitude;
	v->y /= vector_magnitude;
}

/////////////////////////////////////////////////////////////////
float vect3_length(vect3_t v) {
	return (float)(sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)));
}

vect3_t vect3_add(vect3_t a, vect3_t b) {
	vect3_t v = {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z
	};
	return v;
}

vect3_t vect3_sub(vect3_t a, vect3_t b) {
	vect3_t v = {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z
	};
	return v;
}

vect3_t vect3_mul(vect3_t v, float f) {
	vect3_t result = {
		.x = v.x * f,
		.y = v.y * f,
		.z = v.z * f
	};
	return result;
}

vect3_t vect3_div(vect3_t v, float f) {
	vect3_t result = {
		.x = v.x / f,
		.y = v.y / f,
		.z = v.z / f
	};
	return result;
}

vect3_t vect3_cross(vect3_t a, vect3_t b) {
	vect3_t v = {
		.x = (a.y * b.z) - (a.z * b.y),
		.y = (a.z * b.x) - (a.x * b.z),
		.z = (a.x * b.y) - (a.y * b.x)
	};
	return v;
}

float vect3_dot(vect3_t a, vect3_t b) {
	return (float)((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

void vect3_normalize(vect3_t* v) {
	float vector_magnitude = vect3_length(*v);

	v->x /= vector_magnitude;
	v->y /= vector_magnitude;
	v->z /= vector_magnitude;
}

///////////////////////////////////////////////////////////
vect3_t vect3_rotate_x(vect3_t v, float angle){
	vect3_t rotated_vector = {
		.x = v.x,
		.y = v.y * cos(angle) - v.z * sin(angle),
		.z = v.y * sin(angle) + v.z * cos(angle)
	};
	return rotated_vector;
}

vect3_t vect3_rotate_y(vect3_t v, float angle) {
	vect3_t rotated_vector = {
		.x = v.x * cos(angle) - v.z * sin(angle),
		.y = v.y,
		.z = v.x * sin(angle) + v.z * cos(angle)
	};
	return rotated_vector;
}
vect3_t vect3_rotate_z(vect3_t v, float angle) {
	vect3_t rotated_vector = {
		.x = v.x * cos(angle) - v.y * sin(angle),
		.y = v.x * sin(angle) + v.y * cos(angle),
		.z = v.z
	};
	return rotated_vector;
}

vect4_t vect4_from_vect3(vect3_t v) {
	vect4_t vect = { v.x, v.y, v.z, 1.0 };

	return vect;
}

vect3_t vect3_from_vect4(vect4_t v) {
	vect3_t vect = { v.x, v.y, v.z };

	return vect;
}