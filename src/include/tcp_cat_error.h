#ifndef BARE_ADDON_TCP_CAT_ERROR_H
#define BARE_ADDON_TCP_CAT_ERROR_H

typedef struct tcp_cat_error_s tcp_cat_error;

tcp_cat_error *
tcp_cat_error_new(void);

void
tcp_cat_error_free(tcp_cat_error *error);

const char *
tcp_cat_error_get_name(const tcp_cat_error *error);

const char *
tcp_cat_error_get_message(const tcp_cat_error *error);

void
tcp_cat_error_set_name(tcp_cat_error *error, const char *name);

void
tcp_cat_error_set_message(tcp_cat_error *error, const char *message);

#endif // BARE_ADDON_TCP_CAT_ERROR_H