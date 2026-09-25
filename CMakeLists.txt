cmake_minimum_required(VERSION 3.22)

set(EXTRA_COMPONENT_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/esp32
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(safe-and-integrated-IoT-solution)