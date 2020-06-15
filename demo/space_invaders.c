#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "scene.h"
#include "forces.h"
#include "list.h"
#include "bounce_methods.h"
#include "sdl_wrapper.h"
#include "collision.h"

const double ENEMY_CIRCLE_AMOUNT = 0.400;
const double ENEMY_RADIUS = 30;
const vector_t ENEMY_VELOCITY = {50, 0};
const vector_t WINDOW = {1000, 500};
const vector_t USER_INIT_CENTER = {500, 25};
const double ACCELERATION = 100;
const double PI_2 = 3.14159265358979323846;
const double FULL_CIRCLE_FRAC = 1;
const double BULLET_RADIUS = 7.5;
const vector_t ORIG_VEL = {50, 0};
const double CIRCLE_INCREMENT = 60;
const double MASS = 5;
const vector_t USER_START = {500, ENEMY_RADIUS};
const vector_t DEFAULT_SHIFT_DOWN = {0, -50};
const double INIT_ENEMY_HEIGHT = 400;
const int WAIT_SHOOT = 2;
const int WAIT_MAKE_ENEMIES = 7;
const vector_t VELOCITY_VECTOR = {0, 100};

const int USER_INFO = 0;
const int ENEMY_INFO = 1;
const int USER_BULLET_INFO = 2;
const int ENEMY_BULLET_INFO = 3;

const double ENEMY_START_X = 75;
const double PAC_FRACTION = 5.0/6.0;


const double BULLET_MASS = 2;


/**
the following are numbers that are passed in to body_init_with_info as the
void *.  They help to identify what is what
0 is the user
1 is the enemy
2 is the user bul
3 is the enemy bullet
*/


void draw_circle(double circ_frac, vector_t center, double radius,
  list_t *list) {
  vector_t *cent = malloc(sizeof(vector_t));
  assert(cent != NULL);
  cent->x = center.x;
  cent->y = center.y;
  list_add(list, cent);
  double dtheta = 2 * PI_2 / CIRCLE_INCREMENT;
  double theta = 0;
  double origTheta = circ_frac * PI_2 / 4;
  for (int i = 0; i < CIRCLE_INCREMENT * circ_frac;  i++){
    theta = origTheta + (i * dtheta);
    vector_t *add = malloc(sizeof(vector_t));
    assert(add != NULL);
    add->x = center.x + radius * cos(theta);
    add->y = center.y + radius * sin(theta);
    list_add(list, add);
  }
}

void shoot(scene_t *scene, body_t *shooter, int type) {
  int *bullet_info = malloc(sizeof(int));
  // Will creates a cirle that starts at the center of the shooter
  // has an initial y velocity. Once it leaves the screen it can be removed.
  // Will use find_collision to see if it hits the ship but this method should
  list_t *circle_bullet = list_init(CIRCLE_INCREMENT, (free_func_t) vec_free);
  vector_t center = *(vector_t *)list_get(body_get_shape(shooter), 0);
  draw_circle(FULL_CIRCLE_FRAC, center, BULLET_RADIUS, circle_bullet);
  if (type == USER_INFO) {
    *bullet_info = USER_BULLET_INFO;
    body_t *bullet = body_init_with_info(circle_bullet, BULLET_MASS, (rgb_color_t){1, 0, 0}
    , bullet_info, NULL);
    body_set_velocity(bullet, VELOCITY_VECTOR);
    scene_add_body(scene, bullet);
  }
  else {
    *bullet_info = ENEMY_BULLET_INFO;
    body_t *bullet = body_init_with_info(circle_bullet, BULLET_MASS, (rgb_color_t){0, 1, 0}
    ,bullet_info, NULL);
    body_set_velocity(bullet, vec_negate(VELOCITY_VECTOR));
    scene_add_body(scene, bullet);
  }
}



