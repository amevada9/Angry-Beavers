#ifndef __FORCES_H__
#define __FORCES_H__

#include "scene.h"

/**
 * A function called when a collision occurs.
 * @param body1 the first body passed to create_collision()
 * @param body2 the second body passed to create_collision()
 * @param axis a unit vector pointing from body1 towards body2
 *   that defines the direction the two bodies are colliding in
 * @param aux the auxiliary value passed to create_collision()
 */
typedef void (*collision_handler_t)
    (body_t *body1, body_t *body2, vector_t axis, void *aux);

typedef struct auxillary auxillary_t;

/**
 * Creates an auxillary object that we can pass into calc function
 * to get forces and impulses
 * Makes everything as a pointer
 *
 * @param a cosntant that we want to store in the auxillary_t
 * @return a new auxillary_t pointer
 */
auxillary_t *aux_init(double constant);

/**
 * Sets the collision_handler_t in the auxillary to a given collision handler
 * Allows us to store methods and access them
 *
 * @param aux that we want to store the method in. is an aux_t type
 * @param collide which is a given collision method that we want to execute
 */
void aux_set_collision(auxillary_t *aux, collision_handler_t collide);

/**
 * Returns a collision_handler_t that is stored in an aux
 * Used to get a methods when we call create_collision()
 *
 * @param an auxillary_t that we are storing the method
 */
collision_handler_t aux_get_collision(auxillary_t *aux);

/**
 * Sets the auxillary field in an aux to what we want it to be
 * This we can store client aux in our implementor aux so that we
 * can keep it in case we need the info somewhere
 *
 * @param an old aux that is auxillary_t that we want to store
 * the client aux in
 * @param a void *aux that is the client auxillary
 */
void aux_set_aux(auxillary_t *old_aux, void *new_aux);

/**
 * Returns the aux field of an auxillary_t
 *
 * @param an aux that is auxillary
 */
void *aux_get_aux(auxillary_t *new_aux);


/**
 * Adds a body to given auxillary_t object
 *
 * @param a aux pointer object we want to add the body to
 * @param a body we want to add
 */
void aux_add_body(auxillary_t *aux, body_t *body);

/**
 * Returns a body a from an auxillary_t
 * Used to get the bodies that we need so we can calculate
 * forces and impulses
 *
 * @param aux auxillary_t we want to get the body from
 * @param index of the body we want to get
 *
 * @return a body that we want to get
 */
body_t *aux_get_body(auxillary_t *aux, size_t index);

/**
 * Returns the constant that is stored in the auxillary_t
 * Right now there is no list of constants thus we do not
 * need an index
 *
 * @param the aux we want the constnat from
 *
 * @return the double of the constant that is stored
 */
double aux_get_constant(auxillary_t *aux);

/**
 * Frees the aux that we are dealing with
 * Note: frees all associated bodies along with the aux
 *
 * @param auxillary we want to free
 */
void aux_free(auxillary_t *aux);


/**
 * Adds a force creator to a scene that applies gravity between two bodies.
 * The force creator will be called each tick
 * to compute the Newtonian gravitational force between the bodies.
 * See https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form.
 * The force should not be applied when the bodies are very close,
 * because its magnitude blows up as the distance between the bodies goes to 0.
 *
 * @param scene the scene containing the bodies
 * @param G the gravitational proportionality constant
 * @param body1 the first body
 * @param body2 the second body
 */
void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2);

/**
 * This is a method that calcualtes the forces on the bodies passed through
 * create_newtonian_gravity and applies the force to the bodies.
 *
 * @param: forcer, a pointer to a force_holder_t that has the following
 ** @contains void *aux for the constant
 ** @contains a list of bodies that the force will act one
 ** @contains a force creator that specifies the forcer
 ** @contains a freer for the void star
 *
 * @return void
 */
void calc_gravity_force(void *aux);

/**
 * Adds a force creator to a scene that applies gravity between two bodies,
 adjusted to be realistic for a scene of earth.
 * The force creator will be called each tick
 * to compute the Newtonian gravitational force between the bodies.
 * See https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form.
 * The force should not be applied when the bodies are very close,
 * because its magnitude blows up as the distance between the bodies goes to 0.
 *
 * @param scene the scene containing the bodies
 * @param G the gravitational proportionality constant
 * @param body1 the first body
 * @param body2 the second body
 */
void create_earth_gravity(scene_t *scene, double G, body_t *body1, body_t *body2);

/**
 * This is a method that calcualtes the forces on the bodies passed through
 * create_earth_gravity and applies the force to the bodies.
 *
 * @param: forcer, a pointer to a force_holder_t that has the following
 ** @contains void *aux for the constant
 ** @contains a list of bodies that the force will act one
 ** @contains a force creator that specifies the forcer
 ** @contains a freer for the void star
 *
 * @return void
 */
