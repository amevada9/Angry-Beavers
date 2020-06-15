#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include <stdbool.h>
#include "color.h"
#include "list.h"
#include "scene.h"
#include "vector.h"
#include "body.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "sdl_wrapper.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

// Values passed to a key handler when the given arrow key is pressed
#define UP_ARROW 0
#define DOWN_ARROW 1
#define LEFT_ARROW 2
#define RIGHT_ARROW 3
#define SPACE_BAR 4


/**
 * The possible types of key and/or mouse events.
 * Enum types in C are much more primitive than in Java; this is equivalent to:
 * typedef unsigned int EventType;
 * #define KEY_PRESSED 0
 * #define KEY_RELEASED 1
 * #define MOUSE_CLICKED 2
 * #define MOUSE_RELEASED 3
 * #define MOUSE_DRAGGED 4 (Moving the mouse with a click down)
 * #define MOUSE_MOVED 5 (Moving with mouse released)
 */
typedef enum {
    KEY_PRESSED,
    KEY_RELEASED,
    MOUSE_CLICKED,
    MOUSE_RELEASED,
    MOUSE_DRAGGED,
    MOUSE_MOVED
} event_type_t;


/**
 * A keypress handler.
 * When a key is pressed or released, the handler is passed its char value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param key a character indicating which key was pressed
 * @param type the type of event (KEY_PRESSED or KEY_RELEASED are handled here)
 * @param held_time if a press event, the time the key has been held in seconds
 */
typedef void (*key_handler_t)(char key, event_type_t type, double held_time,
  void *input);

/**
 * A mouseclick handler
 * When the mouse is clicked, moved, dragged, or released, the function can
 * recognize it. We want this seperate from the key handler as it will be
 * handled differently.
 * I added the uint32_t to the enum event_type so that we can compare all the
 * types of events together in one type (keeps it cleaner)
 *
 * @param type of the events that we want to handle
 *        MOUSE_CLICKED normal click
 *        MOUSE_RELEASED click was released
 *        MOUSE_DRAGGED mouse was moved with click down
 *        MOUSE_MOVED mouse was moved normally
 *
 * @param click_point the point in vector_t form of where the event occured
 * @param void *input of auxillary info, usually a scene to which to apply changes
 */
typedef void (*mouse_handler_t)(event_type_t type, vector_t click_point, void *input);

/**
 * Initializes the SDL window and renderer.
 * Must be called once before any of the other SDL functions.
 *
 * @param min the x and y coordinates of the bottom left of the scene
 * @param max the x and y coordinates of the top right of the scene
 */
void sdl_init(vector_t min, vector_t max);

/**
 * Processes all SDL events and returns whether the window has been closed.
 * This function must be called in order to handle keypresses.
 *
 * @return true if the window was closed, false otherwise
 */
bool sdl_is_done(void *input);

/**
 * Clears the screen. Should be called before drawing polygons in each frame.
 */
void sdl_clear(void);

/**
 * Draws a polygon from the given list of vertices and a color.
 *
 * @param points the list of vertices of the polygon
 * @param color the color used to fill in the polygon
 */
void sdl_draw_polygon(list_t *points, rgb_color_t color);

/**
 * Displays the rendered frame on the SDL window.
 * Must be called after drawing the polygons in order to show them.
 */
void sdl_show(void);

/**
 * Draws all bodies in a scene.
 * This internally calls sdl_clear(), sdl_draw_polygon(), and sdl_show(),
 * so those functions should not be called directly.
 *
 * @param scene the scene to draw
 */
void sdl_render_scene(scene_t *scene);

/**
 * Registers a function to be called every time a key is pressed.
 * Overwrites any existing handler.
 *
 * Example:
 * ```
 * void on_key(char key, key_event_type_t type, double held_time) {
 *     if (type == KEY_PRESSED) {
 *         switch (key) {
 *             case 'a':
 *                 printf("A pressed\n");
 *                 break;
 *             case UP_ARROW:
 *                 printf("UP pressed\n");
 *                 break;
 *         }
 *     }
 * }
 * int main(void) {
 *     sdl_on_key(on_key);
 *     while (!sdl_is_done());
 * }
 * ```
 *
 * @param handler the function to call with each key press
 */
void sdl_on_key(key_handler_t handler);

void sdl_mouse_setter(mouse_handler_t handler);

/**
 * Gets the amount of time that has passed since the last time
 * this function was called, in seconds.
 *
 * @return the number of seconds that have elapsed
 */
double time_since_last_tick(void);

/**
 * Function that creates the texture for the string that we want to use
 * Will be used in a render text function to put text on the screen.
 *
 * @param const char *string a string that we want to render onscreen
 * @param TTF_Font a font of type TTF_Font pointer that we want to use
 *        will need to initilaize the font before hand and ensure that the size
 *        is defined as well as the .ttf file is included in the repo
 *
 * @param rgb_color_t color of the text color that we want. Will be converted
 *        to a SDL_Color in the function and inserted into text
 *
 * @return a SDL_Texture pointer that we can render
 *
 **/
SDL_Texture *sdl_make_text(char *string, TTF_Font *font, rgb_color_t color);

/**
 * This function allows us to render text on a window
 * Intakes a const string, that we want to render in a certain font, color,
 * and area
 * Can be used for any thing on screen
 *
 * @param a SDL_Texture pointer that we want to render. Already has font and color
 *
 * @param vector_t position on the screen of the text that we want
 * @param vector_t size of the surface. Will be a rectangle of size {x, y}
 *
 */
void sdl_render_text(SDL_Texture *textTexture, vector_t position, vector_t size);

/**
 * Function that creates the texture for the image that we want to use
 * Will be used in a render image function to put text on the screen.
 *
 * @param an SDL_Surface image that we loaded. FIle needs to be directed.
 *
 * @return a SDL_Texture pointer that we can render
 *
 **/
SDL_Texture *sdl_make_image(SDL_Surface *image);

/**
 * Function that renders the image on the a body that we specify
 * Because of the nature and extent of our SDL knowledge, we need the body
 * to be a rectangle whose points are in counter clockwise order from the bottom
 * left. This way the SDL_Rect object can be created quickly.
 * All adjustments have been made so it renders on our screen.
 *
 * @param an SDL_Texture pointer that is the image we want to render
 * @param a body_t pointer of the rectangular body that we want to render
 *        the image on.
 * 
 **/
void sdl_put_image_on_body(SDL_Texture *image_texture, body_t *body);

#endif // #ifndef __SDL_WRAPPER_H__
