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
#include "sdl_wrapper.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

const double BEAVER_MASS = 10;
const double BLOCK_MASS = 600;
const double ROCK_MASS = 100;
const double WOOD_MASS = 30;
const double VIRUS_MASS = 100;
const double WALL_MASS = 1e18;
const double K = 1000;
const double G_CONST = 5e-8;
const double WALL_THICKNESS = 20.0;
const double DRAG = 1.0;
const double CORONA_DRAG = 10.0;
const double GAME_OVER_HOLD = 5.0;
const double MESSAGE_SCREEN_TIME = 0.3;
const double INTRO_SCREEN_TIME = 0.3;
const double ELASTICITY = 0.7;
const double VIRUS_HEALTH = 8;
const double SMALL_ROTATE_ANGLE = 0.05;
const double BLOCK_THICKNESS = 20.0;
const double BLOCK_LENGTH = 200.0;
const double CORONA_RADIUS = 65.0;
const double BOOST_VELO = 50;
const double INTRO_SQUARE_SIZE = 150;
const double SPEED_FACTOR = 4;
const double BLOCK_SIZE = 200;
const int FONT_SIZE = 128;
const int BEAVER_TYPE = 1;
const int SLINGSHOT_TYPE = 2;
const int BAND_TYPE = 3;
const int BLOCK_TYPE = 4;
const int VIRUS_TYPE = 5;
const int WALL_TYPE = 6;
const int BACKGROUND_TYPE = 12;
const int FANCY_BEAVER_TYPE = 8;
const int MAX_BEAVERS = 5;
const int MAX_STRETCH = 100;
const int DEFAULT_STRING = 50;
const int LOADING_SCREEN = 0;
const int MESSAGE_SCREEN = 1;
const int LEVEL_ONE = 2;
const int LEVEL_TWO = 3;
const int LEVEL_THREE = 4;
const int GAME_OVER_SCREEN = 5;
const int WINNER_SCREEN = 6;
const int STRUCT_INFO = 7;
const int YOU_LOST = 0;
const int GAME_STILL_ON = 1;
const int YOU_WON = 2;
const int LEVEL_SCORE_INCREMENT = 100;
const int SMALL_SCORE_INCREMENT = 10;
const int TOTAL_BEAVERS = 5;
const int SCORE_ADD_BEAVER = 4;
const int SCORE_ADD_LAUNCH = 8;
const int SCORE_ADD_OFF_SCREEN = 9;
const int SCORE_ADD_HEALTH = 11;
const rgb_color_t ORANGE = {0.94, 0.43, 0.14, 1};
const rgb_color_t BLACK = {0, 0, 0, 1};
const rgb_color_t CLEAR = {0, 0, 0, 0};
const rgb_color_t SLINGSHOT_COLOR = {0.63, 0.32, 0.18, 1};
const rgb_color_t BLOCK_COLOR = {0.5, 0.7, 0.9, 1};
const rgb_color_t WOOD_COLOR = {0.6, 0.3, 0, 1};
const rgb_color_t ROCK_COLOR = {0.3, 0.3, 0.3, 1};
const rgb_color_t RUBBER_BAND_COLOR = {1, 0, 1, 1};
const vector_t TIP = {146, 127}; // This is where the center of the rubber
// band should be in a relaxed state
const vector_t WINDOW = {1280, 720};
const vector_t GAME_OVER_POSITION = {440, 435};
const vector_t GAME_OVER_SIZE = {400, 150};
const vector_t TITLE_POSITION = {265, 700};
const vector_t TITLE_SIZE = {750, 200};
const vector_t WINNER_POSITION = {290, 420};
const vector_t WINNER_SIZE = {700, 150};
const vector_t LEVEL_POSITION = {10, 710};
const vector_t LEVEL_SIZE = {200, 50};
const vector_t SCORE_POSITION = {10, 650};
const vector_t SCORE_SIZE = {160, 40};
const vector_t LOADING_POSITION = {380, 275};
const vector_t LOADING_SIZE = {500, 100};
const vector_t MESSAGE_POSITION = {150, 540};
const vector_t MESSAGE_SIZE = {1000, 150};
const vector_t BEAV_LEFT_POSITION = {1000, 710};
const vector_t BEAV_LEFT_SIZE = {200, 50};
const vector_t HINT_LOCATION = {225, 150};
const vector_t HINT_SIZE = {850, 120};
const vector_t WINDOW_CENTER = {640, 360};
const vector_t LEFT_BEAVER_POS = {165, 500};
const vector_t CENTER_RONA_POS = {640, 400};
const vector_t RIGHT_BEAVER_POS = {1120, 500};

// Global variables that we need for multiple methods

// Last place that the mouse was clicked
vector_t LAST_CLICK = {0, 0};
// Beavers that have been shot in the scene
int beavers_shot = 0;
//
int beavers_index = 0;
int score = 0;
bool used_boost = false;