// Will create a row of enemies and put them in a starting position
void make_row_enemies(scene_t *scene, int type, double row) {
  int *enemy_info = malloc(sizeof(int));
  *enemy_info = 1;
  vector_t start = vec_init(0, row);
  double factor = 0;
  if (type == 0) {
    start.x = ENEMY_START_X;
    factor = 1;
  }
  else {
    start.x = WINDOW.x - ENEMY_START_X;
    factor = -1;
  }
  for (int i = 0; i < 4; i++) {
    list_t *points = list_init(CIRCLE_INCREMENT, (free_func_t) vec_free);
    draw_circle(ENEMY_CIRCLE_AMOUNT, start, ENEMY_RADIUS, points);
    rgb_color_t color = {0.0, 0.0, 0.0};
    body_t *new_enemy = body_init_with_info(points, MASS, color, enemy_info,
      NULL);
    body_set_velocity(new_enemy, vec_multiply(factor, ENEMY_VELOCITY));
    scene_add_body(scene, new_enemy);
    start.x += (factor * ENEMY_START_X);
  }
}

void make_triangle(vector_t center, double length, list_t *list) {
  vector_t *point = malloc(sizeof(vector_t));
  *point = vec_add(center, (vector_t){0, length / 2});
  vector_t *left = malloc(sizeof(vector_t));
  *left = vec_add(center, (vector_t){-length, 0});
  vector_t *right = malloc(sizeof(vector_t));
  *right = vec_add(center, (vector_t){length, 0});
  list_add(list, point);
  list_add(list, left);
  list_add(list, right);
}

// Makes the user space_ship
// In the demo it is an oval, but here we made it to be like a pacman
// and facing the other way
body_t *make_user_space_ship(scene_t *scene) {
  int *user_info = malloc(sizeof(int));
  *user_info = 0;
  list_t *points = list_init(CIRCLE_INCREMENT, (free_func_t) vec_free);
  make_triangle(USER_START, ENEMY_RADIUS * 2, points);
  rgb_color_t color = {(float) 0, (float) 1, (float) 1};
  body_t *new_ship = body_init_with_info(points, MASS, color, user_info,
  NULL);
  body_set_velocity(new_ship, VEC_ZERO);
  return new_ship;
}

//moves down enemies if they hit the wall and reverses their velocity
void check_shift_down(scene_t *scene) {
  for (int i = 0; i < scene_bodies(scene); i++) {
    body_t *enemy = scene_get_body(scene, i);
    if (*(int *)body_get_info(enemy) == 1) {
        list_t *points_shape = body_get_shape(enemy);
        vector_t *center_point = list_get(points_shape, 0);
        if (center_point->x <= ENEMY_RADIUS ||
          center_point->x >= WINDOW.x - ENEMY_RADIUS) {
          polygon_translate(points_shape, DEFAULT_SHIFT_DOWN);
          body_set_points(enemy, points_shape);
          vector_t new_velo = vec_negate(body_get_velocity(enemy));
          body_set_velocity(enemy, new_velo);
      }
    }
  }
}

/**
 * Key handler
 * Uses three keys, left, right, and shoot.
 * Left: Moves the user left unless
 * Right: Moves the user ship Right
 * Space: Shoots
**/
void on_key(char key, event_type_t type, double held_time, void *scene){
  body_t *space_ship = scene_get_body((scene_t *) scene, 0);
    if (type == KEY_PRESSED){
      // Events when key is pressed.
       switch (key){
         case LEFT_ARROW:
            body_set_velocity(space_ship, vec_init(-(ORIG_VEL.x + held_time
              * ACCELERATION), ORIG_VEL.y));
            break;
         case RIGHT_ARROW:
            body_set_velocity(space_ship, vec_init(ORIG_VEL.x +
              held_time * ACCELERATION, ORIG_VEL.y));
            break;
         case ' ':
           shoot((scene_t *)scene, space_ship, 0);
           break;
       }
     }
     else {
       // Events when key is released.
       switch (key){
           case LEFT_ARROW:
              body_set_velocity(space_ship, vec_init(-(ORIG_VEL.x), ORIG_VEL.y));
              break;
           case RIGHT_ARROW:
              body_set_velocity(space_ship, vec_init(ORIG_VEL.x, ORIG_VEL.y));
              break;
           case ' ':
              break;
     }
   }
 }

