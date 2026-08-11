#ifndef BARE_ADDON_TCP_CAT_RETURN_VALUE_H
#define BARE_ADDON_TCP_CAT_RETURN_VALUE_H

#include <stddef.h>

typedef struct {
  unsigned char *data;
  size_t length;
  size_t capacity;
} tcp_cat_response;

int
tcp_cat_response_append(tcp_cat_response *return_value, const char *buffer, size_t count);

void
tcp_cat_response_free(tcp_cat_response *return_value);

tcp_cat_response *
tcp_cat_response_new(void);

#endif // BARE_ADDON_TCP_CAT_RETURN_VALUE_H