// Make a rectangle shape.
list_t *make_rectangle(vector_t *center, double x_dim, double y_dim){
  list_t *points_list = list_init(4, NULL);
  list_add(points_list, (void *) vec_init_pointer(center->x - x_dim / 2,
    center->y - y_dim / 2));
  list_add(points_list, (void *) vec_init_pointer(center->x + x_dim / 2,
    center->y - y_dim / 2));
  list_add(points_list, (void *) vec_init_pointer(center->x + x_dim / 2,
    center->y + y_dim / 2));
  list_add(points_list, (void *) vec_init_pointer(center->x - x_dim / 2,
    center->y + y_dim / 2));
  return points_list;
}

// Getting info for what type a body is.
int get_body_type(body_t *body) {
  list_t *info = body_get_info(body);
  int type = *(int *)list_get(info, 0);
  return type;
}

// Getting health feature on virus.
double get_body_health(body_t *body) {
  if (get_body_type(body) == VIRUS_TYPE) {
    list_t *info = body_get_info(body);
    double health = *(double *)list_get(info, 1);
    return health;
  }
  return -1;
}

// Adding side wall structures.
void make_walls(scene_t *scene){
  int *wall_info = malloc(sizeof(int));
  *wall_info = WALL_TYPE;
  list_t *info = list_init(1, NULL);
  list_add(info, (void *) wall_info);

  list_t *floor_shape1 = make_rectangle(vec_init_pointer(WINDOW.x / 4,
    -8000), WINDOW.x / 2, 16020);

  body_t *floor1 = body_init_with_info(floor_shape1, WALL_MASS,
    CLEAR, info, NULL);

  scene_add_body(scene, floor1);

  list_t *floor_shape2 = make_rectangle(vec_init_pointer( 3 * WINDOW.x / 4,
    -8000), WINDOW.x / 2, 16020);

  body_t *floor2 = body_init_with_info(floor_shape2, WALL_MASS,
    CLEAR, info, NULL);

  scene_add_body(scene, floor2);

  list_t *right = make_rectangle(vec_init_pointer(WINDOW.x, WINDOW.y / 2), 20,
  WINDOW.y);
  body_t *right_wall = body_init_with_info(right, WALL_MASS,
    CLEAR, info, NULL);

  scene_add_body(scene, right_wall);

  list_t *left = make_rectangle(vec_init_pointer(0, WINDOW.y / 2),
  20, WINDOW.y);
  body_t *left_wall = body_init_with_info(left, WALL_MASS,
    CLEAR, info, NULL);

  scene_add_body(scene, left_wall);
}

// Draws a slingshot and adds it to the body
void make_slingshot(scene_t *scene){
  list_t *listOfPoints = list_init(13, NULL);
  list_add(listOfPoints, vec_init_pointer(143, 0));
  list_add(listOfPoints, vec_init_pointer(157, 0));
  list_add(listOfPoints, vec_init_pointer(157, 50));
  list_add(listOfPoints, vec_init_pointer(182, 100));
  list_add(listOfPoints, vec_init_pointer(182, 150));

  list_add(listOfPoints, vec_init_pointer(173, 150));
  list_add(listOfPoints, vec_init_pointer(173, 100));
  list_add(listOfPoints, vec_init_pointer(146, 52));

  list_add(listOfPoints, vec_init_pointer(127, 93));
  list_add(listOfPoints, vec_init_pointer(127, 138));

  list_add(listOfPoints, vec_init_pointer(120, 138));
  list_add(listOfPoints, vec_init_pointer(120, 93));
  list_add(listOfPoints, vec_init_pointer(143, 47));
  list_t *body_info = list_init(1, NULL);
  int *body_type = malloc(sizeof(int));
  *body_type = SLINGSHOT_TYPE;
  list_add(body_info, body_type);
  body_t *slingshot = body_init_with_info(listOfPoints, INFINITY,
    SLINGSHOT_COLOR,
    body_info, NULL);
  scene_add_body(scene, slingshot);
}

// This takes in the stretch and computes the initial velocity of the beaver.
vector_t compute_launch_speed(vector_t *stretch){
  double displacement = vec_magnitude(*stretch);
  double velocity = sqrt((K * displacement * displacement) /
  (BEAVER_MASS * SPEED_FACTOR));
  vector_t unit = vec_unit(*stretch);
  return(vec_multiply(-velocity, unit));
}

// Sends a beaver flying
void launch_beaver(scene_t *scene, body_t *beaver, vector_t *stretch){
  vector_t velocity = compute_launch_speed(stretch);
  body_set_velocity(beaver, velocity);
  score += SCORE_ADD_LAUNCH;
}

// Draws a rubber band, point is the stretched tip of the rubber band
void draw_rubberband(scene_t *scene, vector_t *point){
  //remove the old rubber band
  for (int i = 0; i < scene_bodies(scene); i++) {
    body_t *enemy = scene_get_body(scene, i);
    //list_t *body_info = body_get_info(enemy);
    if (get_body_type(enemy) == BAND_TYPE){
      scene_remove_body(scene, i);
    }
  }

  // Create the new rubber band with the desired center
  list_t *listOfPoints = list_init(6, NULL);
  list_add(listOfPoints, vec_init_pointer(127, 117));
  list_add(listOfPoints, vec_init_pointer(127, 130));
  list_add(listOfPoints, point);
  list_add(listOfPoints, vec_init_pointer(173, 125));
  list_add(listOfPoints, vec_init_pointer(173, 140));
  list_add(listOfPoints, point);
  list_t *body_info = list_init(1, NULL);
  int *body_type = malloc(sizeof(int));
  *body_type = BAND_TYPE;
  list_add(body_info, (void *) body_type);
  body_t *rubber_band = body_init_with_info(listOfPoints, INFINITY,
    RUBBER_BAND_COLOR, body_info, NULL);
  scene_add_body(scene, rubber_band);
}

