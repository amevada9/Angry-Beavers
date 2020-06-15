#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "forces.h"
#include "vector.h"
#include "body.h"
#include "scene.h"
#include "collision.h"

const double ELASTICITY_TERM = 1.0;
const double FALSE_CONSTANT = -1.0;

typedef struct auxillary {
  double constant;
  list_t *bodies;
  collision_handler_t collision;
  void *aux;
} auxillary_t;

/**
 * Creates an auxillary object that we can pass into calc function
 * to get forces and impulses
 * Makes everything as a pointer
 */
auxillary_t *aux_init(double constant) {
  auxillary_t *new_aux = malloc(sizeof(auxillary_t));
  new_aux->bodies = list_init(2, (free_func_t) body_free);
  new_aux->constant = constant;
  new_aux->collision = NULL;
  return new_aux;
}

/**
 * Sets the collision_handler_t in the auxillary to a given collision handler
 * Allows us to store methods and access them
 */
void aux_set_collision(auxillary_t *aux, collision_handler_t collide) {
  aux->collision = collide;
}

/**
 * Returns a collision_handler_t that is stored in an aux
 * Used to get a methods when we call create_collision()
 */
collision_handler_t aux_get_collision(auxillary_t *aux){
  return aux->collision;
}

/**
 * Sets the auxillary field in an aux to what we want it to be
 * This we can store client aux in our implementor aux so that we
 * can keep it in case we need the info somewhere
 */
void aux_set_aux(auxillary_t *old_aux, void *new_aux) {
  old_aux->aux = new_aux;
}

/**
 * Returns the aux field of an auxillary_t
 */
void *aux_get_aux(auxillary_t *new_aux){
  return new_aux->aux;
}

/**
 * Adds a body to given auxillary_t object
 */
void aux_add_body(auxillary_t *aux, body_t *body) {
  list_add(aux->bodies, body);
}

/**
 * Returns a body a from an auxillary_t
 * Used to get the bodies that we need so we can calculate
 * forces and impulses
 */
body_t *aux_get_body(auxillary_t *aux, size_t index) {
  assert(index >= 0);
  assert(index < list_size(aux->bodies));
  return list_get(aux->bodies, index);
}

/**
 * Returns the constant that is stored in the auxillary_t
 * Right now there is no list of constants thus we do not
 * need an index
 */
double aux_get_constant(auxillary_t *aux) {
  return aux->constant;
}

/**
 * Frees the aux that we are dealing with
 * Note: frees all associated bodies along with the aux
 */
void aux_free(auxillary_t *aux) {
  list_free(aux->bodies);
  free(aux);
}

//GRAVITY HELPER FUNCTIONS

//Returns a list of all the y coordinates of a body's shape
list_t *y_vals(body_t *body){
  list_t *shape = body_get_shape(body);
  list_t *values = list_init(5, NULL);
  for(size_t i = 0; i < list_size(shape); i++){
    double *y = malloc(sizeof(double));
    *y = (*(vector_t*) list_get(shape, i)).y;
    list_add(values, y);
  }
  return values;
}

//Finds vector difference between centroids
vector_t diff_centroids(body_t *body1, body_t *body2){
  vector_t centroid_body_1 = body_get_centroid(body1);
  vector_t centroid_body_2 = body_get_centroid(body2);
  return vec_subtract(centroid_body_2, centroid_body_1);
}

//Helper function to calculate gravity force
void grav_calc_helper(double constant, body_t *body1, body_t *body2){

  double distance_bodies = vec_magnitude(diff_centroids(body1, body2));

  // Calculating values needed to find mutual grav force.
  double mass_body_1 = body_get_mass(body1);
  double mass_body_2 = body_get_mass(body2);
  double magnitude_of_force = (constant * ((double) mass_body_1 *
    (double) mass_body_2)) /
    (distance_bodies * distance_bodies * distance_bodies);

  vector_t force = vec_multiply(magnitude_of_force,
    diff_centroids(body1, body2));

  vector_t force_body_1 = vec_init(force.x *.1 , force.y* .1);
  vector_t force_body_2 = vec_negate(force_body_1);

  body_add_force_imp_pos(body1, force_body_1, body_get_imp_pos(body1));
  body_add_force_imp_pos(body2, force_body_2, body_get_imp_pos(body2));
}

/**
 * Creates a gravitational force creator on a body.
 */
void create_newtonian_gravity(scene_t *scene, double G, body_t *body1,
  body_t *body2) {
  auxillary_t *aux = aux_init(G);
  aux_add_body(aux, body1);
  aux_add_body(aux, body2);
  list_t *bodies = list_init(2, (free_func_t) body_free);
  list_add(bodies, body1);
  list_add(bodies, body2);

  scene_add_bodies_force_creator(scene, (force_creator_t) calc_gravity_force,
  aux, bodies,(free_func_t) aux_free);
}

/**
 * Calculates the gravitational force. Helper funciton to
 * create_newtonian_gravity.
 */