//will find a random enemy and make it shoot a bullet down
void enemy_shoot(scene_t *scene) {
  size_t size = scene_bodies(scene);
  int idx = random_number(0, size - 1);
  int type = *(int *)body_get_info(scene_get_body(scene, idx));
  if (type == 1) {
    shoot(scene, scene_get_body(scene, idx), type);
  }
  else {
    enemy_shoot(scene);
  }
}

//will return true if the collision between two bodies is valid
//example: a collision between an enemy and an enemy bullet returns false
bool is_valid_collision(body_t *body1, body_t *body2) {
  int type_body1 = *(int *) body_get_info(body1);
  int type_body2 = *(int *) body_get_info(body2);

  if (type_body1 == USER_INFO) {
    return type_body2 == ENEMY_INFO || type_body2 == ENEMY_BULLET_INFO;
  }
  else if (type_body1 == ENEMY_INFO) {
    return type_body2 == USER_INFO || type_body2 == USER_BULLET_INFO;
  }
  else if (type_body1 == USER_BULLET_INFO) {
    return type_body2 == ENEMY_INFO || type_body2 == ENEMY_BULLET_INFO;
  }
  else if (type_body1 == ENEMY_BULLET_INFO) {
    return type_body2 == USER_INFO || type_body2 == USER_BULLET_INFO;
  }
  return false;
}

//will call create_destructive_collision if two bodies collide and are valid
//loops through every body and compares it to every other body
void collide(scene_t *scene){
  size_t size = scene_bodies(scene);
  for(size_t i = 0; i < size; i++){
    for(size_t j = i + 1; j < size; j++){
      body_t *body1 = scene_get_body(scene, i);
      body_t *body2 = scene_get_body(scene, j);
      collision_info_t collided = find_collision(body1, body2);
      if (get_if_collided(collided)){
        if(is_valid_collision(body1, body2)){
          create_destructive_collision(scene, body1, body2);
        }
      }
    }
  }
}

//if a body is off the screen, we want to remove it
void remove_off_screen(scene_t *scene){
  size_t size = scene_bodies(scene);
  //1 because we don't want to remove the user
  for(size_t i = 1; i < size; i++){
    body_t *body = scene_get_body(scene, i);
    vector_t center = body_get_centroid(body);
    double x = center.x;
    double y = center.y;
    if (x > WINDOW.x || x < 0 || y > WINDOW.y || y < 0){
      //remove the body from the scene
      scene_remove_body(scene, i);
    }
  }
}

//will return true if the game is over, false if it isn't.  It does
//this by checking if the user is still present
bool game_over(scene_t *scene){
  body_t *first_body = scene_get_body(scene, 0);
  int type_body = *(int *)body_get_info(first_body);
  if(type_body != 0){
    return true;
  }
  return false;
}

int main(void) {
  vector_t min = {.x = 0, .y = 0};
  vector_t max = {.x = WINDOW.x , .y = WINDOW.y};
  sdl_init(min, max);
  scene_t *scene = scene_init();
  int num_rows = 0;
  double clock = 0;
  double shoot = 0;
  body_t *user_space_ship = make_user_space_ship(scene);
  scene_add_body(scene, user_space_ship);

  for (int i = 0; i < 4; i++) {
    make_row_enemies(scene, i % 2, INIT_ENEMY_HEIGHT - (i * 50));
  }

  sdl_on_key(on_key);
  while(!sdl_is_done(scene)) {
    double dt = time_since_last_tick();
    check_shift_down(scene);
    clock += dt;
    shoot += dt;
    if(shoot > WAIT_SHOOT){
      enemy_shoot(scene);
      shoot = 0;
    }
    if (clock > WAIT_MAKE_ENEMIES) {
      make_row_enemies(scene, num_rows % 2, INIT_ENEMY_HEIGHT);
      clock = 0;
    }
    collide(scene);
    scene_tick(scene, dt);
    sdl_render_scene(scene);
    if(game_over(scene)){
      break;
    }
    remove_off_screen(scene);
  }
  scene_free(scene);
}
