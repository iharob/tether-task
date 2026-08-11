#include "tcp_cat_js_context.h"

struct tcp_cat_js_context_s {
  js_deferred_t *deferred;
  js_env_t *env;
  tcp_cat_arguments *arguments;
};

tcp_cat_js_context *
tcp_cat_js_context_new(js_env_t *env, js_deferred_t *deferred, tcp_cat_arguments *arguments) {
  tcp_cat_js_context *context;

  context = malloc(sizeof(*context));
  if (context == NULL) {
    return NULL;
  }

  context->env = env;
  context->deferred = deferred;
  context->arguments = arguments;

  return context;
}

void
tcp_cat_js_context_free(tcp_cat_js_context *const context) {
  if (context == NULL) {
    return;
  }
  // DO NOT FREE context->env, context->deferred, as they are owned by the caller
  tcp_cat_arguments_free(context->arguments);
  free(context);
}

js_env_t *
tcp_cat_js_context_get_js_environment(const tcp_cat_js_context *context) {
  return context->env;
}

js_deferred_t *
tcp_cat_js_context_get_deferred(const tcp_cat_js_context *context) {
  return context->deferred;
}
