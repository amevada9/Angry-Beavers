#include "forces.h"
#include "bounce_methods.h"
#include "scene.h"
#include "body.h"
#include "sdl_wrapper.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const double G_CONST = 100;
const vector_t WINDOW = {1000, 500};
const int MIN_MASS = 15;
const int MAX_MASS = 95;
const int NUM_STARS = 70;

int main(void){
  vector_t min = {.x = 0, .y = 0};
  vector_t max = {.x = WINDOW.x , .y = WINDOW.y};
  sdl_init(min, max);
  scene_t *scene = scene_init();
  // Adding bodies to the scene
  for(int i = 0; i < NUM_STARS; i++) {
    int xCenter = random_number(0, (int) WINDOW.x);
    int yCenter = random_number(0, (int) WINDOW.y);
    vector_t center = {xCenter, yCenter};
    body_t *star = body_init(create_star(4, center),
      random_number(MIN_MASS, MAX_MASS),
      (rgb_color_t) {(float) get_color(), (float) get_color(),
        (float) get_color()});
    scene_add_body(scene, star);
  }

  for(size_t i = 0; i < scene_bodies(scene); i++){
    for(size_t j = i + 1; j < scene_bodies(scene); j++){
       create_newtonian_gravity(scene, G_CONST, scene_get_body(scene, i),
         scene_get_body(scene, j));
    }
  }
  // Main loop
  while(!sdl_is_done(scene)) {
   double dt = time_since_last_tick();

    scene_tick(scene, dt);
    sdl_render_scene(scene);
  }
  scene_free(scene);
}
