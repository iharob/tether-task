#include "tcp_cat_js.h"

#include "errors.h"
#include "js_tcp_cat_helpers.h"
#include "tcp_cat_arguments.h"
#include "tcp_cat_js_context.h"
#include "tcp_cat_response.h"

static void
on_resolve(tcp_cat_response *response, void *data) {
  tcp_cat_js_context *const context = data;

  js_env_t *env;
  js_handle_scope_t *scope;
  js_value_t *resolution = NULL;

  int err;

  assert(context != NULL);

  env = tcp_cat_js_context_get_js_environment(context);
  err = js_open_handle_scope(env, &scope);
  if (err < 0) {
    fprintf(stderr, "FATAL INTERNAL ERROR: Failed to open handle scope\n");
    abort();
  }

  err = tcp_cat_response_to_js_arraybuffer(env, response, &resolution);
  if (err < 0) {
    err = tcp_cat_js_create_error(env, ERROR_RESPONSE_TO_ARRAYBUFFER, "Failed to convert response to ArrayBuffer", &resolution);
    if (err < 0) {
      fprintf(stderr, "FATAL INTERNAL ERROR: Failed to create error object\n");
      abort();
    }
    err = js_reject_deferred(env, tcp_cat_js_context_get_deferred(context), resolution);
    if (err < 0) {
      fprintf(stderr, "FATAL INTERNAL ERROR: Failed to reject deferred\n");
      abort();
    }
  } else {
    err = js_resolve_deferred(env, tcp_cat_js_context_get_deferred(context), resolution);
    if (err < 0) {
      fprintf(stderr, "FATAL INTERNAL ERROR: Failed to resolve deferred\n");
      abort();
    }
  }

  js_close_handle_scope(env, scope);
  tcp_cat_js_context_free(context);
}

static void
on_reject(const char *error_name, const char *error_message, void *data) {
  tcp_cat_js_context *const context = data;

  js_handle_scope_t *scope;
  js_value_t *resolution;
  js_env_t *env;

  int err;

  assert(context != NULL);

  env = tcp_cat_js_context_get_js_environment(context);
  err = js_open_handle_scope(env, &scope);
  if (err < 0) {
    fprintf(stderr, "FATAL INTERNAL ERROR: Failed to open handle scope\n");
    abort();
  }

  err = tcp_cat_js_create_error(env, error_name, error_message, &resolution);
  if (err < 0) {
    fprintf(stderr, "FATAL INTERNAL ERROR: Failed to create error object\n");
    // Even though at this point we have the scope ready,
    // if this happens we can barely continue anyway as the
    // reason is probably OOM. So we abort because there is
    // no reason to keep going.
    abort();
  }

  err = js_reject_deferred(env, tcp_cat_js_context_get_deferred(context), resolution);
  if (err < 0) {
    fprintf(stderr, "FATAL INTERNAL ERROR: Failed to reject deferred\n");
    // This error is like the ones above, unrecoverable.
    abort();
  }
  js_close_handle_scope(env, scope);
  tcp_cat_js_context_free(context);
}

static int
tcp_cat_js_create_arguments_parse_error(js_env_t *env, tcp_cat_parse_arguments_error parse_error, js_value_t **error) {
  int err = 0;

  switch (parse_error) {
  case TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_COUNT:
    err = tcp_cat_js_create_error(env, ERROR_ARGUMENT_COUNT, "Expected 3 arguments (IP address, port, message)", error);
    break;
  case TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_ARGUMENT_TYPE_FIRST:
    err = tcp_cat_js_create_error(env, ERROR_ARGUMENT_TYPE, "First argument must be a string (IP address)", error);
    break;
  case TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_ARGUMENT_TYPE_SECOND:
    err = tcp_cat_js_create_error(env, ERROR_ARGUMENT_TYPE, "Second argument must be a number (port)", error);
    break;
  case TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_ARGUMENT_TYPE_THIRD:
    err = tcp_cat_js_create_error(env, ERROR_ARGUMENT_TYPE, "Third argument must be a string (message)", error);
    break;
  case TCP_CAT_PARSE_ARGUMENTS_ERROR_PORT_OUT_OF_RANGE:
    err = tcp_cat_js_create_error(env, ERROR_PORT_OUT_OF_RANGE, "Port must be between 1 and 65535", error);
    break;
  case TCP_CAT_PARSE_ARGUMENTS_ERROR_NO_MEMORY:
  case TCP_CAT_PARSE_ARGUMENTS_ERROR_OUT_OF_MEMORY:
    err = tcp_cat_js_create_error(env, ERROR_NO_MEMORY, ERROR_MESSAGE_OUT_OF_MEMORY, error);
    break;
  case TCP_CAT_PARSE_ARGUMENTS_ERROR_CANNOT_DETERMINE_TYPE:
    err = tcp_cat_js_create_error(env, ERROR_ARGUMENT_TYPE, "Failed to determine argument type", error);
    break;
  case TCP_CAT_PARSE_ARGUMENTS_ERROR_OTHER:
    err = tcp_cat_js_create_error(env, ERROR_UNKNOWN, "Failed to parse arguments", error);
    break;
  case TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_TYPE:
  case TCP_CAT_PARSE_ARGUMENTS_ERROR_UINT16_OUT_OF_RANGE:
  case TCP_CAT_PARSE_ARGUMENTS_SUCCESS:
    // Internal-only variants: these are remapped to argument-specific errors
    // by tcp_cat_parse_arguments before reaching this switch.
    err = tcp_cat_js_create_error(env, ERROR_UNKNOWN, ERROR_MESSAGE_UNKNOWN, error);
    break;
  }

  return err;
}

void
tcp_cat_execute(js_env_t *env, js_callback_info_t *info, js_deferred_t *deferred) {
  js_value_t *error = NULL;
  tcp_cat_arguments *arguments = NULL;
  uv_loop_t *loop;
  int err;

  err = js_get_env_loop(env, &loop);
  if (err < 0) {
    err = tcp_cat_js_create_error(env, ERROR_GETTING_UV_LOOP, "Failed to get UV loop from JS environment", &error);
    goto error;
  }

  err = tcp_cat_parse_arguments(env, info, &arguments);
  if (err < 0) {
    err = tcp_cat_js_create_arguments_parse_error(env, err, &error);
    goto error;
  }

  tcp_cat_js_context *const context = tcp_cat_js_context_new(env, deferred, arguments);
  if (context == NULL) {
    err = tcp_cat_js_create_error(env, ERROR_NO_MEMORY, ERROR_MESSAGE_OUT_OF_MEMORY, &error);
    goto error;
  }
  tcp_cat_execute_async(loop, arguments, on_resolve, on_reject, context);

  return;

error:
  tcp_cat_arguments_free(arguments);

  if (error != NULL) {
    err = js_reject_deferred(env, deferred, error);
    if (err < 0) {
      fprintf(stderr, "FATAL INTERNAL ERROR: Failed to reject deferred\n");
      abort();
    }
  } else if (err < 0) {
    fprintf(stderr, "FATAL INTERNAL ERROR: Failed to create error object\n");
    abort();
  }
}
