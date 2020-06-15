#ifndef __BODY_H__
#define __BODY_H__

#include <stdbool.h>
#include "color.h"
#include "list.h"
#include "vector.h"

/**
 * A rigid body constrained to the plane.
 * Implemented as a polygon with uniform density.
 * Bodies can accumulate forces and impulses during each tick.
 * Angular physics (i.e. torques) are not currently implemented.
 */
typedef struct body body_t;

/**
 * Allocates memory for a body with the given parameters.
 * The body is initially at rest.
 * Asserts that the mass is positive and that the required memory is allocated.
 *
 * @param shape a list of vectors describing the initial shape of the body
 * @param mass the mass of the body (if INFINITY, stops the body from moving)
 * @param color the color of the body, used to draw it on the screen
 * @param info additional information to associate with the body,
 *   e.g. its type if the scene has multiple types of bodies
 * @param info_freer if non-NULL, a function call on the info to free it
 * @return a pointer to the newly allocated body
 */
body_t *body_init_with_info(
    list_t *shape,
    double mass,
    rgb_color_t color,
    void *info,
    free_func_t info_freer
);

/**
 * Initializes a body without any info.
 * Acts like body_init_with_info() where info and info_freer are NULL.
 */
body_t *body_init(list_t *shape, double mass, rgb_color_t color);



/**
 * Releases the memory allocated for a body.
 *
 * @param body a pointer to a body returned from body_init()
 */
void body_free(body_t *body);

/**
 * Gets the current shape of a body.
 * Returns a newly allocated vector list, which must be list_free()d.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the polygon describing the body's current position
 */
list_t *body_get_shape(body_t *body);

/**
 * Gets the current center of mass of a body.
 * While this could be calculated with polygon_centroid(), that becomes too slow
 * when this function is called thousands of times every tick.
 * Instead, the body should store its current centroid.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's center of mass
 */
vector_t body_get_centroid(body_t *body);

/**
 * Gets the current velocity of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's velocity vector
 */
vector_t body_get_velocity(body_t *body);

/**
 * Gets the current force on a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's force vector
 */
vector_t body_get_force(body_t *body);

/**
 * Gets the mass of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the mass passed to body_init(), which must be greater than 0
 */
double body_get_mass(body_t *body);

/**
 * Gets the display color of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the color passed to body_init(), as an (R, G, B) tuple
 */
rgb_color_t body_get_color(body_t *body);

/**
 * Gets the information associated with a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the info passed to body_init()
 */
void *body_get_info(body_t *body);

/**
 * Translates a body to a new position.
 * The position is specified by the position of the body's center of mass.
 *
 * @param body a pointer to a body returned from body_init()
 * @param x the body's new centroid
 */
void body_set_centroid(body_t *body, vector_t x);

/**
* Sets a body's point list.
*
* @param body a pointer to a body returned from body_init()
* @param list the list to set the points to.
*/
void body_set_points(body_t *body, list_t *list);

/**
 *  Gets a body's launched position.
 *
 * @param body a pointer to a body returned from body_init()
 */
bool body_get_launched(body_t *body);

/**
 *  Sets a body's launched status.
 *
 * @param body a pointer to a body returned from body_init()
 * @param truth the value of launched.
 */
void body_set_launched(body_t *body, bool truth);

/**
 *  Gets a body's impact position
 *
 * @param body a pointer to a body returned from body_init()
 */
vector_t body_get_imp_pos(body_t *body) ;


/**
 * Changes a body's velocity (the time-derivative of its position).
 *
 * @param body a pointer to a body returned from body_init()
 * @param v the body's new velocity
 */
void body_set_velocity(body_t *body, vector_t v);

/**
 * Changes a body' angular velocity.
 *
 * @param body a pointer to a body returned from body_init()
 * @param v the body's new angular velocity
 */
void body_set_angular_velocity(body_t *body, double v);

/**
 * Sets a position of impact for the body.
 *
 * @param body a pointer to a body returned from body_init()
 * @param pos a vector that represents the position of impact
 */
void body_set_impact_pos(body_t *body, vector_t pos);
/**
 * Changes a body's orientation in the plane.
 * The body is rotated about its center of mass.
 * Note that the angle to move is relative to the current orientation.
 *
 * @param body a pointer to a body returned from body_init()
 * @param angle_to_rotate raidian angle to rotate body. Positive is
 * counterclockwise.
 */
void body_set_rotation(body_t *body, double angle_to_rotate);

/**
 *  Adds torque to a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @param add torque value to be added
 */
void body_add_torque(body_t *body, double add);

/**
 *  Sets a body's torque.
 *
 * @param body a pointer to a body returned from body_init()
 * @param add torque to be set to.
 */
void body_set_torque(body_t *body, double add);

