#include "tcp_cat_js.h"
#include "errors.h"

#include <assert.h>
#include <bare.h>
#include <js.h>

static js_value_t *
bare_addon_tcp_cat(js_env_t *env, js_callback_info_t *info) {
  int err;

  js_deferred_t *deferred;
  js_value_t *promise;


  err = js_create_promise(env, &deferred, &promise);
  if (err < 0) {
    js_throw_error(env, ERROR_CREATING_PROMISE, "Could not create promise for tcpCat");
    return NULL;
  }
  tcp_cat_execute(env, info, deferred);

  return promise;
}

static js_value_t *
bare_addon_exports(js_env_t *env, js_value_t *exports) {
  int err;

#define V(name, fn) \
  { \
    js_value_t *val; \
    err = js_create_function(env, name, -1, fn, NULL, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, exports, name, val); \
    assert(err == 0); \
  }

  V("tcpCat", bare_addon_tcp_cat)
#undef V

  return exports;
}

BARE_MODULE(bare_addon, bare_addon_exports)
