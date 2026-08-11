#ifndef BARE_ADDON_TCP_CAT_ARGUMENTS_H
#define BARE_ADDON_TCP_CAT_ARGUMENTS_H

#include <stdint.h>

typedef struct tcp_cat_arguments_s tcp_cat_arguments;

tcp_cat_arguments *
tcp_cat_arguments_new(const char *ip_address, const char *message, uint16_t port);

void
tcp_cat_arguments_free(tcp_cat_arguments *arguments);

const char *
tcp_cat_arguments_get_message(const tcp_cat_arguments *arguments);

const char *
tcp_cat_arguments_get_ip_address(const tcp_cat_arguments *arguments);

uint16_t
tcp_cat_arguments_get_port(const tcp_cat_arguments *arguments);

#endif // BARE_ADDON_TCP_CAT_ARGUMENTS_H
