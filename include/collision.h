#ifndef __COLLISION_H__
#define __COLLISION_H__

#include <stdbool.h>
#include "list.h"
#include "vector.h"
#include "body.h"
#include "scene.h"

/**
 * Represents the status of a collision between two shapes.
 * The shapes are either not colliding, or they are colliding along some axis.
 */
 typedef struct {
     /** Whether the two shapes are colliding */
     bool collided;
     /**
      * If the shapes are colliding, the axis they are colliding on.
      * This is a unit vector pointing from the first shape towards the second.
      * Normal impulses are applied along this axis.
      * If collided is false, this value is undefined.
      */
     vector_t axis;
 } collision_info_t;

/**
 * Returns whether a group of bodies that might have
 * collided have actually collided
 * Gets it directly from the given collision_info_t struct
 *
 * @param collision_info that has a bool in it
 * @return boolean whether the bodies collided
 */
bool get_if_collided(collision_info_t collision_info);

/**
 * Returns the axis of which the bodies might have collided
 * Gets it directly from the given collision_info_t struct
 * if they are not collided, will return NULL
 *
 * @param collision_info that has the axis in it
 * @return vector_t of the axis they collided on
 ** might return NULL or empty if there is no collision (be careful!)
 */
vector_t get_collision_axis(collision_info_t collision_info);

/**
 * Sets the collison axis of a collision_info_t to a particular vector_t
 *
 * @param a collision_info_t that we want to pass the axis into
 * @param the vector_t that represents the axis we are dealing with
 */
void set_collision_axis(collision_info_t collision_info, vector_t axis);

/**
 * Sets the collison status to either true or false depending on what is passed
 * in
 *
 * @param a collision_info_t that we want to pass the axis into
 * @param the boolean that we want to set collided to
 */
void set_collided(collision_info_t collision_info, bool collided);
double extrema(double a, double b, int type);
bool onSegment(vector_t p, vector_t q, vector_t r);
int orientation(vector_t p, vector_t q, vector_t r);
bool do_intersect(vector_t p1, vector_t q1, vector_t p2, vector_t q2);
vector_t point_of_intersect(vector_t p1, vector_t q1, vector_t p2, vector_t q2);
vector_t bodies_intersect(body_t *bod1, body_t *bod2);

/**
 * Mathematical function that tries the max or min of a list doubles
 *
 * @param a list of double/int pointers that we want to find
 * the extrema of
 *
 * @param a int type that denotes whether we want a max or a min
 ** @info: 1 denotes a minimum that we want to find
 ** @info 2 denotes we want to find a maximum
 *
 * @return a double that is the given extrema of the list
 */
double find_extrema(list_t *numbers, int type);

/**
* This function will add the magnitude (postive and negative) of a projection
* of each point in a polygon onto a given line.
*
* @param line is a vector that other points will project onto
* @param shape is a pointer to a list that contains the points of a shape
* @param magnitude is a pointer to a list of doubles that will be added to
*/
void add_mag(vector_t line, list_t *shape, list_t *magnitude);

/**
* This function will find add lines that are perpendicular to every edge in the
* passed in polygon to a list
*
* @param perp_vectors is a pointer to a list that will hold vectors
* @param shape is a pointer to a list of vectors that represents a polygon
*/
void perpendicular_lines(list_t *perp_vectors, list_t *shape);

/**
* This function will project every point from both polygons onto a given line.
* It then determines whether these projections overlap. It returns true if they
* do and false if they do not.
*
* @param line is a vector that will be projected onto
* @param shape1 is a list pointer that represents a polygon
* @param shape2 is a list pointer that represents a polygon
* @return true if the polygons overlap when projected onto the given line
* and false if they do not.
*/
double overlap(vector_t line, list_t *shape1, list_t *shape2);


/**
 * Computes the status of the collision between two convex polygons.
 * The shapes are given as lists of vertices in counterclockwise order.
 * There is an edge between each pair of consecutive vertices,
 * and one between the first vertex and the last vertex.
 *
 * @param shape1 the first shape
 * @param shape2 the second shape
 * @return whether the shapes are colliding, and if so, the collision axis.
 * The axis should be a unit vector pointing from shape1 towards shape2.
 */
collision_info_t find_collision(body_t *body1, body_t *body2);

#endif // #ifndef __COLLISION_H__