void calc_gravity_force(void *aux) {

  body_t *body1 = aux_get_body(aux, 0);
  body_t *body2 = aux_get_body(aux, 1);

  vector_t body_tip1 = *(vector_t *) list_get(body_get_shape(body1), 0);
  vector_t body_tip2 = *(vector_t *) list_get(body_get_shape(body2), 0);

  vector_t radius1 = vec_subtract(body_get_centroid(body1), body_tip1);
  vector_t radius2 = vec_subtract(body_get_centroid(body2), body_tip2);

  double touching = vec_magnitude(radius1) + vec_magnitude(radius2);

    if(vec_magnitude(diff_centroids(body1, body2)) >= touching){
        grav_calc_helper(aux_get_constant(aux), body1, body2);
    }
}

/**
 * Creates a gravitational force creator on a body that simulates Earth gravity.
 */
void create_earth_gravity(scene_t *scene, double G, body_t *body1,
  body_t *body2) {
  auxillary_t *aux = aux_init(G);
  aux_add_body(aux, body1);
  aux_add_body(aux, body2);
  list_t *bodies = list_init(2, (free_func_t) body_free);
  list_add(bodies, body1);
  list_add(bodies, body2);

  scene_add_bodies_force_creator(scene, (force_creator_t) calc_earth_force,
  aux, bodies,(free_func_t) aux_free);
}

/**
 * Calculates the gravitational force. Helper funciton to create_earth_gravity.
 */
void calc_earth_force(void *aux) {
    body_t *body1 = aux_get_body(aux, 0);
    body_t *body2 = aux_get_body(aux, 1);
      //here, the body is the shape. will find min
    double min = find_extrema(y_vals(body1), 1);
    // here, the body is the floor. will find max
    double max = find_extrema(y_vals(body2), 2);
    if(min > max){
      grav_calc_helper(aux_get_constant(aux), body1, body2);
    }
    else{
      //Mimics slowing down ("friction") when body reaches ground and also
      //stops body's motion in the y-direction
      body_set_velocity(body1, vec_init(0.98 * body_get_velocity(body1).x,0));
    }
}

/**
 * Creates a spring force creator on a body.
 */
void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2) {
  auxillary_t *aux = aux_init(k);
  aux_add_body(aux, body1);
  aux_add_body(aux, body2);
  list_t *bodies = list_init(2, (free_func_t) body_free);
  list_add(bodies, body1);
  list_add(bodies, body2);

  scene_add_bodies_force_creator(scene, (force_creator_t) calc_spring_force,
  aux, bodies, (free_func_t) aux_free);

}

/**
 * Calculates the spring force. Helper funciton to create_spring.
 */
void calc_spring_force(void *aux) {
  double k = aux_get_constant(aux);
  body_t *body1 = aux_get_body(aux, 0);
  body_t *body2 = aux_get_body(aux, 1);

  vector_t centroid1 = body_get_centroid(body1);
  vector_t centroid2 = body_get_centroid(body2);
  vector_t difference = vec_subtract(centroid1, centroid2);
  vector_t force_on_2 = vec_multiply(k, difference);
  vector_t force_on_1 = vec_negate(force_on_2);

  body_add_force_imp_pos(body1, force_on_1, body_get_centroid(body1));
  body_add_force_imp_pos(body2, force_on_2, body_get_centroid(body2));

}

/**
 * Creates a drag force creator on a body.
 */
void create_drag(scene_t *scene, double gamma, body_t *body) {
  auxillary_t *aux = aux_init(gamma);
  aux_add_body(aux, body);
  list_t *bodies = list_init(1, (free_func_t) body_free);
  list_add(bodies, body);
  scene_add_bodies_force_creator(scene, (force_creator_t) calc_drag_force,
  aux, bodies, (free_func_t) aux_free);

}
/**
 * Calculates the drag force. Helper funciton to create_drag.
 */
void calc_drag_force(void *aux) {
  double gamma = aux_get_constant(aux);
  body_t *body = aux_get_body(aux, 0);
  vector_t velo = body_get_velocity(body);
  vector_t drag = vec_multiply(-1.0 * gamma, velo);
  body_add_force_imp_pos(body, drag, body_get_centroid(body));
}

/**
 * Adds a force creator to a scene that calls a given collision handler
 * function each time two bodies collide.
 * This generalizes create_destructive_collision() from last week,
 * allowing different things to happen on a collision.
 * The handler is passed the bodies, the collision axis, and an auxiliary value.
 * It should only be called once while the bodies are still colliding.
 */
void create_collision(
    scene_t *scene,
    body_t *body1,
    body_t *body2,
    collision_handler_t handler,
    void *aux,
    free_func_t freer
)
  {
    auxillary_t *new_aux = aux_init(FALSE_CONSTANT);
    aux_add_body(new_aux, body1);
    aux_add_body(new_aux, body2);
    list_t *bodies = list_init(1, (free_func_t) body_free);
    list_add(bodies, body1);
    list_add(bodies, body2);
    aux_set_collision(new_aux, handler);
    aux_set_aux(new_aux, aux);

    scene_add_bodies_force_creator(scene, (force_creator_t) calc_collison,
    new_aux, bodies, freer);
  }

  /**
   * Calculates the collision using the force handler
   * Allows us to have the abstraction for the collisions using the
   * collision_handler_t and its parameters. Calls methods that will add
   * the forces to the body.
   */