// Finds which beaver is next on the scene
body_t *find_beaver(scene_t *scene){
  for(size_t i = 0; i < scene_bodies(scene); i++){
    body_t *body = scene_get_body(scene, i);
    int body_type = get_body_type(body);
    if(body_type == BEAVER_TYPE || body_type == FANCY_BEAVER_TYPE){
      if(body_get_launched(scene_get_body(scene, i)) == false){
        return (scene_get_body(scene, i));
      }
    }
  }
  return NULL;
}

// Making a beaver on the screen.
void make_beaver(scene_t *scene, int index) {
  list_t *listOfPoints = list_init(4, NULL);
  list_add(listOfPoints, vec_init_pointer(0, 0));
  list_add(listOfPoints, vec_init_pointer(CORONA_RADIUS, 0));
  list_add(listOfPoints, vec_init_pointer(CORONA_RADIUS, CORONA_RADIUS));
  list_add(listOfPoints, vec_init_pointer(0, CORONA_RADIUS));
  list_t *body_info = list_init(1, NULL);
  int *body_type = malloc(sizeof(int));
  if (index == 3) {
    *body_type = FANCY_BEAVER_TYPE;
  }
  else {
    *body_type = BEAVER_TYPE;
  }
  list_add(body_info, (void *) body_type);
  body_t *launcher = body_init_with_info(listOfPoints, BEAVER_MASS, CLEAR,
    body_info, NULL);
  body_set_centroid(launcher, TIP);
  body_set_launched(launcher, false);
  body_set_rotation(launcher, SMALL_ROTATE_ANGLE);
  scene_add_body(scene, launcher);
  score += SCORE_ADD_BEAVER;
}

// Making a virus body on the scene.
void make_corona(scene_t *scene, vector_t center, double size){
  list_t *listOfPoints = list_init(4, NULL);
  list_add(listOfPoints, vec_init_pointer(0, 0));
  list_add(listOfPoints, vec_init_pointer(size, 0));
  list_add(listOfPoints, vec_init_pointer(size, size));
  list_add(listOfPoints, vec_init_pointer(0, size));
  list_t *body_info = list_init(2, NULL);
  int *body_type = malloc(sizeof(int));
  *body_type = VIRUS_TYPE;
  double *body_health = malloc(sizeof(double));
  *body_health= VIRUS_HEALTH;
  list_add(body_info, (void *) body_type);
  list_add(body_info, (void *) body_health);
  //virus will also need a health info
  body_t *rona = body_init_with_info(listOfPoints, VIRUS_MASS, CLEAR,
     body_info, NULL);
  body_set_centroid(rona, center);
  //wack reason for this.  If you got questions, ask will
  body_set_rotation(rona, SMALL_ROTATE_ANGLE);
  scene_add_body(scene, rona);
  create_drag(scene, CORONA_DRAG, rona);
}

// Making structure block on screen.
void make_block(scene_t *scene, vector_t *center, double size_x, double size_y,
   double mass, rgb_color_t color, double ang_vel){
  list_t *rec = make_rectangle(center, size_x, size_y);
  list_t *body_info = list_init(1, NULL);
  int *body_type = malloc(sizeof(int));
  *body_type = BLOCK_TYPE;
  list_add(body_info, (void *) body_type);
  body_t *block = body_init_with_info(rec, mass, color, body_info, NULL);
  body_set_angular_impulse(block, ang_vel);
  scene_add_body(scene, block);
  create_drag(scene, DRAG, block);
}

// Specific structure block for rock.
void make_rock(scene_t *scene, vector_t *center, double size_x, double size_y,
  double ang_vel){
  make_block(scene, center, size_x, size_y, ROCK_MASS, ROCK_COLOR,ang_vel);
}

// Specific structure block for wood.
void make_wood(scene_t *scene, vector_t *center, double size_x, double size_y,
  double ang_vel){
  make_block(scene, center, size_x, size_y, WOOD_MASS, WOOD_COLOR, ang_vel);
}

// Adding background iamge.
void make_background_image(scene_t *scene) {
  list_t *background = make_rectangle(vec_init_pointer(WINDOW_CENTER.x,
    WINDOW_CENTER.y), WINDOW.x, WINDOW.y);
  list_t *body_info = list_init(1, NULL);
  int *background_type = malloc(sizeof(int));
  *background_type = BACKGROUND_TYPE;
  list_add(body_info, background_type);
  body_t *background_body = body_init_with_info(background, INFINITY, CLEAR,
      body_info, NULL);
  scene_add_body(scene, background_body);
}

