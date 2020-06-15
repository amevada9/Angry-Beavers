#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "body.h"
#include "polygon.h"

const double ACC_MULT = 0.5;
const size_t DEFAULT_SIZE = 10;
const double PI = 3.14159265359;
const int BEAVER = 1;
const int CORONA = 5;
const int FANCY_BEAVER = 8;

typedef struct body {
  list_t *points;
  vector_t velocity;
  vector_t acceleration;
  vector_t centroid;
  vector_t force;
  vector_t impulse;
  double mass;
  rgb_color_t color;
  double angle;
  void *info;
  free_func_t info_freer;
  bool removed;
  bool collided_with;
  list_t *bodies_collided_with;
  vector_t impact_pos;
  double torque;
  double angular_impulse;
  double angular_velocity;
  bool is_launched;
  double scale_factor;
  vector_t rotate_point;
  vector_t ground;
} body_t;

/**
 * Allocates memory for a body with the given parameters.
 * The body is initially at rest.
 */
body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
  void *info, free_func_t info_freer){
    body_t *body = malloc(sizeof(body_t));
    assert(body != NULL);
    body->points = list_init(DEFAULT_SIZE, (free_func_t) vec_free);
    assert(body->points != NULL);
    body->points = shape;
    body->centroid = polygon_centroid(shape);
    assert(mass >= 0);
    body->mass = mass;
    body->color = color;
    body->velocity = VEC_ZERO;
    body->force = VEC_ZERO;
    body->info = info;
    body->info_freer = info_freer;
    body->removed = false;
    body->angle = 0.0;
    body->collided_with = false;
    body->bodies_collided_with = list_init(1, (free_func_t) body_free);
    body-> impact_pos = body->centroid; 
    body->torque = 0.0;
    body->angular_impulse = 0.0;
    body->angular_velocity = 0.0;
    body->scale_factor = 1.0;
    body->rotate_point = body->centroid;
    return body;
    body->ground = VEC_ZERO;
}

/**
Calls body_init_with_info, passing info fields as NULL.
 */
body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  return body_init_with_info(shape, mass, color, NULL, NULL);
}


/**
Releases the memory allocated for a body.
  */
void body_free(body_t *body) {
  list_free(body->points);
  if(body->info_freer != NULL){
    body->info_freer(body->info);
  }
  free(body);
}

/**
Gets the current shape of a body. Returns a newly allocated vector list,
which must be list_free()d.
 */
list_t *body_get_shape(body_t *body) {
  list_t *points = body->points;
  size_t n = list_size(points);
  list_t *returnList = list_init(n, (free_func_t) vec_free);
  for(size_t i = 0; i < n; i++){
    vector_t *newVec = malloc(sizeof(vector_t));
    assert(newVec!= NULL);
    vector_t *oldVec = list_get(points, i);
    newVec->x = oldVec->x;
    newVec->y = oldVec->y;
    list_add(returnList, newVec);
  }
  return returnList;
}

/**
Gets the current center of mass of a body.
*/
vector_t body_get_centroid(body_t *body) {
  return body->centroid;
}

/**
Gets the current velocity of a body.
*/
vector_t body_get_velocity(body_t *body) {
  return body->velocity;
}

/**
Gets the current force on a body.
*/
vector_t body_get_force(body_t *body) {
  return body->force;
}

/**
Gets the mass of a body.
*/
double body_get_mass(body_t *body) {
  return body->mass;
}

/**
Gets the display color of a body.
*/
rgb_color_t body_get_color(body_t *body) {
  return body->color;
}

/**
 * Gets the information associated with a body.
 */
void *body_get_info(body_t *body){
  return body->info;
}

/**
Sets the ground to a vector.
*/
void body_set_ground(body_t *body, vector_t vec){
  body->ground = vec;
}

/**
Gets the ground vector.
*/
vector_t body_get_ground(body_t *body){
  return body->ground;
}

/**
Translates a body to a new position.
The position is specified by the position of the body's center of mass.
*/
void body_set_centroid(body_t *body, vector_t x) {
  vector_t translationVector = vec_negate(body_get_centroid(body));
  polygon_translate(body->points, translationVector);
  body->centroid.x = x.x;
  body->centroid.y = x.y;
  polygon_translate(body->points, x);
}

/**
Sets a body's point list.
*/
void body_set_points(body_t *body, list_t *list) {
  body->points = list;
  body->centroid = polygon_centroid(list);
}

/**
Gets if the body has been launched.
*/
bool body_get_launched(body_t *body){
  return body->is_launched;
}

/**
Sets if body is launched.
*/
void body_set_launched(body_t *body, bool truth){
  body->is_launched = truth;
}

/**
Gets the impact position of a body.
*/
vector_t body_get_imp_pos(body_t *body) {
  return body->impact_pos;
}

/**
Gets the rotation point of a body.
*/
vector_t body_get_rot_point(body_t *body) {
  return body->rotate_point;
}

/**
Changes a body's velocity (the time-derivative of its position).
*/
void body_set_velocity(body_t *body, vector_t v) {
  body->velocity = v;
}

/**
Sets angular velocity.
*/
void body_set_angular_velocity(body_t *body, double v) {
  body->angular_velocity = v;
}

/**
Sets torque of body.
*/
void body_set_torque (body_t *body, double v) {
  body->torque = v;
}

/**
Sets impact position of a body.
*/
void body_set_impact_pos(body_t *body, vector_t pos){
  body->impact_pos = pos;
  body->rotate_point = body->impact_pos;
}


