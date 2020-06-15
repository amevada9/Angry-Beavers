#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "list.h"
#include "vector.h"
#include "list.h"
#include "collision.h"


const double NINETY_DEGREES = 1.5070796327;
const double NEGATE = -1;
const int TYPE_MIN = 1;
const int TYPE_MAX = 2;
const int LARGE_NUMBER = 99999;
const int COLINEAR = 0;
const int CLOCKWISE = 1;
const int CC = 2;

bool get_if_collided(collision_info_t collision_info) {
  return collision_info.collided;
}

vector_t get_collision_axis(collision_info_t collision_info) {
    return collision_info.axis;
}

void set_collision_axis(collision_info_t collision_info, vector_t axis) {
  collision_info.axis = axis;
}

void set_collided(collision_info_t collision_info, bool collided) {
  collision_info.collided = collided;
}

double extrema(double a, double b, int type) {
  if (type == 0) {
    if (a < b) {
      return a;
    }
    return b;
  }
  else {
    if (a > b) {
      return a;
    }
    return b;
  }
}

bool onSegment(vector_t p, vector_t q, vector_t r)
{
  if (q.x <= extrema(p.x, r.x, 1) && q.x >= extrema(p.x, r.x, 0) &&
        q.y <= extrema(p.y, r.y, 1) && q.y >= extrema(p.y, r.y, 0)) {
      return true;
  }
  return false;
}

int orientation(vector_t p, vector_t q, vector_t r)
{
    double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0.0){
      return COLINEAR;
    }
    else if (val > 0.0){
      return CLOCKWISE;
    }
    return CC;
}

bool do_intersect(vector_t p1, vector_t q1, vector_t p2, vector_t q2)
{
    double o1 = orientation(p1, q1, p2);
    double o2 = orientation(p1, q1, q2);
    double o3 = orientation(p2, q2, p1);
    double o4 = orientation(p2, q2, q1);
    if (o1 != o2 && o3 != o4)
        return true;
    if (o1 == 0.0 && onSegment(p1, p2, q1)){
      return true;
    }
    if (o2 == 0.0 && onSegment(p1, q2, q1)){
      return true;
    }
    if (o3 == 0.0 && onSegment(p2, p1, q2)){
      return true;
    }
    if (o4 == 0.0 && onSegment(p2, q1, q2)){
      return true;
    }
    return false;
}

vector_t point_of_intersect(vector_t p1, vector_t q1, vector_t p2, vector_t q2){
  double dy1 = q1.y - p1.y;
  double dx1 = q1.x - p1.x;
  double m1 = dy1/dx1;
  double b1 = p1.y - (m1 * p1.x);

  double dy2 = q2.y - p2.y;
  double dx2 = q2.x - p2.x;

  double m2 = dy2 / dx2;
  double b2 = p2.y - (m2 * p2.x);
  double good_x = (b2 -b1)/(m1-m2);
  double good_y = m1 * good_x + b1;
  if (dx2 != 0 && dx1 != 0) {
    return(vec_init(good_x, good_y));
  }
  //now handle cases where dx is 0
  if(dx1 == 0){
    good_x = q1.x;
    good_y = m2 * good_x + b2;
    return(vec_init(good_x, good_y));
  }
  if(dx2 == 0){
    good_x = q2.x;
    good_y = m1 * good_x + b1;
    return(vec_init(good_x, good_y));
  }
  return(vec_init(0,0));
}

//only call this if you know that two bodies do indeed intersect
vector_t bodies_intersect(body_t *bod1, body_t *bod2){
  list_t *shape1 = body_get_shape(bod1);
  list_t *shape2 = body_get_shape(bod2);
  int size1 = list_size(shape1);
  int size2 = list_size(shape2);
  for(size_t i = 0; i < size1; i++){
    for(size_t j = 0; j < size2; j++){
      vector_t *p1 = list_get(shape1, i);
      vector_t *q1 = list_get(shape1, (i + 1) % size1);
      vector_t *p2 = list_get(shape2, j);
      vector_t *q2 = list_get(shape2, (j + 1) % size2);
      if(do_intersect(*p1, *q1, *p2, *q2)){
        return point_of_intersect(*p1, *q1, *p2, *q2);
      }
    }
  }
  return(vec_init(0, 0));
}

//Will return the min of the list or max of the list depending on what
// type is passed in
double find_extrema(list_t *numbers, int type){
  double *extrema = list_get(numbers, 0);
  for (int i = 1; i < list_size(numbers); i++){
    double *temp = list_get(numbers, i);
    if(type == 1 && *temp < *extrema){
      extrema = temp;
    }
    if(type == 2 && *temp > *extrema){
      extrema = temp;
    }
  }
  return *extrema;
}