// Building level one structure.
void make_level_one(scene_t *scene){
  make_background_image(scene);
  make_slingshot(scene);
  make_walls(scene);
  vector_t *original_tip = vec_init_pointer(TIP.x, TIP.y);
  draw_rubberband(scene, original_tip);
  make_corona(scene, vec_init(900, 250), 70);
  make_wood(scene, vec_init_pointer(800, 250), BLOCK_THICKNESS,
    BLOCK_SIZE, 0.0);
  make_rock(scene, vec_init_pointer(1000, 250), BLOCK_THICKNESS,
    BLOCK_SIZE, 0.0);
  beavers_index = 0;
  used_boost = false;
}

// Building level two structure.
void make_level_two(scene_t *scene){
  make_background_image(scene);
  make_slingshot(scene);
  make_walls(scene);
  vector_t *original_tip = vec_init_pointer(TIP.x, TIP.y);
  draw_rubberband(scene, original_tip);
  make_corona(scene, vec_init(900, 425), 70.0);
  make_corona(scene, vec_init(900, 250), 70.0);
  make_rock(scene, vec_init_pointer(700, 350), BLOCK_THICKNESS, BLOCK_LENGTH,
    0.0);
  make_rock(scene, vec_init_pointer(1100, 350), BLOCK_THICKNESS, BLOCK_LENGTH,
    0.0);
  make_wood(scene, vec_init_pointer(900, 362.5), BLOCK_LENGTH, BLOCK_THICKNESS,
    0.0);
  beavers_index = 0;
  used_boost = false;
}

// Building level three structure.
void make_level_three(scene_t *scene){
  make_background_image(scene);
  make_slingshot(scene);
  make_walls(scene);
  vector_t *original_tip = vec_init_pointer(TIP.x, TIP.y);
  draw_rubberband(scene, original_tip);
  make_corona(scene, vec_init(800, 300), 70.0);
  make_corona(scene, vec_init(1000, 350), 70.0);
  make_block(scene, vec_init_pointer(600, 500), BLOCK_THICKNESS, BLOCK_LENGTH,
    INFINITY, ROCK_COLOR, 5.0);
  make_block(scene, vec_init_pointer(600, 200), BLOCK_THICKNESS, BLOCK_LENGTH,
    INFINITY, ROCK_COLOR, -5.0);
  beavers_index = 0;
  used_boost = false;
}
//0 is you failed, 1 is the game is still on, 2 is you won
int is_game_over(scene_t *scene) {
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *body = scene_get_body(scene, i);
    if (get_body_type(body) == VIRUS_TYPE) {
      if(beavers_shot >= MAX_BEAVERS) {
        return YOU_LOST;
      }
      return GAME_STILL_ON;
    }
  }
  return YOU_WON;
}

// Removing bodies that are off the screen.
void remove_off_screen(scene_t *scene){
  size_t size = scene_bodies(scene);
  for(size_t i = 0; i < size; i++){
    body_t *body = scene_get_body(scene, i);
    vector_t center = body_get_centroid(body);
    int body_type = get_body_type(body);
    double x = center.x;
    double y = center.y;
    if (x > WINDOW.x || x < 0 || y > WINDOW.y || y < 0){
      if(body_type == BEAVER_TYPE || body_type == VIRUS_TYPE || body_type ==
          BLOCK_TYPE){
        score += SCORE_ADD_OFF_SCREEN;
        body_remove(body);
      }
    }
  }
}

// Checking if a body is spinning to stop if beaver or virus
void check_spinning(scene_t *scene){
  for(size_t i = 0; i < scene_bodies(scene); i++){
    body_t *body = scene_get_body(scene, i);
    int type_body = get_body_type(body);
    if(type_body == BEAVER_TYPE || type_body == FANCY_BEAVER_TYPE || type_body
        == VIRUS_TYPE){
      double ang_vel = body_get_angular_velocity(body);
      if(ang_vel != 0.0) {
        body_set_angular_velocity(body, 0.0);
      }
    }
  }
}

// Critera to see if physics collision is one we want.
bool is_valid_physics_collision(body_t *body1, body_t *body2) {
  int type_body1 = get_body_type(body1);
  int type_body2 = get_body_type(body2);

  if (type_body1 == BEAVER_TYPE || type_body1 == FANCY_BEAVER_TYPE) {
    return(type_body2 == BLOCK_TYPE);
  }
  if (type_body1 == VIRUS_TYPE) {
    return(type_body2 == BLOCK_TYPE);
  }
  if (type_body1 == BLOCK_TYPE) {
    return(type_body2 == VIRUS_TYPE ||
       type_body2 == BEAVER_TYPE || type_body2 == BLOCK_TYPE ||
       type_body2 == FANCY_BEAVER_TYPE);
  }
  return false;
}

// Create physics collisions between everything.
void physics_collide(scene_t *scene){
  size_t size = scene_bodies(scene);
  for(size_t i = 3; i < size; i++){
    for(size_t j = i + 1; j < size; j++){
      body_t *body1 = scene_get_body(scene, i);
      body_t *body2 = scene_get_body(scene, j);
      if(is_valid_physics_collision(body1, body2)) {
        create_physics_collision(scene, ELASTICITY, body1, body2);
      }
    }
  }
}

