#ifndef BARE_ADDON_TCP_CAT_JS_CONTEXT_H
#define BARE_ADDON_TCP_CAT_JS_CONTEXT_H

#include "tcp_cat_arguments.h"
#include "tcp_cat_uv_context.h"

#include <js.h>

typedef struct tcp_cat_js_context_s tcp_cat_js_context;

tcp_cat_js_context *
tcp_cat_js_context_new(js_env_t *env, js_deferred_t *deferred, tcp_cat_arguments *arguments);

void
tcp_cat_js_context_free(tcp_cat_js_context *context);

js_env_t *
tcp_cat_js_context_get_js_environment(const tcp_cat_js_context *context);

js_deferred_t *
tcp_cat_js_context_get_deferred(const tcp_cat_js_context *context);

#endif // BARE_ADDON_TCP_CAT_JS_CONTEXT_H
