#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"
#include "polygon.h"

const size_t GROWTH_FACTOR = 2;

/**
Defines a list_t with fields size, a data list of vector_t's, and a capacity.
*/
typedef struct list {
  size_t size;
  void **data;
  size_t capacity;
  free_func_t freer;
} list_t;

/**
Initialize the list and fields of a list_t.
*/
list_t *list_init(size_t initial_size, free_func_t freer) {
  if(initial_size == 0){
    initial_size = 1;
  }
  list_t *list = malloc(sizeof(list_t));
  assert(list != NULL);
  list->data = malloc(initial_size * sizeof(void *));
  assert(list->data != NULL);
  list->size = 0;
  list->capacity = initial_size;
  list->freer = freer;
  return list;
}

/**
Frees all memory associated with a list.
*/
void list_free(list_t *list) {
  if (list->freer != NULL) {
    for (size_t i = 0; i < list->size; i++){
      list->freer(list->data[i]);
    }
  }
  free(list->data);
  free(list);
}

/**
Returns the size of a list.
*/
size_t list_size(list_t *list) {
  return list->size;
}

/**
Resizes a list if it has reached its capacity.
*/
void capacity_check(list_t *list) {
  list->data =
    realloc(list->data , list->capacity * GROWTH_FACTOR * sizeof(void *));
  assert(list->data != NULL);
  list->capacity *= GROWTH_FACTOR;
}

/**
Gets the element at a given index in a list.
*/
void *list_get(list_t *list, size_t index) {
  assert(list->size > index);
  void *temp = list->data[index];
  return temp;
}

/**
Appends an element to the end of a list.
*/
void list_add(list_t *list, void *value) {
  if(list->size >= list->capacity){
    capacity_check(list);
  }
  assert(list->size < list->capacity && value != NULL);
  list->data[list->size] = value;
  list->size++;
}

/**
Removes an element from the list at a given index.
*/
void *list_remove(list_t *list, size_t index) {
  assert(list->size > index);
  void *removedElement = list->data[index];
  for(size_t i = index; i < list->size - 1; i++){
    list->data[i] = list->data[i + 1];
  }
  list->size--;
  return removedElement;
}