/**
 *  Gets a body's torque.
 *
 * @param body a pointer to a body returned from body_init()
 * @return torque value
 */
double body_get_torque(body_t *body);

/**
 * Gets a body's rotation point.
 *
 * @param body a pointer to a body returned from body_init()
 * @return rotation point.
 */
vector_t body_get_rot_point(body_t *body);

/**
 *  Gets a body's anuglar velocity.
 *
 * @param body a pointer to a body returned from body_init()
 * @return angular velocity.
 */
double body_get_angular_velocity(body_t *body);

/**
 *  Sets a body's rotation point .
 *
 * @param body a pointer to a body returned from body_init()
 * @param rotation_point vector to set rot pt to.
 */

void body_set_rotation_point(body_t* body, vector_t rotation_point);
/**
 * Applies a force to a body over the current tick.
 * If multiple forces are applied in the same tick, they should be added.
 * Should not change the body's position or velocity; see body_tick().
 *
 * @param body a pointer to a body returned from body_init()
 * @param force the force vector to apply
 */
void body_add_force(body_t *body, vector_t force);

/**
 *  Adds a force to specified impact position.
 *
 * @param body a pointer to a body returned from body_init()
 * @param force vector to add to force field.
 * @param imp_pos position to set as impact position.
 */
void body_add_force_imp_pos(body_t *body, vector_t force, vector_t imp_pos);

/**
 *  Sets a body's angular impulse.
 *
 * @param body a pointer to a body returned from body_init()
 * @param v new value for angular impulse.
 */
void body_set_angular_impulse(body_t *body, double v);

/**
 *  Adds to a body's angular impulse.
 *
 * @param body a pointer to a body returned from body_init()
 * @param v impulse to add.
 */
void body_add_angular_impulse(body_t *body, double v);

/**
 * Applies an impulse to a body.
 * An impulse causes an instantaneous change in velocity,
 * which is useful for modeling collisions.
 * If multiple impulses are applied in the same tick, they should be added.
 * Should not change the body's position or velocity; see body_tick().
 *
 * @param body a pointer to a body returned from body_init()
 * @param impulse the impulse vector to apply
 */
void body_add_impulse(body_t *body, vector_t impulse);

/**
 *  Adds an impulse to specified impact position.
 *
 * @param body a pointer to a body returned from body_init()
 * @param impulse vector to add to impulse field.
 * @param imp_pos position to set as impact position.
 */
void body_add_impulse_impact_pos(body_t *body, vector_t impulse, vector_t imp_pos);


/*
Finds the angle between two vectors.
@param one vector
@param two another vector
@returns double representing angle between two vectors
*/
double ang_diff(vector_t one, vector_t two);

/**
 * Updates the body after a given time interval has elapsed.
 * Sets acceleration and velocity according to the forces and impulses
 * applied to the body during the tick.
 * The body should be translated at the *average* of the velocities before
 * and after the tick.
 * Resets the forces and impulses accumulated on the body.
 *
 * @param body the body to tick
 * @param dt the number of seconds elapsed since the last tick
 */
void body_tick(body_t *body, double dt);

/**
 * Marks a body for removal--future calls to body_is_removed() will return true.
 * Does not free the body.
 * If the body is already marked for removal, does nothing.
 *
 * @param body the body to mark for removal
 */
void body_remove(body_t *body);

/**
 * Returns whether a body has been marked for removal.
 * This function returns false until body_remove() is called on the body,
 * and returns true afterwards.
 *
 * @param body the body to check
 * @return whether body_remove() has been called on the body
 */
bool body_is_removed(body_t *body);

/**
 * Sets the collided status of a body to the given status
 *
 * @param body that we want to change
 * @param bool that we want to set the status equal
 */
void body_collided(body_t *body, bool is_collided);

/**
 * Gets whether the body has collided with anything
 * Will we used to prevent double addition of impulses
 *
 * @param body a pointer to a body that we want to change
 * @return bool that says whether the body is colliding or not
 */
bool body_is_collided(body_t *body);


/**
* Will take in two bodies that collided in the last tick and store the second
* one in the first one's "bodies collided with" list.  No return type
*
* @param body is a body_t *
* @param collided is another body_t * that is added to body's list of bodies collided with
*/
void body_set_collision_body(body_t *body, body_t *collided);

/**
* Will return a list of the bodies that a certain body collided with last tick
*
* @param body is a body_t *
* @return a list_t * of the bodies that this body collided with
*/
list_t *body_get_collision_bodies(body_t *body);

/**
* finds the list of bodies that certain body collided with last tick and will
* return the body at the given index in that list
*
* @param body is a body_t *
* @param index is an integer that is the index in the list
*/
void body_remove_collision_body(body_t *body, int index);


#endif // #ifndef __BODY_H__
