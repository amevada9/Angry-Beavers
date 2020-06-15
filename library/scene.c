#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "scene.h"
#include "body.h"
#include "polygon.h"
#include "forces.h"
#include "collision.h"

const size_t INIT_SIZE = 5;

/**
 A collection of bodies. The scene automatically resizes to store arbitrarily
 many bodies.
 */
typedef struct scene{
  list_t *scene_forces;
  list_t *bodies;
} scene_t;

typedef struct force_holder{
  force_creator_t force;
  free_func_t freer;
  void *aux;
  list_t *bodies;
} force_holder_t;


/**
Allocates memory for an empty scene. Makes a reasonable guess of the number
of bodies to allocate space for. Asserts that the required memory is
successfully allocated.
*/
scene_t *scene_init(void){
  scene_t *new_scene = malloc(sizeof(scene_t));
  assert(new_scene != NULL);
  new_scene->bodies = list_init(2 * INIT_SIZE, (free_func_t) body_free);
  new_scene->scene_forces = list_init(2 * INIT_SIZE, NULL);
  return new_scene;
}

force_holder_t *force_holder_init(
    force_creator_t force,
    free_func_t freer,
    void *aux,
    list_t *bodies
  ) {

    force_holder_t *force_holder = malloc(sizeof(force_holder_t));
    force_holder->force = force;
    force_holder->freer = freer;
    force_holder->aux = aux;
    force_holder->bodies = bodies;
    return force_holder;
}

force_creator_t get_force(force_holder_t *force){
  return force->force;
}

void force_holder_free(force_holder_t *force) {
  free(force);
}

void *force_get_aux(force_holder_t *force) {
    return force->aux;
  }

// Gets a body from force holder
body_t *force_get_body(force_holder_t *force, size_t index) {
  assert(index >= 0);
  assert(index < list_size(force_get_all_bodies(force)));
  return list_get(force->bodies, index);
}

list_t *force_get_all_bodies(force_holder_t *force) {
  return force->bodies;
}


/**
Releases memory allocated for a given scene and all its bodies.
*/
void scene_free(scene_t *scene){
  list_free(scene->bodies);
  list_free(scene->scene_forces);
  free(scene);
}

/**
Gets the number of bodies in a given scene.
*/
size_t scene_bodies(scene_t *scene){
  return list_size(scene->bodies);
}

/**
Gets the body at a given index in a scene. Asserts that the index is valid.
*/
body_t *scene_get_body(scene_t *scene, size_t index){
  assert(index >= 0);
  assert(index < scene_bodies(scene));
  return (body_t *)list_get(scene->bodies, index);
}

/**
Adds a body to a scene.
*/
void scene_add_body(scene_t *scene, body_t *body){
  list_add(scene->bodies, body);
}

/**
Removes and frees the body at a given index from a scene.
Asserts that the index is valid.
*/
void scene_remove_body(scene_t *scene, size_t index){
  body_remove(scene_get_body(scene, index));
}

void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                             free_func_t freer) {

   list_t *empty_list = list_init(0, free);
   scene_add_bodies_force_creator(scene, forcer, aux, empty_list,
     freer);
}

void scene_add_bodies_force_creator(
  scene_t *scene,
  force_creator_t forcer,
  void *aux,
  list_t *bodies,
  free_func_t freer) {
      force_holder_t *force = force_holder_init(forcer, freer, aux, bodies);
      list_add(scene->scene_forces, force);
}

void scene_clear(scene_t *scene) {
  list_free(scene->bodies);
  list_free(scene->scene_forces);
  scene->bodies = list_init(5, (free_func_t) body_free);
  scene->scene_forces = list_init(5, (free_func_t) force_holder_free);
}

/**
  Executes a tick of a given scene over a small time interval.
  This requires ticking each body in the scene.
*/
void scene_tick(scene_t *scene, double dt) {
  for (int i = 0; i < list_size(scene->scene_forces); i++) {
    force_holder_t *force_holder = (force_holder_t *)list_get(scene->scene_forces, i);
    get_force(force_holder)(force_get_aux(force_holder));
  }

  bool free = false;
  for (size_t i = 0; i < list_size(scene->scene_forces); i++) {
    force_holder_t *force_holder = list_get(scene->scene_forces, i);
    list_t *list_bodies = force_get_all_bodies(force_holder);

    for(size_t j = 0; j < list_size(list_bodies); j++){
      if(body_is_removed(force_get_body(force_holder, j))) {
        free = true;
        break;
      }
    }
    if(free) {
      force_holder_t *removed = (force_holder_t *) list_remove(scene->scene_forces, i);
      force_holder_free(removed);
      i--;
      free = false;
    }
  }

  for(int i = 0; i < scene_bodies(scene); i++) {
    if(body_is_removed(scene_get_body(scene, i))) {
      body_t *removed = (body_t *) list_remove(scene->bodies, i);
      body_free(removed);
      i--;
    }
  }

  for(size_t i = 0; i < scene_bodies(scene); i++) {
     body_tick(scene_get_body(scene, i), dt);
   }
}
