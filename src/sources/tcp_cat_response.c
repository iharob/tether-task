#include "tcp_cat_response.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

tcp_cat_response *
tcp_cat_response_new(void) {
  tcp_cat_response *const return_value = malloc(sizeof(*return_value));
  if (return_value == NULL) {
    return NULL;
  }

  return_value->data = malloc(512);
  if (return_value->data == NULL) {
    free(return_value);
    return NULL;
  }
  return_value->length = 0;
  return_value->capacity = 512;

  return return_value;
}

int
tcp_cat_response_append(tcp_cat_response *const return_value, const char *const buffer, const size_t count) {
  unsigned char *data;

  if (return_value == NULL) {
    return -1;
  }

  if (count == 0) {
    // We are not appending anything so we do not need to allocate memory
    return 0;
  }

  if (count > SIZE_MAX - return_value->length) {
    return -1;
  }

  const size_t current_offset = return_value->length;
  const size_t new_length = return_value->length + count;
  if (new_length < return_value->capacity) {
    memcpy(return_value->data + current_offset, buffer, count);
    return_value->length += count;

    return 0;
  }

  size_t new_capacity = return_value->capacity;
  while (new_capacity < new_length) {
    if (new_capacity > SIZE_MAX / 2) {
      return -1;
    }

    new_capacity *= 2;
  }

  data = realloc(return_value->data, new_capacity);
  if (data == NULL) {
    return -1;
  }
  return_value->data = data;

  // Copy the bytes first at the right offset (do not touch the length yet).
  memcpy(return_value->data + current_offset, buffer, count);
  // Now update the length
  return_value->length += count;

  return 0;
}

void
tcp_cat_response_free(tcp_cat_response *return_value) {
  if (return_value == NULL) {
    return;
  }

  free(return_value->data);
  free(return_value);
}