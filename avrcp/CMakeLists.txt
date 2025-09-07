add_library(avrcp STATIC avrcp.c)
target_include_directories(avrcp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    )
target_link_libraries(avrcp btstack)
