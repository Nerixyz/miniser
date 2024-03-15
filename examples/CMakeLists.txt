function(add_example name)
    add_executable(${name} ${CMAKE_CURRENT_LIST_DIR}/${name}.cpp)
    set_target_properties(${name} PROPERTIES
        CXX_STANDARD 20
    )
    target_link_libraries(${name} PRIVATE miniser)
endfunction()

add_example(basic)
add_example(rename)
