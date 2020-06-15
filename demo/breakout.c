
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
#include "bounce_methods.h"

const vector_t WINDOW = {1000, 500};
const vector_t USER_INIT_CENTER = {500, 25};
const double ACCELERATION = 500;
const double PI_2 = 3.14159265358979323846;
const double BALL_RADIUS = 7.5;
const vector_t ORIG_VEL = {50, 0};
const double CIRCLE_INCREMENT = 200;
const double MASS = 10000;
const vector_t BALL_VELOCITY = {100, 100};

const int PADDLE_INFO = 0;
const int BRICK_INFO = 1;
const int BALL_INFO = 2;
const int WALL_INFO = 4;
const int KILLER_BALL_INFO = 5;

const double WALL_THICKNESS = 5;

const double BRICK_OFFSET = 10;
const double BRICK_WIDTH = 89;
const double BRICK_HEIGHT = 25;
const int BRICK_HEALTH = 3;
const double BRICK_REGEN_TIME = 65;
const double CREATE_STAR_TIME = 10;

const double PADDLE_X = 100.0;
const double PADDLE_Y = 10.0;
const vector_t PADDLE_INIT = {462.5, 11.0};
const double ELASTICITY = 1.0;
const double COLOR_INC = 0.1;

//Draws ball and adds it to the scene
void make_ball(scene_t *scene) {
  double radius = BALL_RADIUS;
  vector_t center = {(WINDOW.x / 2) - PADDLE_X / 2,
    (PADDLE_INIT.y + BALL_RADIUS + BRICK_OFFSET)};
  vector_t *cent = malloc(sizeof(vector_t));
  list_t *shape = list_init(CIRCLE_INCREMENT, (free_func_t) vec_free);
  assert(cent != NULL);
  cent->x = center.x;
  cent->y = center.y;
  double dtheta = 2 * PI_2 / CIRCLE_INCREMENT;
  double theta = 0;
  double origTheta = PI_2 / 4;
  for (int i = 0; i < CIRCLE_INCREMENT;  i++){
    theta = origTheta + (i * dtheta);
    vector_t *add = malloc(sizeof(vector_t));
    assert(add != NULL);
    add->x = center.x + radius * cos(theta);
    add->y = center.y + radius * sin(theta);
    list_add(shape, add);
  }
  int *ball_info = malloc(sizeof(int));
  *ball_info = BALL_INFO;
  list_t *info = list_init(1, NULL);
  list_add(info, (void *) ball_info);
  body_t *ball = body_init_with_info(shape, MASS,
    (rgb_color_t) {0, 0, 0}, info, NULL);
  body_set_velocity(ball, BALL_VELOCITY);
  scene_add_body(scene, ball);
}

//Returns points list of a rectangle shape
list_t *make_rectangle(vector_t *center, double x_dim, double y_dim){
  list_t *points_list = list_init(4, (free_func_t) vec_free);
  list_add(points_list, (void *) vec_init_pointer(center->x + x_dim / 2,
    center->y + y_dim / 2));
  list_add(points_list, (void *) vec_init_pointer(center->x - x_dim / 2,
    center->y + y_dim / 2));
  list_add(points_list, (void *) vec_init_pointer(center->x - x_dim / 2,
    center->y - y_dim / 2));
  list_add(points_list, (void *) vec_init_pointer(center->x + x_dim / 2,
    center->y - y_dim / 2));
  return points_list;
}

// Makes a single brick body and adds to scene.
void make_brick(list_t* shape, rgb_color_t color, scene_t *scene){
  int *brick_info = malloc(sizeof(int));
  *brick_info = BRICK_INFO;
  int *brick_health = malloc(sizeof(int));
  *brick_health = BRICK_HEALTH;
  list_t *info = list_init(2, NULL);
  list_add(info, (void *) brick_info);
  list_add(info, (void *) brick_health);
  body_t *brick = body_init_with_info(shape, INFINITY, color, info,
    (free_func_t) list_free);
  scene_add_body(scene, brick);
}

// Shifts existing bricks down.
void move_bricks_down(scene_t *scene){
  for(int i = 5; i < scene_bodies(scene); i++){
    vector_t orig_cent = body_get_centroid(scene_get_body(scene, i));
    body_set_centroid(scene_get_body(scene,i), vec_init(orig_cent.x,
      orig_cent.y - (BRICK_HEIGHT + BRICK_OFFSET)));
  }
}

