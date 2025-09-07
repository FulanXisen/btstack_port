add_library(a2dp STATIC a2dp_sink.c)

target_include_directories(a2dp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    )
target_link_libraries(a2dp btstack)
