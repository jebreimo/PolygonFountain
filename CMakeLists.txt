##****************************************************************************
## Copyright © 2019 Jan Erik Breimo. All rights reserved.
## Created by Jan Erik Breimo on 2019-12-30.
##
## This file is distributed under the BSD License.
## License text is included with the source distribution.
##****************************************************************************
cmake_minimum_required(VERSION 3.13)
project(PolygonFountain)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(tungsten
    GIT_REPOSITORY "https://github.com/jebreimo/Tungsten.git")
FetchContent_GetProperties(tungsten)
if (NOT tungsten_POPULATED)
    FetchContent_Populate(tungsten)
    add_subdirectory(${tungsten_SOURCE_DIR} ${tungsten_BINARY_DIR})
endif ()

list(APPEND CMAKE_MODULE_PATH ${tungsten_SOURCE_DIR}/tools/cmake)

include(TungstenTargetEmbedShaders)

add_executable(PolygonFountain
    src/PolygonFountain/PolygonFountain.cpp
    src/PolygonFountain/FadingShaderProgram.cpp
    src/PolygonFountain/FadingShaderProgram.hpp
    )

target_link_libraries(PolygonFountain
    PRIVATE
        Tungsten::Tungsten
    )

tungsten_target_embed_shaders(PolygonFountain
    FILES
        src/PolygonFountain/PolygonFountain-frag.glsl
        src/PolygonFountain/PolygonFountain-vert.glsl
    )
