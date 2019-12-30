//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2017-04-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "FadingShaderProgram.hpp"

#include "Tungsten/ShaderTools.hpp"
#include "PolygonFountain-vert.glsl.hpp"
#include "PolygonFountain-frag.glsl.hpp"

void FadingShaderProgram::setup()
{
    program = Tungsten::createProgram();
    auto vertexShader = Tungsten::createShader(GL_VERTEX_SHADER,
                                               PolygonFountain_vert);
    Tungsten::attachShader(program, vertexShader);

    auto fragmentShader = Tungsten::createShader(GL_FRAGMENT_SHADER,
                                                 PolygonFountain_frag);
    Tungsten::attachShader(program, fragmentShader);
    Tungsten::linkProgram(program);
    Tungsten::useProgram(program);

    positionAttribute = Tungsten::getVertexAttribute(program, "a_Position");

    modelMatrix = Tungsten::getUniform<Xyz::Matrix4f>(
            program, "u_ModelMatrix");
    projectionMatrix = Tungsten::getUniform<Xyz::Matrix4f>(
            program, "u_ProjectionMatrix");
    minColor = Tungsten::getUniform<Xyz::Vector4f>(program, "u_MinColor");
    maxColor = Tungsten::getUniform<Xyz::Vector4f>(program, "u_MaxColor");
}
