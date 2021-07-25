cmake_minimum_required(VERSION 3.2)
project(Ts22_Admin)

add_library(Ts22_Admin SHARED
    src/plugin.cpp
)

target_include_directories(Ts22_Admin PUBLIC include)
target_compile_options(Ts22_Admin PUBLIC -c -O2 -Wall -fPIC)
set_target_properties(Ts22_Admin PROPERTIES PREFIX "")

set(RELEASE_PATH ${CMAKE_BINARY_DIR}/Release)
function(copy SOURCE DESTINATION CMD)
    add_custom_command(TARGET Ts22_Admin POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E ${CMD}
        ${SOURCE} ${RELEASE_PATH}/${DESTINATION}
    )
endfunction()

copy(${CMAKE_SOURCE_DIR}/icons Ts22_Admin copy_directory)
copy(${CMAKE_BINARY_DIR}/Ts22_Admin.dll Ts22_Admin.dll copy)