void calc_earth_force(void *aux);

/**
 * Adds a force creator to a scene that acts like a spring between two bodies.
 * The force creator will be called each tick
 * to compute the Hooke's-Law spring force between the bodies.
 * See https://en.wikipedia.org/wiki/Hooke%27s_law.
 *
 * @param scene the scene containing the bodies
 * @param k the Hooke's constant for the spring
 * @param body1 the first body
 * @param body2 the second body
 */
void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2);

/**
 * This is a method that calcualtes the forces on the bodies passed through
 * create_spring and applies the force to the bodies.
 *
 * @param: forcer, a pointer to a force_holder_t that has the following
 ** @contains void *aux for the constant
 ** @contains a list of bodies that the force will act one
 ** @contains a force creator that specifies the forcer
 ** @contains a freer for the void star
 *
 * @return void
 */
void calc_spring_force(void *aux);



/**
 * Adds a force creator to a scene that applies a drag force on a body.
 * The force creator will be called each tick
 * to compute the drag force on the body proportional to its velocity.
 * The force points opposite the body's velocity.
 *
 * @param scene the scene containing the bodies
 * @param gamma the proportionality constant between force and velocity
 *   (higher gamma means more drag)
 * @param body the body to slow down
 */
void create_drag(scene_t *scene, double gamma, body_t *body);

/**
 * This is a method that calcualtes the forces on the bodies passed through
 * create_drag and applies the force to the bodies.
 *
 * @param: forcer, a pointer to a force_holder_t that has the following
 ** @contains void *aux for the constant
 ** @contains a list of bodies that the force will act one
 ** @contains a force creator that specifies the forcer
 ** @contains a freer for the void star
 *
 * @return void
 */
void calc_drag_force(void *aux);


/**
 * Adds a force creator to a scene that calls a given collision handler
 * function each time two bodies collide.
 * This generalizes create_destructive_collision() from last week,
 * allowing different things to happen on a collision.
 * The handler is passed the bodies, the collision axis, and an auxiliary value.
 * It should only be called once while the bodies are still colliding.
 *
 * @param scene the scene containing the bodies
 * @param body1 the first body
 * @param body2 the second body
 * @param handler a function to call whenever the bodies collide
 * @param aux an auxiliary value to pass to the handler
 * @param freer if non-NULL, a function to call in order to free aux
 */
void create_collision(
    scene_t *scene,
    body_t *body1,
    body_t *body2,
    collision_handler_t handler,
    void *aux,
    free_func_t freer
);

/**
 * Calculates the collision using the force handler
 * Allows us to have the abstraction for the collisions using the
 * collision_handler_t and its parameters. Calls methods that will add
 * the forces to the body.
 *
 * @param an aux that carries all the things that we need. In create
 * collision we will use auxillary_t to transfer information from the
 * user aux to the implementor aux allow us to interface between the
 * layers.
 */
void calc_collison(void *aux);

/**
 * Adds a force creator to a scene that destroys two bodies when they collide.
 * The bodies should be destroyed by calling body_remove().
 * This should be represented as an on-collision callback
 * registered with create_collision().
 *
 * @param scene the scene containing the bodies
 * @param body1 the first body
 * @param body2 the second body
 */
void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2);

/**
 * This is a method that marks the bodies passed throguh
 * create_destructive_collision as removed.
 *
 * @param body1 the first body
 * @param body2 the second body
 * @param axis of intersection
 * @param user auxillary for any extra information that is needed.
 *
 * @return void
 */
void calc_destructive_force(body_t *body1, body_t *body2, vector_t axis, void *aux);


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
 *
 * @param scene the scene containing the bodies
 * @param elasticity the "coefficient of restitution" of the collision;
 * 0 is a perfectly inelastic collision and 1 is a perfectly elastic collision
 * @param body1 the first body
 * @param body2 the second body
 */
void create_physics_collision(
    scene_t *scene,
    double elasticity,
    body_t *body1,
    body_t *body2
);

/**
 * Calculates the impulses associated with a given collision
 * Uses the aux to get the bodies and constants needed
 * Gets the axis by getting the collision between the two
 * Might need to be changed.
 *
 * @param body1 the first body
 * @param body2 the second body
 * @param axis of intersection
 * @param user auxillary for any extra information that is needed.
 *
 * @return void
 */
void calc_physics_collision(body_t *body1, body_t *body2, vector_t axis, void *aux);


/**
 * Calculates the magnitude of the impulse associated with a given collision
 * Uses the aux to get the bodies and constants needed
 *
 * @param body1 the first body
 * @param body2 the second body
 * @param axis of intersection
 * @param user auxillary for any extra information that is needed.
 *
 * @return double
 *
 */
 double impulse_mag(body_t *body1, body_t *body2, vector_t axis, void *aux);

#endif // #ifndef __FORCES_H__
