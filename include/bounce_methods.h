#ifndef __BOUNCE_METHODS_H__
#define __BOUNCE_METHODS_H__

#include <stddef.h>
#include "vector.h"
#include "polygon.h"
#include "list.h"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 500
#define FULL_CIRCLE 6.28319
#define NEGATE (-1)

/**
 * Returns a random integer between two given integers to be used in other
 * functions such as create_star
 * Placed here because it is used in many demos and is useful
 *
 * @param lower: the lower bound of the random integer
 * @param upper: the upper bound of the random integer
 *
 * @return a random int between the lower and upper
*/
int random_number(int lower, int upper);

/**
 * Returns a random double between 0 and 1 so it can be put
 * into the rgb_color_t object for our Bodies
 * Placed here because it is used in many demos and is useful
 *
 * @param void
 *
 * @return a random double between zero and 1
*/
double get_color(void);

/**
 * Function that creates a star of a provided number of sides.  It takes in
 * a number of sides for the star and places a star in the top left corner of
 * the screen.
 *
 * @param sides: a size_t that represents the number of sides of the star
 *
 * @return a vec_list_t that contains all the vertices of the star
 *
*/
list_t *create_star(int sides, vector_t center);

#endif // #ifndef __BOUNCE_METHODS_H__