/**
* finds the projection of each point in the provided shape onto the line given
* it then adds the magnitude of that projection to the list given
**/
void add_mag(vector_t line, list_t *shape, list_t *magnitude){
  for(int i = 0; i < list_size(shape); i++){
    vector_t *point = list_get(shape, i);
    vector_t projection =  vec_projection(*point, line);
    double *mag = malloc(sizeof(double));
    *mag = vec_magnitude(projection);
    //this corrects for projections that are negative
    if(vec_dot(*point, line) < 0){
      *mag *= NEGATE;
    }
    list_add(magnitude, mag);
  }
}

/**
*takes in a list that will contain the perpendicular vectors we are searching
* for and adds the vectors it gets from the given shape.
**/
void perpendicular_lines(list_t *perp_vectors, list_t *shape){
  size_t size = list_size(shape);
  for(size_t i = 0; i < size; i++){
    vector_t *point1 = list_get(shape, i);
    vector_t *point2 = list_get(shape, (i + 1) % size);
    vector_t difference = vec_subtract(*point1, *point2);
    vector_t *to_add = malloc(sizeof(vector_t));
    *to_add = vec_rotate(difference, NINETY_DEGREES);
    list_add(perp_vectors, to_add);
  }
}

double overlap(vector_t line, list_t *shape1, list_t *shape2){
  //lists of doubles that represent the magnitude of the projected vectors
  list_t *mag1 = list_init(list_size(shape1), (free_func_t) free);
  list_t *mag2 = list_init(list_size(shape2), (free_func_t) free);

  add_mag(line, shape1, mag1);
  add_mag(line, shape2, mag2);

  double max1 = find_extrema(mag1, TYPE_MAX);
  double max2 = find_extrema(mag2, TYPE_MAX);
  double min1 = find_extrema(mag1, TYPE_MIN);
  double min2 = find_extrema(mag2, TYPE_MIN);

  list_free(mag1);
  list_free(mag2);

  if(min1 <= min2){
    return(max1 - min2);
  }
  return(max2 - min1);
}

collision_info_t find_collision(body_t *body1, body_t *body2){
  list_t *shape1 = body_get_shape(body1);
  list_t *shape2 = body_get_shape(body2);
  collision_info_t information = {
    .collided = false,
    .axis = {-1, -1},
  };
  int size_of_perp = list_size(shape1) + list_size(shape2);
    //create a list of vectors that are the perpendicular lines and add to it
  list_t *perp_vectors = list_init(size_of_perp, (free_func_t) vec_free);
  perpendicular_lines(perp_vectors, shape1);
  perpendicular_lines(perp_vectors, shape2);
  vector_t overlap_vec = vec_init(0, 0);
  double least_overlap = LARGE_NUMBER;
  for(int i = 0; i < list_size(perp_vectors); i++){
    //if they do not overlap, return false because they are not colliding
    vector_t *line = list_get(perp_vectors, i);
    double temp_overlap = overlap(*line, shape1, shape2);
    if(temp_overlap < 0){
      list_free(perp_vectors);

      bool collided_last_tick = false;
      int index1 = -1;
      int index2 = -1;
      list_t *one_collidedWith = body_get_collision_bodies(body1);
      list_t *two_collidedWith = body_get_collision_bodies(body2);
      for (size_t i = 0; i < list_size(one_collidedWith); i++) {
        body_t *collided = (body_t *)list_get(one_collidedWith, i);
        if (collided == body2) {
            collided_last_tick = true;
            index1 = i;
            break;
        }
      }
      for (size_t i = 0; i < list_size(two_collidedWith); i++) {
        body_t *collided = (body_t *)list_get(two_collidedWith, i);
        if (collided == body1) {
            collided_last_tick = true;
            index2 = i;
            break;
        }
      }
      if(collided_last_tick){
        body_remove_collision_body(body1, index1);
        body_remove_collision_body(body2, index2);
        body_collided(body1, false);
        body_collided(body2, false);
      }
      return information;
    }
    else if(temp_overlap < least_overlap){
      overlap_vec = *line;
      least_overlap = temp_overlap;
    }
  }
  vector_t overlap_axis = vec_unit(overlap_vec);
  information.collided = true;
  information.axis = overlap_axis;
  list_free(perp_vectors);
  return information;
}