// Adds a row of bricks in the top position on the screne.
void add_brick_row(scene_t *scene){
  vector_t *center = vec_init_pointer(BRICK_OFFSET + BRICK_WIDTH / 2,
    WINDOW.y - (BRICK_OFFSET + BRICK_HEIGHT / 2));
  rgb_color_t color = {1, 0, 0};
  for(int i = 0; i < 10; i++){
      // Creates color gradient
      double color_increment = i * COLOR_INC;
      color.r = 1 - color_increment;
      color.g = color_increment;
      if(i > 1){
        color.b = color_increment;
      }
    center->x = BRICK_OFFSET  + (BRICK_WIDTH / 2) +
      i * (BRICK_WIDTH + BRICK_OFFSET);
    make_brick(make_rectangle(center, BRICK_WIDTH, BRICK_HEIGHT), color, scene);
  }
}

// Uses helper funcitons defined above to make initial brick configuration.
void make_brick_config(scene_t *scene){
  add_brick_row(scene);
  move_bricks_down(scene);
  add_brick_row(scene);
  move_bricks_down(scene);
  add_brick_row(scene);
}

// Initializes position of paddle in the scene.
void make_paddle(scene_t *scene){
  int *paddle_info = malloc(sizeof(int));
  *paddle_info = PADDLE_INFO;
  list_t *info = list_init(1, NULL);
  list_add(info, (void *) paddle_info);
  rgb_color_t color = {0, 1, 0};
  vector_t *paddle_initial = vec_init_pointer(PADDLE_INIT.x, PADDLE_INIT.y);
  list_t *points = make_rectangle(paddle_initial, PADDLE_X, PADDLE_Y);
  body_t *paddle = body_init_with_info(points, INFINITY, color, info, NULL);
  scene_add_body(scene, paddle);
}

/**
 * Key handler
 * Uses two keys: left, right
 * Left: Moves the user left
 * Right: Moves the user Right
**/
void on_key(char key, event_type_t type, double held_time, void *scene){
  body_t *paddle = scene_get_body((scene_t *) scene, 1);
    if (type == KEY_PRESSED){
      // Events when key is pressed.
       switch (key){
         case LEFT_ARROW:
            body_set_velocity(paddle, vec_init(-(ORIG_VEL.x + held_time
              * ACCELERATION), ORIG_VEL.y));
            break;
         case RIGHT_ARROW:
            body_set_velocity(paddle, vec_init((ORIG_VEL.x) +
              held_time * ACCELERATION, ORIG_VEL.y));
            break;
       }
     }
     else {
       // Events when key is released.
       switch (key){
           case LEFT_ARROW:
              body_set_velocity(paddle, vec_init(-(ORIG_VEL.x), ORIG_VEL.y));
              break;
           case RIGHT_ARROW:
              body_set_velocity(paddle, vec_init(ORIG_VEL.x, ORIG_VEL.y));
              break;
     }
   }
 }

// Make three walls for ball to bounce off of.
void make_walls(scene_t *scene){
  int *wall_info = malloc(sizeof(int));
  *wall_info = WALL_INFO;
  list_t *info = list_init(1, NULL);
  list_add(info, (void *) wall_info);
  list_t *ceiling_shape = make_rectangle(vec_init_pointer(WINDOW.x / 2,
    WINDOW.y - WALL_THICKNESS / 2), WINDOW.x, WALL_THICKNESS);
  list_t *left_wall_shape = make_rectangle(vec_init_pointer(WALL_THICKNESS / 2,
     WINDOW.y / 2), WALL_THICKNESS, WINDOW.y);
  list_t *right_wall_shape = make_rectangle(vec_init_pointer(WINDOW.x -
    WALL_THICKNESS / 2, WINDOW.y / 2), WALL_THICKNESS, WINDOW.y);
  body_t *ceiling = body_init_with_info(ceiling_shape, INFINITY,
    (rgb_color_t) {0, 0, 0}, info, NULL);
  body_t *left_wall = body_init_with_info(left_wall_shape, INFINITY,
    (rgb_color_t) {0, 0, 0}, info, NULL);
  body_t *right_wall = body_init_with_info(right_wall_shape, INFINITY,
    (rgb_color_t) {0, 0, 0}, info, NULL);
  scene_add_body(scene, ceiling);
  scene_add_body(scene, left_wall);
  scene_add_body(scene, right_wall);
}

