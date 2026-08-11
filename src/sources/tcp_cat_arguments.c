#include "tcp_cat_arguments.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct tcp_cat_arguments_s {
  char *ip_address;
  uint16_t port;
  char *message;
};

tcp_cat_arguments *
tcp_cat_arguments_new(const char *const ip_address, const char *const message, uint16_t port) {
  tcp_cat_arguments *const arguments = malloc(sizeof(*arguments));
  if (arguments == NULL) {
    return NULL;
  }

  arguments->ip_address = strdup(ip_address);
  if (arguments->ip_address == NULL) {
    free(arguments);
    return NULL;
  }

  arguments->message = strdup(message);
  if (arguments->message == NULL) {
    free(arguments->ip_address);
    free(arguments);
    return NULL;
  }

  arguments->port = port;

  return arguments;
}

void
tcp_cat_arguments_free(tcp_cat_arguments *const arguments) {
  if (arguments == NULL) {
    return;
  }

  free(arguments->ip_address);
  free(arguments->message);
  free(arguments);
}

const char *
tcp_cat_arguments_get_message(const tcp_cat_arguments *const arguments) {
  return arguments->message;
}

const char *
tcp_cat_arguments_get_ip_address(const tcp_cat_arguments *arguments) {
  return arguments->ip_address;
}

uint16_t
tcp_cat_arguments_get_port(const tcp_cat_arguments *arguments) {
  return arguments->port;
}
