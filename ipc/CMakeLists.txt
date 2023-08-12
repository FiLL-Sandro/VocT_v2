project(msq_ops)

list(APPEND srcs
     msq_ops.c
     )

add_library(msq_ops SHARED
            ${srcs}
            )
target_include_directories(msq_ops INTERFACE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           )
