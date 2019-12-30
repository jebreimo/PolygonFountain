//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2017-04-29.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#version 100

attribute vec3 a_Position;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ProjectionMatrix;

uniform vec4 u_MinColor;
uniform vec4 u_MaxColor;

varying highp vec4 v_Color;

void main()
{
    vec4 realPosition = u_ProjectionMatrix * u_ModelMatrix * vec4(a_Position, 1.0);
    gl_Position = realPosition;
    v_Color = u_MinColor + clamp(distance(realPosition, vec4(0, 0, 0, 1)) / 1.0, 0.0, 1.0) * (u_MaxColor - u_MinColor);
}