// Checks if there is a collision and acts according to the identity
// of the objects collided.
void collide(scene_t *scene){
  body_t *ball = scene_get_body(scene, 0);
  // Checking all other bodies to see if there is a collision.
  for(size_t i = 1; i < scene_bodies(scene); i++){
    body_t *collision_body = scene_get_body(scene, i);
    collision_info_t collided = find_collision(ball, collision_body);
    if (get_if_collided(collided) &&
      *(int *)body_get_info(collision_body) != KILLER_BALL_INFO){
      // Ball bounces off whatever body it hits
      create_physics_collision(scene, ELASTICITY, ball, collision_body);
      // If the ball hits a brick, remove the brick
      if(*(int *) list_get((list_t *) body_get_info(collision_body), 0)
        == BRICK_INFO){
        int *health = (int *) list_remove((list_t*)
          body_get_info(collision_body), 1);
        // Removes the current health and adds the new health to the end of
        // list.
        *health = *health - 1;
        list_add((list_t*) body_get_info(collision_body), health);
        if (*(int *) list_get((list_t *)
          body_get_info(collision_body), 1) <= 0){
          scene_remove_body(scene, i);
        }
      }
    }
  }
}

// Actions if a killer stars collide with the paddle.
bool killer_collide(scene_t *scene){
  body_t *paddle = scene_get_body(scene, 1);
  bool paddle_status = body_is_collided(paddle);
  for(size_t i = 2; i < scene_bodies(scene); i++){
    body_t *body = scene_get_body(scene, i);
    if((*(int *) body_get_info(body)) == KILLER_BALL_INFO){
      collision_info_t collided = find_collision(paddle, body);
      bool is_collided = get_if_collided(collided);
      if (is_collided) {
        return true;
      }
    }
  }
  body_collided(paddle, paddle_status);
  return false;
}

// Checks if the ball is fof screen.
bool ball_is_off_screen(scene_t *scene){
  body_t *ball = scene_get_body(scene, 0);
  vector_t ball_center = body_get_centroid(ball);
  if(ball_center.y < (PADDLE_Y) || ball_center.x < 0 || ball_center.x
    > WINDOW.x){
    return true;
  }
  return false;
}

// Used just to remove the killer star, if it moves off screen (i.e. misses the
// paddle).
void remove_off_screen(scene_t *scene){
  size_t size = scene_bodies(scene);
  for(size_t i = 1; i < size; i++){
    body_t *body = scene_get_body(scene, i);
    vector_t center = body_get_centroid(body);
    double x = center.x;
    double y = center.y;
    if ((x > WINDOW.x || x < 0 || y > WINDOW.y || y < 0) &&
        *(int *)body_get_info(body) == KILLER_BALL_INFO) {
      body_remove(body);
    }
  }
}

// Makes a killer star object and adds it to scene.
void make_killer_star(scene_t *scene) {
  double rand_x = random_number(0, WINDOW.x);
  double rand_y = random_number(WINDOW.y / 2, WINDOW.y);
  list_t *shape = create_star(10, vec_init(rand_x, rand_y));
  rgb_color_t color = {0, 1, 0};
  int *info = malloc(sizeof(int));
  *info = KILLER_BALL_INFO;
  body_t *star = body_init_with_info(shape, 1, color, info, (free_func_t) free);
  body_set_velocity(star, vec_init(0, -50));
  scene_add_body(scene, star);
}

// Verifies if the game is over if the ball is off the screen or all the bricks
// are gone (only 5 bodies on the scene (ball, paddle, walls)).
bool game_over(scene_t *scene){
  if(ball_is_off_screen(scene) || scene_bodies(scene) == 5){
    return true;
  }
  return false;
}

// Main function with while loop that executes scene tick.
int main(void) {
  sdl_init(VEC_ZERO, WINDOW);
  scene_t *scene = scene_init();
  make_ball(scene);
  make_paddle(scene); // ensuring it is position 1 for key handler
  make_walls(scene);
  make_brick_config(scene);
  double clock = 0;
  double killer = 0;

  sdl_on_key(on_key);
  while(!sdl_is_done(scene)) {
    double dt = time_since_last_tick();
    clock += dt;
    killer += dt;
    scene_tick(scene, dt);
    if (clock > BRICK_REGEN_TIME){
      move_bricks_down(scene);
      add_brick_row(scene);
      clock = 0;
    }
    if(killer > CREATE_STAR_TIME){
      make_killer_star(scene);
      killer = 0;
    }
    collide(scene);
    remove_off_screen(scene);
    sdl_render_scene(scene);
    bool is_killed = killer_collide(scene);
    if(game_over(scene) || is_killed){
      break;
    }
  }
  scene_free(scene);
}