void calc_collison(void *aux) {
  body_t *body1 = aux_get_body(aux, 0);
  body_t *body2 = aux_get_body(aux, 1);
  collision_info_t info = find_collision(body1, body2);
  void *other_aux = aux_get_aux(aux);
  if (get_if_collided(info)) {
    vector_t axis = get_collision_axis(info);
    (aux_get_collision(aux))(body1, body2, axis, other_aux);
  }
}

/**
 * Adds a force creator to a scene that destroys two bodies when they collide.
 * The bodies should be destroyed by calling body_remove().
 */
void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2){
    create_collision(scene, body1, body1, calc_destructive_force, NULL, NULL);
}

/**
 * Marks the bodies as removed as passed in from create_destructive_collision.
 */
void calc_destructive_force(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    body_remove(body1);
    body_remove(body2);
}

/**
 * Adds a force creator to a scene that applies impulses
 * to resolve collisions between two bodies in the scene.
 * This should be represented as an on-collision callback
 * registered with create_collision().
 *
 * You may remember from project01 that you should avoid applying impulses
 * multiple times while the bodies are still colliding.
 * You should also have a special case that allows either body1 or body2
 * to have mass INFINITY, as this is useful for simulating walls.
 */
void create_physics_collision(
    scene_t *scene,
    double elasticity,
    body_t *body1,
    body_t *body2
){
    auxillary_t *new_aux = aux_init(elasticity);
    aux_add_body(new_aux, body1);
    aux_add_body(new_aux, body2);
    list_t *bodies = list_init(1, (free_func_t) body_free);
    list_add(bodies, body1);
    list_add(bodies, body2);

    create_collision(scene, body1, body2, calc_physics_collision, new_aux, NULL);
  }

  /**
   * Calculates the impulses associated with a given collision
   * Uses the aux to get the bodies and constants needed
   * Gets the axis by getting the collision between the two
   * Might need to be changed.
   */
  void calc_physics_collision(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    bool can_proceed = true;
    list_t *collidedWith = body_get_collision_bodies(body1);
    for (size_t i = 0; i < list_size(collidedWith); i++) {
      body_t *collided = (body_t *)list_get(collidedWith, i);
      if (collided == body2) {
          can_proceed = false;
          break;
      }
    }

    if (can_proceed) {
      double impulse_magnitude = impulse_mag(body1, body2, axis, aux);
      vector_t impulse = vec_multiply(impulse_magnitude, axis);

      body_add_impulse(body1, vec_multiply(1, impulse));
      body_add_impulse(body2, vec_multiply(-1, impulse));

      vector_t intersect = bodies_intersect(body1, body2);

      body_set_impact_pos(body1,intersect);
      body_set_impact_pos(body2,intersect);
      vector_t radial_line = vec_subtract(body_get_imp_pos(body2), body_get_centroid(body2));
      double angle_btwn = ang_diff(impulse, radial_line);

      body_collided(body1, true);
      body_collided(body2, true);

      body_set_collision_body(body1, body2);
      body_set_collision_body(body2, body1);


      if(intersect.x != 0.0 || intersect.y != 0.0){

        double distance = vec_magnitude(radial_line);
        if(((body_get_imp_pos(body2).x == body_get_centroid(body2).x) && (body_get_imp_pos(body2).y == body_get_centroid(body2).y))){
          distance = 0.0;
        }

        double rotational = vec_magnitude(impulse) * sin(angle_btwn);

        body_add_angular_impulse(body1, rotational);
        body_add_angular_impulse(body2, rotational);
    }
  }
}

/**
 * Calculates the magnitude of the impulse associated with a given collision
 * Uses the aux to get the bodies and constants needed
 */
  double impulse_mag(body_t *body1, body_t *body2, vector_t axis, void *aux){
      double elasticity = aux_get_constant(aux);
      double body1_mass = body_get_mass(body1);
      double body2_mass = body_get_mass(body2);
      //reduced mass is calculated, taking into account the cases where one body
      //has mass infinity
      double reduced_mass = 0.0;
      if (body1_mass == INFINITY) {
        reduced_mass = body2_mass;
      }
      else if(body2_mass == INFINITY) {
        reduced_mass = body1_mass;
      }
      else {
        reduced_mass = (body1_mass * body2_mass) / (body1_mass + body2_mass);
      }

      double elasticity_term = ELASTICITY_TERM + elasticity;
      double body1_velocity = vec_dot(body_get_velocity(body1), axis);
      double body2_velocity = vec_dot(body_get_velocity(body2), axis);
      double velocity_term = body2_velocity - body1_velocity;

      return reduced_mass * elasticity_term * velocity_term;
  }
