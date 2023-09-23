set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma -mavx -mf16c -mlzcnt -mbmi2")
clickhouse_add_executable (xsimd_check example/xsimd.cpp)
target_compile_options(xsimd_check PRIVATE
        -Wunused-parameter
        -Wextra
        -Wreorder
        -Wno-undef
        -Wno-old-style-cast
        -Wno-reserved-identifier
        -Wno-zero-as-null-pointer-constant
        -Wno-documentation
        -Wno-deprecated
        -Wno-cast-align
        -Wno-shorten-64-to-32
)

target_link_libraries(xsimd_check PRIVATE ch_contrib::xsimd)