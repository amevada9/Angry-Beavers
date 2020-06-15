#include "list.h"
#include "vector.h"
#include "polygon.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const double CENTROID_CONST = 6.0;
const double SUM_SCALE = 0.5;

/**
Computes the area of a polygon.
*/
double polygon_area(list_t *polygon) {
  double sum = 0.0;
  size_t size = list_size(polygon);
  for (size_t i = 0; i < size; i++) {
      vector_t one = *((vector_t*)list_get(polygon, i));
      vector_t two =  *((vector_t*)list_get(polygon, ((i+1) % size)));
      sum += SUM_SCALE * vec_cross(one, two);
  }
  return fabs(sum);
}


/**
Computes the center of mass of a polygon.
*/
vector_t polygon_centroid(list_t *polygon) {
  double area = polygon_area(polygon);
  vector_t centroid = {0, 0};
  for(size_t i = 0; i < list_size(polygon); i++) {
    vector_t one =  *((vector_t*) list_get(polygon, i));
    vector_t two =
      *((vector_t*) list_get(polygon, (i + 1) % list_size(polygon)));
    double cross = vec_cross(one, two);
    centroid.x += (one.x + two.x) * cross;
    centroid.y += (one.y + two.y) * cross;
  }
  if(area == 0) {
    return centroid;
  }
  else {
    centroid.x /= (CENTROID_CONST * area);
    centroid.y /= (CENTROID_CONST * area);
  return centroid;
  }
}

/**
Translates all vertices in a polygon by a given vector.
*/
void polygon_translate(list_t *polygon, vector_t translation) {
  for (size_t i = 0; i < list_size(polygon); i++) {
    vector_t temp = vec_add(*((vector_t*)list_get(polygon, i)), translation);
    *((vector_t*)list_get(polygon, i)) = temp;
  }
}

/**
Rotates vertices in a polygon by a given angle about a given point.
*/
void polygon_rotate(list_t *polygon, double angle, vector_t point) {
  vector_t translator = point;
  polygon_translate(polygon, vec_negate(translator));
  for(size_t i = 0; i < list_size(polygon); i++) {
    vector_t temp = vec_rotate(*((vector_t*)list_get(polygon, i)), angle);
    *((vector_t*)list_get(polygon, i)) = temp;
  }
  polygon_translate(polygon, translator);
}
