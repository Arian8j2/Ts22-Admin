cmake_minimum_required(VERSION 3.10.2)
project(Ts22_Admin)

option(TS3_INSTALLER "create teamspeak 3 plugin installer" ON)

add_library(${PROJECT_NAME} SHARED
    src/plugin.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_options(${PROJECT_NAME} PUBLIC -c -O2 -Wall -fPIC)
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

set(RELEASE_PATH ${CMAKE_BINARY_DIR}/Release)
function(copy SOURCE DESTINATION CMD)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E ${CMD}
        ${SOURCE} ${RELEASE_PATH}/${DESTINATION}
    )
endfunction()

set(LIB_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
copy(${CMAKE_SOURCE_DIR}/icons ${PROJECT_NAME} copy_directory)
copy(${CMAKE_BINARY_DIR}/${PROJECT_NAME}${LIB_SUFFIX} ${PROJECT_NAME}${LIB_SUFFIX} copy)

if(TS3_INSTALLER)
    copy(${CMAKE_SOURCE_DIR}/package.ini . copy)
    copy(${CMAKE_SOURCE_DIR}/icons plugins/${PROJECT_NAME} copy_directory)
    copy(${CMAKE_BINARY_DIR}/${PROJECT_NAME}${LIB_SUFFIX} plugins/${PROJECT_NAME}${LIB_SUFFIX} copy)


    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD COMMAND
        ${CMAKE_COMMAND} -E tar "cfv" "${PROJECT_NAME}.ts3_plugin" --format=zip "plugins" "package.ini"
        WORKING_DIRECTORY ${RELEASE_PATH}
        COMMENT "Creating teamspeak3 installer ..."
    )

    function(remove files command)
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E ${command} ${files}
            WORKING_DIRECTORY ${RELEASE_PATH}
        )
    endfunction()

    remove(plugins remove_directory)
    remove(package.ini remove)
endif()