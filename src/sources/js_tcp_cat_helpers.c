#include "js_tcp_cat_helpers.h"

static tcp_cat_parse_arguments_error
js_read_uint16(js_env_t *env, js_value_t *value, uint16_t *result) {
  int err;
  js_value_type_t type;
  uint32_t uint32_result;

  err = js_typeof(env, value, &type);
  if (err < 0) {
    return TCP_CAT_PARSE_ARGUMENTS_ERROR_CANNOT_DETERMINE_TYPE;
  }

  if (type != js_number) {
    return TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_TYPE;
  }

  err = js_get_value_uint32(env, value, &uint32_result);
  if (err < 0) {
    return TCP_CAT_PARSE_ARGUMENTS_ERROR_OTHER;
  }

  if (uint32_result > UINT16_MAX) {
    return TCP_CAT_PARSE_ARGUMENTS_ERROR_UINT16_OUT_OF_RANGE;
  }
  *result = (uint16_t) uint32_result;

  return TCP_CAT_PARSE_ARGUMENTS_SUCCESS;
}

static tcp_cat_parse_arguments_error
js_string_strdup(js_env_t *env, js_value_t *value, char **result) {
  int err;
  js_value_type_t type;
  size_t length;

  err = js_typeof(env, value, &type);
  if (err < 0) {
    return TCP_CAT_PARSE_ARGUMENTS_ERROR_CANNOT_DETERMINE_TYPE;
  }

  if (type != js_string) {
    return TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_TYPE;
  }

  err = js_get_value_string_utf8(env, value, NULL, 0, &length);
  if (err < 0) {
    return TCP_CAT_PARSE_ARGUMENTS_ERROR_OTHER;
  }

  *result = malloc(length + 1);
  if (*result == NULL) {
    return TCP_CAT_PARSE_ARGUMENTS_ERROR_NO_MEMORY;
  }

  err = js_get_value_string_utf8(env, value, (utf8_t *) *result, length + 1, &length);
  if (err < 0) {
    free(*result);
    *result = NULL;

    return TCP_CAT_PARSE_ARGUMENTS_ERROR_OTHER;
  }

  return TCP_CAT_PARSE_ARGUMENTS_SUCCESS;
}

tcp_cat_parse_arguments_error
tcp_cat_parse_arguments(js_env_t *env, const js_callback_info_t *info, tcp_cat_arguments **arguments) {
  int err;
  js_value_t *values[3];
  size_t count;
  char *ip_address = NULL;
  char *message = NULL;
  uint16_t port;

  // Initial count must be equal to expected number of parameters
  count = 3;
  err = js_get_callback_info(env, info, &count, values, NULL, NULL);
  if (err < 0) {
    return TCP_CAT_PARSE_ARGUMENTS_ERROR_OTHER;
  }

  if (count != 3) {
    return TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_COUNT;
  }

  err = js_string_strdup(env, values[0], &ip_address);
  if (err < 0) {
    if (err == TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_TYPE) {
      err = TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_ARGUMENT_TYPE_FIRST;
    }
    goto error;
  }

  err = js_read_uint16(env, values[1], &port);
  if (err < 0) {
    if (err == TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_TYPE) {
      err = TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_ARGUMENT_TYPE_SECOND;
    } else if (err == TCP_CAT_PARSE_ARGUMENTS_ERROR_UINT16_OUT_OF_RANGE) {
      err = TCP_CAT_PARSE_ARGUMENTS_ERROR_PORT_OUT_OF_RANGE;
    }
    goto error;
  }

  if (port == 0) {
    err = TCP_CAT_PARSE_ARGUMENTS_ERROR_PORT_OUT_OF_RANGE;
    goto error;
  }

  err = js_string_strdup(env, values[2], &message);
  if (err < 0) {
    if (err == TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_TYPE) {
      err = TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_ARGUMENT_TYPE_THIRD;
    }
    goto error;
  }
  *arguments = tcp_cat_arguments_new(ip_address, message, port);

  free(ip_address);
  free(message);

  if (*arguments == NULL) {
    return TCP_CAT_PARSE_ARGUMENTS_ERROR_OUT_OF_MEMORY;
  }
  return 0;

error:
  free(ip_address);
  free(message);

  return err;
}

int
tcp_cat_response_to_js_arraybuffer(js_env_t *env, tcp_cat_response *const response, js_value_t **result) {
  void *data = NULL;

  if (response == NULL) {
    return -1;
  }

  const int err = js_create_arraybuffer(env, response->length, &data, result);
  if (err < 0) {
    return err;
  }

  if (response->data != NULL) {
    memcpy(data, response->data, response->length);
  }

  return 0;
}

int
tcp_cat_js_create_error(js_env_t *env, const char *error_code, const char *error_message, js_value_t **error) {
  js_value_t *js_error_message = NULL;
  js_value_t *js_error_code = NULL;

  int err;

  err = js_create_string_utf8(env, (const utf8_t *) error_message, -1, &js_error_message);
  if (err < 0) {
    return err;
  }

  err = js_create_string_utf8(env, (const utf8_t *) error_code, -1, &js_error_code);
  if (err < 0) {
    return err;
  }

  err = js_create_error(env, js_error_code, js_error_message, error);
  if (err < 0) {
    return err;
  }

  return 0;
}