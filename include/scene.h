#ifndef __SCENE_H__
#define __SCENE_H__

#include "body.h"
#include "list.h"


/**
 * A collection of bodies and force creators.
 * The scene automatically resizes to store
 * arbitrarily many bodies and force creators.
 */
typedef struct scene scene_t;

typedef void (*force_creator_t)(void *aux);

/**
* A collection of force creators.  It will hold a list of bodies that the
* force will act on as well as auxillary variables to be passed in
*/
typedef struct force_holder force_holder_t;

/**
 * A function which adds some forces or impulses to bodies,
 * e.g. from collisions, gravity, or spring forces.
 * Takes in an auxiliary value that can store parameters or state.
 **/

/**
 * Allocates memory for an empty scene.
 * Makes a reasonable guess of the number of bodies to allocate space for.
 * Asserts that the required memory is successfully allocated.
 *
 * @return the new scene
 */
scene_t *scene_init(void);

force_holder_t *force_holder_init(
  force_creator_t force,
  free_func_t freer,
  void *aux,
  list_t *bodies
);

force_creator_t get_force(force_holder_t *force);

/**
 * Frees holder slot but not the bodies associated with it
 * Allows us to get rid of something without freeing bodies that
 * dont need freeing
 *
 * @param: a force holder we want to free
 *
 */
void force_holder_free(force_holder_t *force);

/**
 * Returns an aux of a force force_holder
 * Aux hold parameters for the forces that can be passed in
 *
 * @param a force hold whose aux we want
 *
 * @return a void * auxillary struct that hold parameters for the force
 * can be any arbitrary kind of auxillary
 *
 */
void *force_get_aux(force_holder_t *force);


/**
* takes in a force holder and an index and returns the body that the
* force holder has at that index
*
* @param force is a force_holder_t pointer
* @param index is the index
* @return the body at the index
*/
body_t *force_get_body(force_holder_t *force, size_t index);

/**
* returns a list of the bodies that a given force acts on
* @param force is a force_holder_t pointer t
* @return a list of the bodies that the force acts on
*/
list_t *force_get_all_bodies(force_holder_t *force);

/**
 * Releases memory allocated for a given scene
 * and all the bodies and force creators it contains.
 *
 * @param scene a pointer to a scene returned from scene_init()
 */
void scene_free(scene_t *scene);

/**
 * Gets the number of bodies in a given scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @return the number of bodies added with scene_add_body()
 */
size_t scene_bodies(scene_t *scene);

/**
 * Gets the body at a given index in a scene.
 * Asserts that the index is valid.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param index the index of the body in the scene (starting at 0)
 * @return a pointer to the body at the given index
 */
body_t *scene_get_body(scene_t *scene, size_t index);

/**
 * Adds a body to a scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param body a pointer to the body to add to the scene
 */
void scene_add_body(scene_t *scene, body_t *body);

/**
 * @deprecated Use body_remove() instead
 *
 * Removes and frees the body at a given index from a scene.
 * Asserts that the index is valid.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param index the index of the body in the scene (starting at 0)
 */
void scene_remove_body(scene_t *scene, size_t index);

/**
 * @deprecated Use scene_add_bodies_force_creator() instead
 * so the scene knows which bodies the force creator depends on
 */
void scene_add_force_creator(
    scene_t *scene,
    force_creator_t forcer,
    void *aux,
    free_func_t freer
);

/**
 * Adds a force creator to a scene,
 * to be invoked every time scene_tick() is called.
 * The auxiliary value is passed to the force creator each time it is called.
 * The force creator is registered with a list of bodies it applies to,
 * so it can be removed when any one of the bodies is removed.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param forcer a force creator function
 * @param aux an auxiliary value to pass to forcer when it is called
 * @param bodies the list of bodies affected by the force creator.
 *   The force creator will be removed if any of these bodies are removed.
 *   This list does not own the bodies, so its freer should be NULL.
 * @param freer if non-NULL, a function to call in order to free aux
 */
void scene_add_bodies_force_creator(
    scene_t *scene,
    force_creator_t forcer,
    void *aux,
    list_t *bodies,
    free_func_t freer
);

/**
 * Clears a scene of all the bodies and forces associated with it
 * Frees all the information related to it except the shell of the scene
 *
 * @param scene that we want to free
 *
 */
void scene_clear(scene_t *scene);

/**
 * Executes a tick of a given scene over a small time interval.
 * This requires executing all the force creators
 * and then ticking each body (see body_tick()).
 * If any bodies are marked for removal, they should be removed from the scene
 * and freed, along with any force creators acting on them.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param dt the time elapsed since the last tick, in seconds
 */
void scene_tick(scene_t *scene, double dt);

#endif // #ifndef __SCENE_H__
