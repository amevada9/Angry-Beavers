#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "scene.h"
#include "forces.h"
#include "list.h"
#include "bounce_methods.h"
#include "sdl_wrapper.h"

const int NUM_BODIES = 50;
const double MASS = 10;
const double CIRCLE_INCREMENT = 600;
const double SPRING_CONSTANT = 10.0;
const vector_t INITIAL_VELO = {0, 100};
const vector_t WINDOW = {1000, 500};
const vector_t INIT_CENTER = {500, 250};
const double ACCELERATION = 100;
const double PI = 3.14159265358979323846;
const double FACTOR_MOD = 0.01;

// This function creates a circle given a radius and a center
// Also given a list that we want to add the points to
// and a circ frac of how much of the circle we want
void draw_circle(double circ_frac, vector_t center, double radius,
  list_t *list) {
  double dtheta = 2 * PI / CIRCLE_INCREMENT;
  double theta = 0;
  double origTheta = circ_frac * PI / 4;
  for (int i = 0; i < CIRCLE_INCREMENT * circ_frac;  i++){
    theta = origTheta + i * dtheta;
    vector_t *add = malloc(sizeof(vector_t));
    assert(add != NULL);
    add->x = center.x + radius * cos(theta);
    add->y = center.y + radius * sin(theta);
    list_add(list, add);
  }
}

// This method uses draw circles to create a body of a circle
// with mass, color, speed, etc.
// Adds it to a given scene. Also given a type to see whether it is
// an infinite mass or normal mass one.
void make_circles(scene_t *scene, vector_t center, double radius, size_t type) {
  list_t *points = list_init(CIRCLE_INCREMENT, (free_func_t) vec_free);
  draw_circle(1, center, radius, points);
  body_t *new_circle = NULL;
  // Types will allow us to add bodies to the screen that we want
  // Type 1 is normal bodies that we see. Random color and uniform mass
  // Type 2 are white (so invisible) and have infinite mass. Act as anchors
  if (type == 1) {
    rgb_color_t color = {(float) get_color(), (float) get_color(),
      (float) get_color()};
    new_circle = body_init(points, MASS, color);
  }
  else {
    rgb_color_t color = {(float) 1.0, (float) 1.0, (float) 1.0};
    new_circle = body_init(points, INFINITY, color);
  }
  scene_add_body(scene, new_circle);
}



int main(void) {
  double *GAMMA = malloc(sizeof(double));
  *GAMMA = 0.5;
  vector_t min = {.x = 0, .y = 0};
  vector_t max = {.x = WINDOW.x , .y = WINDOW.y};
  sdl_init(min, max);
  scene_t *scene = scene_init();
  // Diameter of the circles essentially.
  double space_between_centers = WINDOW.x / NUM_BODIES;
  // Radius of the circles using the average diameter
  double radius = space_between_centers / 2;
  for(int i = 0; i < NUM_BODIES; i++) {
    vector_t bottom = {radius + i * space_between_centers, 0};
    vector_t center = {radius + i * space_between_centers, WINDOW.y / 2};
    make_circles(scene, center, radius, 2);
    make_circles(scene, bottom, radius, 1);
  }

  for(size_t i = 1; i < scene_bodies(scene); i += 2) {
    body_t *body1 = scene_get_body(scene, i);
    body_t *body2 = scene_get_body(scene, i - 1);
    double *factor = malloc(sizeof(double));
    *factor = SPRING_CONSTANT / (1 + (i *(FACTOR_MOD)));
    create_spring(scene, factor, body1, body2);
    create_drag(scene, GAMMA, body1);
  }
  while(!sdl_is_done(scene)) {
    double dt = time_since_last_tick();
    // Here we want to match the infite mass bodies with the colored less
    // mass ones and create a spring force between them

    scene_tick(scene, dt);
    sdl_render_scene(scene);
 }
 scene_free(scene);
}
