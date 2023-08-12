project(Menu)

list(APPEND srcs
     Menu.cpp
     cli.cpp
     )

add_executable(Menu
               ${srcs}
               )
# target_include_directories(Menu INTERFACE
#                            ${CMAKE_CURRENT_SOURCE_DIR}
#                            )

target_link_libraries(Menu
                      common
                      msq_ops
                      )
