#ifndef BARE_ADDON_TCP_CAT_H
#define BARE_ADDON_TCP_CAT_H

#include "tcp_cat_arguments.h"
#include "tcp_cat_response.h"

#include <uv.h>

typedef void(tcp_cat_resolve_callback)(tcp_cat_response *, void *);
typedef void(tcp_cat_reject_callback)(const char *, const char *, void *);

void
tcp_cat_execute_async(uv_loop_t *loop, const tcp_cat_arguments *arguments, tcp_cat_resolve_callback resolve, tcp_cat_reject_callback reject, void *data);

#endif // BARE_ADDON_TCP_CAT_H
