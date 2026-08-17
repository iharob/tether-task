#include "tcp_cat_uv.h"

#include "errors.h"
#include "tcp_cat_uv_context.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static tcp_cat_uv_context *
get_context_from_uv_stream(const uv_stream_t *const stream) {
  if (stream == NULL) {
    return NULL;
  }
  return stream->data;
}

static void
on_alloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buffer) {
  (void) handle;

  buffer->base = malloc(suggested_size);
  buffer->len = suggested_size;
}

static void
on_close(uv_handle_t *handle) {
  tcp_cat_uv_context *const context = handle->data;
  assert(context != NULL);

  if (tcp_cat_context_has_pending_error(context)) {
    tcp_cat_uv_context_reject(context);
  } else {
    tcp_cat_uv_context_resolve(context);
  }
  tcp_cat_uv_context_free(context);
  free(handle);
}

static void
abort_on_error(uv_stream_t *stream, tcp_cat_uv_context *const context, int err) {
  tcp_cat_context_set_pending_error(context, uv_err_name(err), uv_strerror(err));
  uv_close((uv_handle_t *) stream, on_close);
}

static void
on_read(uv_stream_t *stream, const ssize_t nread, const uv_buf_t *buffer) {
  tcp_cat_uv_context *const context = get_context_from_uv_stream(stream);
  assert(context != NULL);

  if (nread < 0) {
    if (nread != UV_EOF) {
      tcp_cat_context_set_pending_error(context, uv_err_name((int) nread), uv_strerror((int) nread));
    }
    uv_read_stop(stream);
    uv_close((uv_handle_t *) stream, on_close);

    free(buffer->base);
    return;
  }

  const int err = tcp_cat_uv_context_append_response(context, buffer->base, nread);
  if (err < 0) {
    tcp_cat_context_set_pending_error(context, ERROR_NO_MEMORY, ERROR_MESSAGE_OUT_OF_MEMORY);

    uv_read_stop(stream);
    uv_close((uv_handle_t *) stream, on_close);
  }
  free(buffer->base);
}

static void
handle_write_error(uv_stream_t *stream, tcp_cat_uv_context *context, int err) {
  tcp_cat_context_set_pending_error(context, uv_err_name(err), uv_strerror(err));
  uv_close((uv_handle_t *) stream, on_close);
}

static void
on_write(uv_write_t *req, int status) {
  tcp_cat_uv_context *const context = get_context_from_uv_stream(req->handle);
  assert(context != NULL);

  if (status < 0) {
    if (status != UV_ECANCELED) {
      handle_write_error(req->handle, context, status);
    }
  }

  free(req);
}

static void
handle_connect_success(uv_connect_t *const connect_req, tcp_cat_uv_context *const context) {
  int err;
  uv_write_t *write = NULL;
  uv_buf_t buffer;

  write = malloc(sizeof(*write));
  if (write == NULL) {
    tcp_cat_context_set_pending_error(context, ERROR_NO_MEMORY, ERROR_MESSAGE_OUT_OF_MEMORY);
    uv_close((uv_handle_t *) connect_req->handle, on_close);

    return;
  }

  const char *const message = tcp_cat_uv_context_get_message(context);
  buffer = uv_buf_init((char *) message, strlen(message));

  err = uv_write(write, connect_req->handle, &buffer, 1, on_write);
  if (err < 0) {
    abort_on_error(connect_req->handle, context, err);

    free(write);
    return;
  }

  err = uv_read_start(connect_req->handle, on_alloc, on_read);
  if (err < 0) {
    abort_on_error(connect_req->handle, context, err);
  }
}

static void
on_connect(uv_connect_t *connect_req, int status) {
  tcp_cat_uv_context *const context = get_context_from_uv_stream(connect_req->handle);
  assert(context != NULL);

  if (status == 0) {
    handle_connect_success(connect_req, context);
  } else if (status < 0) {
    abort_on_error(connect_req->handle, context, status);
  }
  free(connect_req);
}

void
tcp_cat_execute_async(uv_loop_t *const loop, const tcp_cat_arguments *const arguments, tcp_cat_resolve_callback resolve, tcp_cat_reject_callback reject, void *data) {
  struct sockaddr_in address;
  uv_connect_t *connect;
  uv_tcp_t *socket;

  assert(loop != NULL);

  const char *const message = tcp_cat_arguments_get_message(arguments);
  // We keep all the data that will be exchanged across callbacks in this
  // structure.
  tcp_cat_uv_context *context = tcp_cat_uv_context_new(message, resolve, reject, data);
  if (context == NULL) {
    reject(ERROR_NO_MEMORY, ERROR_MESSAGE_OUT_OF_MEMORY, data);
    return;
  }

  connect = malloc(sizeof(*connect));
  if (connect == NULL) {
    reject(ERROR_NO_MEMORY, ERROR_MESSAGE_OUT_OF_MEMORY, data);
    tcp_cat_uv_context_free(context);

    return;
  }

  socket = malloc(sizeof(*socket));
  if (socket == NULL) {
    reject(ERROR_NO_MEMORY, ERROR_MESSAGE_OUT_OF_MEMORY, data);
    tcp_cat_uv_context_free(context);

    free(connect);

    return;
  }

  const char *const ip_address = tcp_cat_arguments_get_ip_address(arguments);
  const uint16_t port = tcp_cat_arguments_get_port(arguments);

  // This happens outside of the loop and can be synchronous
  int err = uv_ip4_addr(ip_address, port, &address);
  if (err < 0) {
    free(connect);
    free(socket);

    reject(uv_err_name(err), uv_strerror(err), data);
    tcp_cat_uv_context_free(context);

    return;
  }

  err = uv_tcp_init(loop, socket);
  if (err < 0) {
    free(connect);
    free(socket);

    reject(uv_err_name(err), uv_strerror(err), data);
    tcp_cat_uv_context_free(context);

    return;
  }
  socket->data = context;

  err = uv_tcp_connect(connect, socket, (const struct sockaddr *) &address, on_connect);
  if (err < 0) {
    tcp_cat_context_set_pending_error(context, uv_err_name(err), uv_strerror(err));
    // Free it here as `on_connect` will not run
    free(connect);
    // Close the socket so that on_close be called
    uv_close((uv_handle_t *) socket, on_close);
  }
}
