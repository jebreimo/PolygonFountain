//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2017-04-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Tungsten/Tungsten.hpp"
#include "Tungsten/Uniform.hpp"

class FadingShaderProgram
{
public:
    void setup();

    Tungsten::ProgramHandle program;

    Tungsten::Uniform<Xyz::Matrix4f> modelMatrix;
    Tungsten::Uniform<Xyz::Matrix4f> projectionMatrix;
    Tungsten::Uniform<Xyz::Vector4f> minColor;
    Tungsten::Uniform<Xyz::Vector4f> maxColor;

    GLuint positionAttribute;
};
