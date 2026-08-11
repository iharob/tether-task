#ifndef BARE_ADDON_TCP_CAT_CONTEXT_H
#define BARE_ADDON_TCP_CAT_CONTEXT_H

#include "tcp_cat_uv.h"

#include <stdbool.h>

typedef struct tcp_cat_context_s tcp_cat_uv_context;

tcp_cat_uv_context *
tcp_cat_uv_context_new(const char *message, tcp_cat_resolve_callback resolve, tcp_cat_reject_callback reject, void *callback_data);

void
tcp_cat_uv_context_free(tcp_cat_uv_context *context);

void
tcp_cat_uv_context_resolve(tcp_cat_uv_context *context);

void
tcp_cat_uv_context_reject(tcp_cat_uv_context *context);

int
tcp_cat_uv_context_append_response(const tcp_cat_uv_context *context, const char *buffer, size_t nread);

const char *
tcp_cat_uv_context_get_message(const tcp_cat_uv_context *context);

void
tcp_cat_context_set_pending_error(tcp_cat_uv_context *context, const char *name, const char *message);

bool
tcp_cat_context_has_pending_error(const tcp_cat_uv_context *context);

#endif // BARE_ADDON_TCP_CAT_CONTEXT_H