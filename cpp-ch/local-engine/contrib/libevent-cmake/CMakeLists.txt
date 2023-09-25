set(LIBEVENT_ROOT_DIR "${GLUTEN_CONTRIB_DIR}/libevent")
set(LIBEVENT_BINARY_DIR "${GLUTEN_CONTRIB_BINARY_DIR}/libevent")

set(EVENT__LIBRARY_TYPE STATIC)
set(EVENT__HAVE_KQUEUE OFF)
add_subdirectory("${LIBEVENT_ROOT_DIR}" "${LIBEVENT_BINARY_DIR}")

target_compile_options(event_core_static PRIVATE
        -Wno-keyword-macro
        -Wno-reserved-macro-identifier
        -Wno-documentation
        -Wno-strict-prototypes
        -Wno-cast-align
        -Wno-implicit-function-declaration
        -Wno-unreachable-code-return
        -Wno-assign-enum
        -Wno-format-pedantic
        -Wno-shorten-64-to-32
        -Wno-extra-semi-stmt
        -Wno-covered-switch-default
        -Wno-unused-macros
        -Wno-array-parameter
        -Wno-cast-qual
        -Wno-shadow
        -Wno-unreachable-code
        -Wno-unreachable-code-fallthrough
)

add_library(ch_contrib::event ALIAS event_core_static)