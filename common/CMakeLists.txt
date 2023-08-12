project(common)

list(APPEND srcs
     Log.cpp
     utility.cpp
     )

add_library(common SHARED
            ${srcs}
            )
target_include_directories(common INTERFACE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           )
target_link_directories(common INTERFACE
                        ${CMAKE_CURRENT_SOURCE_DIR}
                        )
target_link_libraries(common INTERFACE
                      ${CMAKE_CURRENT_SOURCE_DIR}/libcjson.so
                      )
target_link_options(common INTERFACE
                    -lcjson
                    -Wl,-rpath=${CMAKE_CURRENT_SOURCE_DIR}/../common
                    )
