add_library(hci STATIC hci.c)
target_include_directories(hci PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    )
target_link_libraries(hci btstack)
