#include "tcp_cat_uv_context.h"

#include "tcp_cat_error.h"
#include "tcp_cat_uv.h"

#include <stdlib.h>
#include <string.h>

struct tcp_cat_context_s {
  tcp_cat_resolve_callback *resolve;
  tcp_cat_reject_callback *reject;

  void *callback_data;

  tcp_cat_response *response;
  char *message;

  tcp_cat_error *pending_error;
  bool has_pending_error;
};

tcp_cat_uv_context *
tcp_cat_uv_context_new(const char *message, tcp_cat_resolve_callback resolve, tcp_cat_reject_callback reject, void *callback_data) {
  tcp_cat_uv_context *const context = malloc(sizeof(*context));
  if (context == NULL) {
    return NULL;
  }

  tcp_cat_response *const response = tcp_cat_response_new();
  if (response == NULL) {
    free(context);
    return NULL;
  }

  context->message = strdup(message);
  if (context->message == NULL) {
    tcp_cat_response_free(response);
    free(context);

    return NULL;
  }
  context->response = response;

  context->pending_error = tcp_cat_error_new();
  if (context->pending_error == NULL) {
    tcp_cat_response_free(context->response);
    free(context->message);
    free(context);

    return NULL;
  }

  context->callback_data = callback_data;
  context->resolve = resolve;
  context->reject = reject;
  context->has_pending_error = false;

  return context;
}

void
tcp_cat_uv_context_free(tcp_cat_uv_context *context) {
  if (context == NULL) {
    return;
  }
  tcp_cat_response_free(context->response);
  tcp_cat_error_free(context->pending_error);
  free(context->message);

  free(context);
}

void
tcp_cat_uv_context_resolve(tcp_cat_uv_context *const context) {
  context->resolve(context->response, context->callback_data);
}

void
tcp_cat_uv_context_reject(tcp_cat_uv_context *const context) {
  context->reject(
    tcp_cat_error_get_name(context->pending_error),
    tcp_cat_error_get_message(context->pending_error),
    context->callback_data
  );
}

int
tcp_cat_uv_context_append_response(const tcp_cat_uv_context *const context, const char *buffer, size_t nread) {
  return tcp_cat_response_append(context->response, buffer, nread);
}

const char *
tcp_cat_uv_context_get_message(const tcp_cat_uv_context *const context) {
  return context->message;
}

void
tcp_cat_context_set_pending_error(tcp_cat_uv_context *context, const char *const name, const char *const message) {
  if (context->has_pending_error) {
    return;
  }
  tcp_cat_error_set_message(context->pending_error, message);
  tcp_cat_error_set_name(context->pending_error, name);
  // We just set it, so ---
  context->has_pending_error = true;
}

bool
tcp_cat_context_has_pending_error(const tcp_cat_uv_context *context) {
  return context->has_pending_error;
}