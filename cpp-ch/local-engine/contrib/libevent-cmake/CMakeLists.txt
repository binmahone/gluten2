set(LIBEVENT_ROOT_DIR "${GLUTEN_CONTRIB_DIR}/libevent")
set(LIBEVENT_BINARY_DIR "${GLUTEN_CONTRIB_BINARY_DIR}/libevent")

set(EVENT__LIBRARY_TYPE STATIC CACHE STRING "Build libevent as static library")
set(EVENT__HAVE_KQUEUE OFF)
set(EVENT__HAVE_ARC4RANDOM 0)
set(EVENT__HAVE_STRLCPY 0)
set(EVENT__HAVE_ISSETUGID 0)
set(EVENT__HAVE_GETEUID 1)
set(EVENT__DISABLE_OPENSSL ON CACHE STRING "Disable OpenSSL support")
set(EVENT__DISABLE_BENCHMARK ON CACHE STRING "Disable benchmark")
set(EVENT__DISABLE_TESTS ON CACHE STRING "Disable tests")
set(EVENT__DISABLE_REGRESS ON CACHE STRING "Disable regress")
set(EVENT__DISABLE_SAMPLES ON CACHE STRING "Disable samples")
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