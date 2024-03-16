#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include "matrix.h"

typedef struct {
	vec3_t position;
	vec3_t direction;
	vec3_t forward_velocity;
	float yaw;
	float pitch;
}camera_t;

void init_camera(vec3_t position, vec3_t direction);

vec3_t get_camera_position(void);
vec3_t get_camera_direction(void);
vec3_t get_camera_forward_velocity(void);
float get_camera_yaw(void);
float get_camera_pitch(void);

void set_camera_position(vec3_t position);
void set_camera_direction(vec3_t direction);
void set_camera_forward_velocity(vec3_t forward_velocity);
void set_camera_yaw(float angle);
void set_camera_pitch(float angle);

vec3_t get_camera_target(void);

#endif