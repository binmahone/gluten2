set(XSIMD_ROOT_DIR "${GLUTEN_CONTRIB_DIR}/xsimd")
# set(XSIMD_BINARY_DIR "${GLUTEN_CONTRIB_BINARY_DIR}/xsimd")

set(XSIMD_INCLUDE_DIR ${XSIMD_ROOT_DIR}/include)
file(STRINGS "${XSIMD_INCLUDE_DIR}/xsimd/config/xsimd_config.hpp" xsimd_version_defines
        REGEX "#define XSIMD_VERSION_(MAJOR|MINOR|PATCH)")
foreach(ver ${xsimd_version_defines})
    if(ver MATCHES "#define XSIMD_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XSIMD_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif()
endforeach()
set(xsimd_VERSION ${XSIMD_VERSION_MAJOR}.${XSIMD_VERSION_MINOR}.${XSIMD_VERSION_PATCH})
message(STATUS "xsimd v${xsimd_VERSION}")

set(XSIMD_HEADERS
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_constants.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_fma3_avx.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_fma3_avx2.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_fma3_sse.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_fma4.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_generic.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_isa.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_neon.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_neon64.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_scalar.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_sse2.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_sse3.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_sse4_1.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_sse4_2.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_ssse3.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/arch/xsimd_sve.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/config/xsimd_arch.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/config/xsimd_config.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/config/xsimd_cpuid.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/memory/xsimd_aligned_allocator.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/memory/xsimd_alignment.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_all_registers.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_api.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_neon_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_neon64_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_avx2_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_avx512f_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_avx_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_batch.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_batch_constant.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_fma3_avx_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_fma3_avx2_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_fma3_sse_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_fma4_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_generic_arch.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_sse2_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_sse3_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_sse4_1_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_sse4_2_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_ssse3_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_sve_register.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_traits.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/types/xsimd_utils.hpp
        ${XSIMD_INCLUDE_DIR}/xsimd/xsimd.hpp
)

add_library(_xsimd INTERFACE)
target_include_directories(_xsimd INTERFACE $<BUILD_INTERFACE:${XSIMD_INCLUDE_DIR}>)


add_library(ch_contrib::xsimd ALIAS _xsimd)