/**
Changes a body's orientation in the plane.
The body is rotated about its center of mass.
Note that the angle is *absolute*, not relative to the current orientation.
*/
void body_set_rotation(body_t *body, double angle_to_rotate) {
  polygon_rotate(body->points, angle_to_rotate, body->rotate_point);
  body->angle = (body->angle + angle_to_rotate) ;
}

/**
Sets a body's rotation point.
*/
void body_set_rotation_point(body_t* body, vector_t rotation_point){
  body->rotate_point = rotation_point;
}

/**
Adds a force to a body. This is for backwards compatibility.
*/
void body_add_force(body_t *body, vector_t force) {
  body->force = vec_add(body->force, force);
}


/**
Adds torque to a body.
*/
void body_add_torque(body_t *body, double add){
  body->torque += add;
}

/**
Gets torque of a body.
*/
double body_get_torque(body_t *body){
  return body->torque;
}

/**
Adds the force with an impact position set to add torque.
*/
void body_add_force_imp_pos(body_t *body, vector_t force, vector_t imp_pos) {

  body->force = vec_add(body->force, force);

  double force_mag = vec_magnitude(body->force);////
  vector_t radial_line = vec_subtract(body->impact_pos, body->rotate_point);
  double angle_btwn = ang_diff(radial_line, body->force); ////
  double translational = force_mag * cos(angle_btwn);
  double rotational =  force_mag * sin(angle_btwn);
  if(force_mag != 0.0){
    body->scale_factor = translational / force_mag;
  }

  double distance = vec_magnitude(radial_line);
  if(((body->impact_pos.x == body->centroid.x) && (body->impact_pos.y ==
    body->centroid.y))){
    distance = 0.0;
  }
  if(translational == 0.0){
    rotational = 0.0;
  }

  body->torque = distance * rotational;

}
/**
Add impulse to a body.
*/
void body_add_impulse(body_t *body, vector_t impulse) {
  body->impulse = vec_add(body->impulse, impulse);
}

/**
Sets angular impulse of a body to v.
*/
void body_set_angular_impulse(body_t *body, double v){
  body->angular_velocity = v;
}

/**
Adds v to current angular impulse.
*/
void body_add_angular_impulse(body_t *body, double v){
  body->angular_impulse += v;
}

/**
Returns angular velocity.
*/
double body_get_angular_velocity(body_t *body){
  return body->angular_velocity;
}


/*
Finds the angle between two vectors.
*/
double ang_diff(vector_t one, vector_t two){
  double mag_one = vec_magnitude(one);
  double mag_two = vec_magnitude(two);
  double dot = vec_dot(one, two);
  if(mag_one ==0  || mag_two == 0){
    return 0.0;
  }
  double cos_ang = dot / (mag_one * mag_two);
  return acos(cos_ang);
}



/**
Moves a body at its current velocity over a given time interval.
*/
void body_tick(body_t *body, double dt) {
  if(!body_is_removed(body)) {

  // calcuating effects of linear forces with adjusted force from above
    vector_t old_velocity = body_get_velocity(body);
    body->impulse.x = body->impulse.x + (dt * (body->force.x));
    body->impulse.y = body->impulse.y + (dt * (body->force.y));
    vector_t new_velocity = {old_velocity.x + ((1.0/body->mass) *
      body->impulse.x),
                            old_velocity.y + ((1.0/body->mass) *
                            body->impulse.y)};

    body_set_velocity(body, new_velocity);
    double translate_x = (body->velocity.x + old_velocity.x) * dt * ACC_MULT;
    double translate_y = (body->velocity.y + old_velocity.y) * dt * ACC_MULT;
    vector_t translation_vector = {translate_x, translate_y};


     double old_ang_velo = body->angular_velocity;
     body->angular_impulse = body->angular_impulse + (dt *(body->torque));
     double ang_velo = old_ang_velo + ((1.0/(body->mass * 100)) *
     body->angular_impulse);
     body_set_angular_velocity(body, ang_velo);
     double angle_to_move = body->angular_velocity * dt * 1.0;
     if(angle_to_move != 0.0){
          body_set_rotation(body, angle_to_move);
     }

     body_set_centroid(body, vec_add(body_get_centroid(body),
     translation_vector));
    body->impact_pos = body->centroid;
    body->rotate_point = body->centroid;
    body->force = VEC_ZERO;
    body->impulse = VEC_ZERO;
    body->torque = 0.0;
    body->angular_impulse = 0.0;

    body->scale_factor = 1.0;
  }
}

/**
 * Marks a body for removal--future calls to body_is_removed() will return true.
 * Does not free the body.
 * If the body is already marked for removal, does nothing.
 */
void body_remove(body_t *body) {
  body->removed = true;
}

/**
 * Returns whether a body has been marked for removal.
 * This function returns false until body_remove() is called on the body,
 * and returns true afterwards.
 */
bool body_is_removed(body_t *body){
  return body->removed;
}

void body_collided(body_t *body, bool is_collided) {
  body->collided_with = is_collided;
}

bool body_is_collided(body_t *body) {
  return body->collided_with;
}

void body_set_collision_body(body_t *body, body_t *collided) {
  list_add(body->bodies_collided_with, collided);
}

list_t *body_get_collision_bodies(body_t *body) {
  return body->bodies_collided_with;
}

void body_remove_collision_body(body_t *body, int index){
  list_remove(body->bodies_collided_with, index);
}