// Adjusts health on virus.
void health(scene_t *scene){
  // Checking all other bodies to see if there is a collision.
  for(size_t i = 2; i < scene_bodies(scene); i++){
    body_t *body1 = scene_get_body(scene, i);
    int body1_type = get_body_type(body1);
    for(size_t j = 2; j < scene_bodies(scene); j++){
      if(i != j){
        body_t *body2 = scene_get_body(scene, j);
        int body2_type = get_body_type(body2);
        collision_info_t collided = find_collision(body1, body2);
        if(get_if_collided(collided)) {
          if(body2_type == VIRUS_TYPE){
            if(body1_type == BEAVER_TYPE || body1_type == FANCY_BEAVER_TYPE){
              scene_remove_body(scene, j);
            }
            else if (body1_type == BLOCK_TYPE){
              double velo_mag = vec_magnitude(body_get_velocity(body1)) +
                                vec_magnitude(body_get_velocity(body2));
              double health_dec = velo_mag * body_get_mass(body1) / 12000;
              double *health = (double *) list_remove((list_t *)
                body_get_info(body2), 1);
              *health = *health - health_dec;
              score += SCORE_ADD_HEALTH;
              list_add((list_t*) body_get_info(body2), health);
              if (*(double *) list_get((list_t *) body_get_info(body2), 1)
                <= 0){
                scene_remove_body(scene, j);
              }
            }
          }
        }
      }
    }
  }
}

// Clicking event handler.
void on_click(event_type_t type, vector_t clicked_point, void *scene) {
  int status = is_game_over(scene);
  if(status == YOU_LOST || status == YOU_WON){
    return;
  }
  body_t *beaver = find_beaver(scene);
  vector_t *change_vector = malloc(sizeof(vector_t));
  vector_t *new_tip;
  clicked_point.y = WINDOW.y - clicked_point.y;

  *change_vector = vec_subtract(clicked_point, LAST_CLICK);
  *change_vector = vec_multiply(1, *change_vector);
  if (vec_magnitude(*change_vector) > MAX_STRETCH){
    *change_vector = vec_unit(*change_vector);
    *change_vector = vec_multiply(MAX_STRETCH, *change_vector);
  }

  if(type == MOUSE_CLICKED){
    beavers_index++;
    LAST_CLICK = clicked_point;
    make_beaver(scene, beavers_index);
  }
  else if (type == MOUSE_DRAGGED) {
    *change_vector = vec_add(*change_vector, TIP);
    new_tip = vec_init_pointer(change_vector->x, change_vector->y);
    draw_rubberband(scene, new_tip);
    if(beaver != NULL){
      body_set_centroid(beaver, *new_tip);
    }
  }
  else if (type == MOUSE_RELEASED){
    if(beaver != NULL){
      beavers_shot++;
      create_drag(scene, DRAG, beaver);
      launch_beaver(scene, beaver, change_vector);
      body_set_launched(beaver, true);
      create_earth_gravity(scene, G_CONST, beaver, scene_get_body(scene, 2));
      create_earth_gravity(scene, G_CONST, beaver, scene_get_body(scene, 3));
      new_tip = vec_init_pointer(TIP.x, TIP.y);
      draw_rubberband(scene, new_tip);
    }
  }
}

// Draws basic square for sprites.
body_t *draw_square(scene_t *scene, vector_t centroid, double size){
  list_t *listOfPoints = list_init(4, NULL);
  list_add(listOfPoints, vec_init_pointer(0, 0));
  list_add(listOfPoints, vec_init_pointer(size, 0));
  list_add(listOfPoints, vec_init_pointer(size, size));
  list_add(listOfPoints, vec_init_pointer(0, size));
  int *body_type = malloc(sizeof(int));
  *body_type = BLOCK_TYPE;
  list_t *body_info = list_init(2, NULL);
  list_add(body_info, body_type);
  body_t *square = body_init_with_info(listOfPoints, BEAVER_MASS, CLEAR,
    body_info, NULL);
  body_set_centroid(square, centroid);
  return square;
}

// Adding sprites to square.
void attach_sprites(scene_t *scene,
                    SDL_Texture *normal_beav_texture,
                    SDL_Texture *fancy_beav_texture,
                    SDL_Texture *corona_texture,
                    SDL_Texture *hurt_rona_texture,
                    SDL_Texture *background
) {
  sdl_put_image_on_body(background, scene_get_body(scene, 0));
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    int beaver_num = 0;
    body_t *body = scene_get_body(scene, i);
    int body_type = get_body_type(body);
    double health = get_body_health(body);
    // Right now the third beaver your shoot will be the fancy beaver.
    // Otherwise make the normal beaver
    if (body_type == BEAVER_TYPE) {
      beaver_num++;
      sdl_put_image_on_body(normal_beav_texture, body);
    }
    else if (body_type == FANCY_BEAVER_TYPE) {
      beaver_num++;
      sdl_put_image_on_body(fancy_beav_texture, body);
    }
    else if (body_type == VIRUS_TYPE) {
      if (health >= VIRUS_HEALTH) {
        sdl_put_image_on_body(corona_texture, body);
      }
      else {
        sdl_put_image_on_body(hurt_rona_texture, body);
      }
    }
  }
}

