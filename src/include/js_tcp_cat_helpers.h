#ifndef BARE_ADDON_JS_TCP_CAT_HELPERS_H
#define BARE_ADDON_JS_TCP_CAT_HELPERS_H

#include "tcp_cat_uv.h"

#include <js.h>

typedef enum {
  TCP_CAT_PARSE_ARGUMENTS_SUCCESS,
  TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_COUNT = -1,
  TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_TYPE = -2,
  TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_ARGUMENT_TYPE_FIRST = -3,
  TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_ARGUMENT_TYPE_SECOND = -4,
  TCP_CAT_PARSE_ARGUMENTS_ERROR_WRONG_ARGUMENT_TYPE_THIRD = -5,
  TCP_CAT_PARSE_ARGUMENTS_ERROR_PORT_OUT_OF_RANGE = -6,
  TCP_CAT_PARSE_ARGUMENTS_ERROR_NO_MEMORY = -7,
  TCP_CAT_PARSE_ARGUMENTS_ERROR_CANNOT_DETERMINE_TYPE = -8,
  TCP_CAT_PARSE_ARGUMENTS_ERROR_OTHER = -9,
  TCP_CAT_PARSE_ARGUMENTS_ERROR_UINT16_OUT_OF_RANGE = -10,
  TCP_CAT_PARSE_ARGUMENTS_ERROR_OUT_OF_MEMORY = -11
} tcp_cat_parse_arguments_error;

tcp_cat_parse_arguments_error
tcp_cat_parse_arguments(js_env_t *env, const js_callback_info_t *info, tcp_cat_arguments **arguments);

int
tcp_cat_response_to_js_arraybuffer(js_env_t *env, tcp_cat_response *response, js_value_t **result);

int
tcp_cat_js_create_error(js_env_t *env, const char *error_code, const char *error_message, js_value_t **error);

#endif // BARE_ADDON_JS_TCP_CAT_HELPERS_H
