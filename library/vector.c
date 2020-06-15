#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const vector_t VEC_ZERO =  {0.0, 0.0};

/*
Will and Arya 9:17 ct 5/6/20 - we wrote a vec_projection method which projects one
vector onto another
*/

vector_t *vec_init_pointer(double x, double y){
  vector_t *vec = malloc(sizeof(vector_t));
  vec->x = x;
  vec->y = y;
  return vec;
}

vector_t vec_init(double x, double y) {
  vector_t vec = {x,y};
  return vec;
}

void vec_free(vector_t *vector){
  free(vector);
}

vector_t vec_add(vector_t v1, vector_t v2){
  vector_t vNew = {
    .x = v1.x + v2.x,
    .y = v1.y + v2.y,
  };
  return vNew;
}

/**
Subtracts two vectors by components.
 */
vector_t vec_subtract(vector_t v1, vector_t v2){
  vector_t vNew = {
    .x = v1.x - v2.x,
    .y = v1.y - v2.y,
  };
  return vNew;
}

/**
Negates a vector to compute its inverse.
 */
vector_t vec_negate(vector_t v){
  vector_t vNew = {-v.x, -v.y};
  return vNew;
}

/**
Scales components of a vector.
 */
vector_t vec_multiply(double scalar, vector_t v){
  vector_t vNew = {scalar * v.x, scalar * v.y};
  return vNew;
}

/**
Computres the dot product of two vectors.
 */
double vec_dot(vector_t v1, vector_t v2){
  return (v1.x * v2.x) + (v1.y * v2.y);
}

/**
Computes the cross product of two vectors.
 */
double vec_cross(vector_t v1, vector_t v2){
  return (v1.x * v2.y) - (v1.y * v2.x);
}

/**
Rotates a vector about a given angle.
 */
vector_t vec_rotate(vector_t v, double angle){
  vector_t vNew = {v.x * cos(angle) - v.y * sin(angle), v.x * sin(angle) +
    v.y * cos(angle)};
  return vNew;
}

void vector_free(vector_t *vec) {
  free(vec);
}

double vec_magnitude(vector_t v) {
  double magX = v.x * v.x;
  double magY = v.y * v.y;
  double magnitude = sqrt(magX + magY);
  return magnitude;
}

vector_t vec_projection(vector_t v1, vector_t v2){
  double dot = vec_dot(v1, v2);
  double mag = vec_magnitude(v2);
  double mag_squared = mag * mag;
  double scalar = dot / mag_squared;
  return vec_multiply(scalar, v2);
}

vector_t vec_unit(vector_t v){
  double length = vec_magnitude(v);
  double to_multiply = 1/length;
  return vec_multiply(to_multiply, v);
}
