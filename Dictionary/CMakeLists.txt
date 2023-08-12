project(Dictionary)

list(APPEND srcs
     Dictionary.cpp
     Db.cpp
     )

add_executable(Dictionary
               ${srcs}
               )

target_link_libraries(Dictionary
                      common
                      msq_ops
                      )
