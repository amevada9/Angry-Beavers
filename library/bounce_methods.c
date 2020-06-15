#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "polygon.h"
#include "bounce_methods.h"
#include "list.h"

//============================================================================//
// This module holds methods that are repeated throughout the various demos
// but are not associated with a library module, a struct, or a force
//============================================================================//

const double TIP_LENGTH_SCALE = 10.0;
const double BEND_LENGTH_SCALE = 25.0;
const int MIN_RAD = 10;
const int MAX_RAD = 35;
const double BEND_SCALAR = 0.4;

/**
 * Returns a random integer between two given integers
*/
int random_number(int lower, int upper){
  int num = (rand() % (upper - lower + 1)) + lower;
  return num;
}

/**
 * Returns a random double between 0 and 1 so it can be put
*/
double get_color(void) {
  double toReturn = rand()/(double)RAND_MAX;
  return toReturn;
}

/**
 * Function that creates a star of a provided number of sides and at a given
 center
*/
list_t *create_star(int sides, vector_t center) {
  double starTipLength = (double) random_number(MIN_RAD, MAX_RAD);
  double starBendLength = starTipLength * BEND_SCALAR;
  list_t *listOfPoints = list_init(sides * 2, (free_func_t) vec_free);
  vector_t *startingPoint = malloc(sizeof(vector_t));
  *startingPoint = vec_init(center.x, center.y);
  double angleToRotate = FULL_CIRCLE / (sides * 2);
  for (int i = 0; i < sides * 2; i++) {
    if(i % 2 == 0) {
      vector_t *starTip = malloc(sizeof(vector_t));
      *starTip = vec_init(startingPoint->x, startingPoint->y + starTipLength);
      list_add(listOfPoints, starTip);
    }
    else {
      vector_t *starBend = malloc(sizeof(vector_t));
      *starBend = vec_init(startingPoint->x, startingPoint->y + starBendLength);
      list_add(listOfPoints, starBend);
    }
    polygon_rotate(listOfPoints, -angleToRotate, *startingPoint);
  }
  free(startingPoint);
  return listOfPoints;
}