//Key handler
void on_key(char key, event_type_t type, double held_time, void *scene){
  if (type == KEY_PRESSED){
    // Events when key is pressed.
     switch (key){
       case ' ':
        break;
     }
   }
   else {
     // Events when key is released.
     switch (key){
         case ' ':
           for (size_t i = 0; i < scene_bodies(scene); i++) {
             body_t *body = scene_get_body(scene, i);
             vector_t body_velo = body_get_velocity(body);
             if (get_body_type(body) == FANCY_BEAVER_TYPE &&
                 vec_magnitude(body_velo) > BOOST_VELO && !used_boost) {
                   body_velo = vec_multiply(2, body_velo);
                   body_set_velocity(body, body_velo);
                   used_boost = true;
                 }
          }
        break;
   }
 }
}

// Main funciton.
int main(void) {
  // initializes the scene and stuff
    sdl_init(VEC_ZERO, WINDOW);
    sdl_mouse_setter(on_click);
    sdl_on_key(on_key);
    // loads the images
    SDL_Surface *normal_beav_Surface = IMG_Load("./images/normal_beav.png");
    if (normal_beav_Surface == NULL) {
      fprintf(stderr, "Normal Beav image was not loaded, check repo\n");
      printf("Normal Beav was not loaded\n");
      return 1;
    }
    SDL_Surface *fancy_beav_Surface = IMG_Load("./images/fancy_beav.png");
    if (fancy_beav_Surface == NULL) {
      fprintf(stderr, "Fancy Beav image was not loaded, check repo\n");
      printf("Fancy Beav was not loaded\n");
      return 1;
    }
    SDL_Surface *corona_Surface = IMG_Load("./images/rona.png");
    if (normal_beav_Surface == NULL) {
      fprintf(stderr, "Corona sprite was not loaded, check repo\n");
      printf("Corona Sprite was not loaded\n");
      return 1;
    }

    SDL_Surface *background_image_surface =
      IMG_Load("./images/modified_background.png");
    if (background_image_surface == NULL) {
      fprintf(stderr, "Modified Background image was not loaded, check repo\n");
      printf("Modified Background was not loaded\n");
      return 1;
    }

    SDL_Surface *hurt_rona_surface = IMG_Load("./images/hurt_rona.png");
    if (hurt_rona_surface == NULL) {
      fprintf(stderr, "Hurt rona image was not loaded, check repo\n");
      printf("Hurt Rona Image was not loaded\n");
      return 1;
    }

    SDL_Texture *background_image_texture =
      sdl_make_image(background_image_surface);
    SDL_Texture *normal_beav_texture = sdl_make_image(normal_beav_Surface);
    SDL_Texture *fancy_beav_texture = sdl_make_image(fancy_beav_Surface);
    SDL_Texture *corona_texture = sdl_make_image(corona_Surface);
    SDL_Texture *hurt_rona_texture = sdl_make_image(hurt_rona_surface);

    TTF_Font *verdanaFont = TTF_OpenFont("./fonts/Verdana.ttf", FONT_SIZE);
    if (verdanaFont == NULL) {
      fprintf(stderr, "Verdana font was not loaded, check repo\n");
      printf("Verdana Font was not loaded\n");
      return 1;
    }

    TTF_Font *timesFont = TTF_OpenFont("./fonts/Times New Roman.ttf",
      FONT_SIZE);
    if (timesFont == NULL) {
      fprintf(stderr, "Times New Roman font was not loaded, check repo\n");
      printf("Times New Roman Font was not loaded\n");
      return 1;
    }

    TTF_Font *fancyFont = TTF_OpenFont("./fonts/Xcelsion Italic.ttf",
      FONT_SIZE);
    if (fancyFont == NULL) {
      fprintf(stderr, "Fancy XCelsion font was not loaded, check repo\n");
      printf("Font was not loaded\n");
      return 1;
    }

    char *title = malloc(DEFAULT_STRING * sizeof(char));
    char *loading = malloc(DEFAULT_STRING * sizeof(char));
    char *hint = malloc(2 * DEFAULT_STRING * sizeof(char));
    title = "Angry Beavers";
    loading = "Loading...";
    hint = "Hint: Use the Space Bar for a Boost on the Special Beaver!";

    SDL_Texture *title_texture = sdl_make_text(title, fancyFont, ORANGE);
    SDL_Texture *loading_texture = sdl_make_text(loading, fancyFont, BLACK);
    SDL_Texture *hint_texture = sdl_make_text(hint, timesFont,
      RUBBER_BAND_COLOR);

    char *message_text1 = malloc(DEFAULT_STRING * sizeof(char));
    char *message_text2 = malloc(DEFAULT_STRING * sizeof(char));
    message_text1 = "Help! Coronavirus has taken over the world!";
    message_text2 = "We need your help get the Beavers back to Caltech!";

    SDL_Texture *message_texture = sdl_make_text(message_text1, timesFont,
      BLACK);
    SDL_Texture *message_texture2 = sdl_make_text(message_text2, timesFont,
       BLACK);

    vector_t MESSAGE_POSITION2 = vec_subtract(MESSAGE_POSITION, vec_init(0,
      155));

    //Declare the Text For Level 1
    char *level_one_text = malloc(DEFAULT_STRING * sizeof(char));
    level_one_text = "Level 1";
    SDL_Texture *level_one_texture = sdl_make_text(level_one_text, timesFont,
      ORANGE);

    char *level_two_text = malloc(DEFAULT_STRING * sizeof(char));
    level_two_text = "Level 2";
    SDL_Texture *level_two_texture = sdl_make_text(level_two_text, timesFont,
      ORANGE);

    char *level_three_text = malloc(DEFAULT_STRING * sizeof(char));
    level_three_text = "Level 3";
    SDL_Texture *level_three_texture = sdl_make_text(level_three_text,
      timesFont,
      ORANGE);

    char *game_over_text = malloc(DEFAULT_STRING * sizeof(char));
    game_over_text = "GAME OVER! You Suck!";
    SDL_Texture *game_over_texture = sdl_make_text(game_over_text, timesFont,
      ORANGE);

    char *winner_text = malloc(DEFAULT_STRING * sizeof(char));
    winner_text = "WOOOO! The beavers can go back to Campus";
    SDL_Texture *winner_texture = sdl_make_text(winner_text, timesFont, ORANGE);

    /**
      Screen that we are once
      0 - loading Screen
      1 - Message Screen
      2 - Level 1
      3 - Level 2
      4 - Level 3
      5 - Game Over
      6 - Winner!
    **/
    int screen = 0;
    double message_time = 0;
    double intro_time = 0;
    double clock = 0;
    double dt = 0;
    int status = 1;
    bool clock_start = false;
    bool is_screen_made = false;
    scene_t *bigScene = scene_init();

    char *score_text = malloc(DEFAULT_STRING * sizeof(char));
    char *beavers_left_text = malloc(DEFAULT_STRING * sizeof(char));
    SDL_Texture *score_texture = sdl_make_text(score_text, timesFont, ORANGE);
    SDL_Texture *beav_left_texture = sdl_make_text(beavers_left_text, timesFont,
      ORANGE);

    while (!sdl_is_done(bigScene)) {
      check_spinning(bigScene);
      remove_off_screen(bigScene);
      if (screen == LOADING_SCREEN) {
          if (!is_screen_made) {
            body_t *left_beaver = draw_square(bigScene,
              LEFT_BEAVER_POS, INTRO_SQUARE_SIZE);
            body_t *virus = draw_square(bigScene,
              CENTER_RONA_POS, INTRO_SQUARE_SIZE);
            body_t *right_beaver = draw_square(bigScene,
              RIGHT_BEAVER_POS, INTRO_SQUARE_SIZE);
            scene_add_body(bigScene, left_beaver);
            scene_add_body(bigScene, virus);
            scene_add_body(bigScene, right_beaver);
            is_screen_made = true;
          }
          if (intro_time >= INTRO_SCREEN_TIME) {
           screen++;
           scene_clear(bigScene);

         }
         else {
         dt = time_since_last_tick();
         intro_time += dt;
         sdl_put_image_on_body(normal_beav_texture, scene_get_body(bigScene,
            0));
         sdl_put_image_on_body(fancy_beav_texture, scene_get_body(bigScene, 2));
         sdl_put_image_on_body(corona_texture, scene_get_body(bigScene, 1));
         sdl_render_text(title_texture, TITLE_POSITION, TITLE_SIZE);
         sdl_render_text(loading_texture, LOADING_POSITION, LOADING_SIZE);
         sdl_render_text(hint_texture, HINT_LOCATION, HINT_SIZE);
         sdl_render_scene(bigScene);
         sdl_clear();
       }
      }
      else if (screen == MESSAGE_SCREEN) {
          dt = time_since_last_tick();
          if (message_time >= MESSAGE_SCREEN_TIME) {
            screen = LEVEL_ONE;
            scene_clear(bigScene);
            is_screen_made = false;
          }
          else {
            message_time += dt;
            sdl_render_text(message_texture, MESSAGE_POSITION, MESSAGE_SIZE);
            sdl_render_text(message_texture2, MESSAGE_POSITION2, MESSAGE_SIZE);
            sdl_render_scene(bigScene);
            sdl_clear();
        }
      }
      else if (screen == LEVEL_ONE) {
          dt = time_since_last_tick();
          if (!is_screen_made) {
            make_level_one(bigScene);
            is_screen_made = true;
            beavers_shot = 0;
            clock_start = false;
            clock = 0;
            status = 1;
          }
          sprintf(score_text, "Score: %d", score);
          score_texture = sdl_make_text(score_text, timesFont, ORANGE);

          sprintf(beavers_left_text, "Beavers Left: %d", TOTAL_BEAVERS -
            beavers_shot);
          beav_left_texture = sdl_make_text(beavers_left_text, timesFont,
              BLACK);

          if(clock_start == true){
            clock += dt;
          }
          check_spinning(bigScene);
          health(bigScene);
          physics_collide(bigScene);
          scene_tick(bigScene, dt);
          // Attach the sprites to the body
          attach_sprites(bigScene,
                        normal_beav_texture,
                        fancy_beav_texture,
                        corona_texture,
                        hurt_rona_texture,
                        background_image_texture);
          // Here we will check if the game is OVER
          // If game is over we will old screen for 5 sec and then break to
          // Game over screen. Else it will go to the next level
          status = is_game_over(bigScene);

          if (status == YOU_LOST) {
            clock_start = true;
            status = is_game_over(bigScene);
            if(status == YOU_LOST && clock > GAME_OVER_HOLD) {
              screen = GAME_OVER_SCREEN;
              is_screen_made = false;
              scene_clear(bigScene);
            }
          }
          else if (status == YOU_WON) {
            screen = LEVEL_TWO;
            is_screen_made = false;
            scene_clear(bigScene);
          }
          sdl_render_text(level_one_texture, LEVEL_POSITION, LEVEL_SIZE);
          sdl_render_text(score_texture, SCORE_POSITION, SCORE_SIZE);
          sdl_render_text(beav_left_texture, BEAV_LEFT_POSITION,
              BEAV_LEFT_SIZE);
          sdl_render_scene(bigScene);
          sdl_clear();
        }
        else if (screen == LEVEL_TWO) {
          dt = time_since_last_tick();
          if (!is_screen_made) {
            scene_clear(bigScene);
            make_level_two(bigScene);
            is_screen_made = true;
            score += LEVEL_SCORE_INCREMENT;
            beavers_shot = 0;
            clock_start = false;
            clock = 0;
            status = 1;
          }
          sprintf(score_text, "Score: %d", score);
          score_texture = sdl_make_text(score_text, timesFont, ORANGE);

          sprintf(beavers_left_text, "Beavers Left: %d", TOTAL_BEAVERS -
            beavers_shot);
          beav_left_texture = sdl_make_text(beavers_left_text, timesFont,
             BLACK);

          if (clock_start == true){
            clock += dt;
          }
          check_spinning(bigScene);
          health(bigScene);
          scene_tick(bigScene, dt);
          physics_collide(bigScene);
          // Attach the sprites to the body
          attach_sprites(bigScene,
                        normal_beav_texture,
                        fancy_beav_texture,
                        corona_texture,
                        hurt_rona_texture,
                        background_image_texture);
          // Here we will check if the game is OVER
          // If game is over we will old screen for 5 sec and then break to
          // Game over screen. Else it will go to the next level
          status = is_game_over(bigScene);
          if (status == YOU_LOST) {
            clock_start = true;
            status = is_game_over(bigScene);
            if(status == YOU_LOST && clock > GAME_OVER_HOLD) {
              screen = GAME_OVER_SCREEN;
              is_screen_made = false;
              scene_clear(bigScene);
            }
          }
          else if (status == YOU_WON) {
            screen++;
            is_screen_made = false;
          }

          sdl_render_text(level_two_texture, LEVEL_POSITION, LEVEL_SIZE);
          sdl_render_text(score_texture, SCORE_POSITION, SCORE_SIZE);
          sdl_render_text(beav_left_texture, BEAV_LEFT_POSITION,
            BEAV_LEFT_SIZE);
          sdl_render_scene(bigScene);
          sdl_clear();
        }
        else if (screen == LEVEL_THREE) {
          if (!is_screen_made) {
            scene_clear(bigScene);
            make_level_three(bigScene);
            is_screen_made = true;
            score += LEVEL_SCORE_INCREMENT;
            beavers_shot = 0;
            clock_start = false;
            clock = 0;
            status = 1;
          }
          sprintf(score_text, "Score: %d", score);
          score_texture = sdl_make_text(score_text, timesFont, ORANGE);

          sprintf(beavers_left_text, "Beavers Left: %d", TOTAL_BEAVERS -
           beavers_shot);
          beav_left_texture = sdl_make_text(beavers_left_text, timesFont,
            BLACK);
          dt = time_since_last_tick();

          if(clock_start == true) {
            clock += dt;
          }
          check_spinning(bigScene);
          health(bigScene);
          physics_collide(bigScene);
          scene_tick(bigScene, dt);
          attach_sprites(bigScene,
                          normal_beav_texture,
                          fancy_beav_texture,
                          corona_texture,
                          hurt_rona_texture,
                          background_image_texture);
          status = is_game_over(bigScene);
          if (status == YOU_LOST) {
            clock_start = true;
            status = is_game_over(bigScene);
            if(status == YOU_LOST && clock > GAME_OVER_HOLD){
              screen = GAME_OVER_SCREEN;
              is_screen_made = false;
              scene_clear(bigScene);
            }
          }
          else if (status == YOU_WON) {
            screen = WINNER_SCREEN;
            is_screen_made = false;
            scene_clear(bigScene);
          }
          sdl_render_text(level_three_texture, LEVEL_POSITION, LEVEL_SIZE);
          sdl_render_text(score_texture, SCORE_POSITION, SCORE_SIZE);
          sdl_render_text(beav_left_texture, BEAV_LEFT_POSITION,
             BEAV_LEFT_SIZE);
          sdl_render_scene(bigScene);
          sdl_clear();
        }
        else if (screen == GAME_OVER_SCREEN) {
          sdl_render_text(game_over_texture, GAME_OVER_POSITION,
            GAME_OVER_SIZE);
          sdl_render_scene(bigScene);
          sdl_clear();
        }
        else if (screen == WINNER_SCREEN) {
          sdl_render_text(winner_texture, WINNER_POSITION, WINNER_SIZE);
          sdl_render_scene(bigScene);
          sdl_clear();
        }
      }
    }
