#include "tcp_cat_error.h"

#include <stdlib.h>
#include <string.h>
#include <uv.h>

struct tcp_cat_error_s {
  char name[32];
  char message[128];
};

tcp_cat_error *
tcp_cat_error_new(void) {
  tcp_cat_error *const new_error = malloc(sizeof(*new_error));
  if (new_error == NULL) {
    return NULL;
  }
  return new_error;
}

void
tcp_cat_error_free(tcp_cat_error *const error) {
  if (error == NULL) {
    return;
  }
  free(error);
}

const char *
tcp_cat_error_get_name(const tcp_cat_error *const error) {
  return error->name;
}

const char *
tcp_cat_error_get_message(const tcp_cat_error *const error) {
  return error->message;
}

void
tcp_cat_error_set_name(tcp_cat_error *const error, const char *const name) {
  if (error == NULL) {
    return;
  }

  strncpy(error->name, name, sizeof(error->name) - 1);
  error->name[sizeof(error->name) - 1] = '\0';
}

void
tcp_cat_error_set_message(tcp_cat_error *const error, const char *const message) {
  if (error == NULL) {
    return;
  }

  strncpy(error->message, message, sizeof(error->message) - 1);
  error->message[sizeof(error->message) - 1] = '\0';
}