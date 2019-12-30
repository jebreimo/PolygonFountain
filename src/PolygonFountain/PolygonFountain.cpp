//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 26.04.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <cmath>
#include <deque>
#include <iostream>
#include <vector>
#include <Tungsten/Tungsten.hpp>
#include <Tungsten/Shapes.hpp>
#include "FadingShaderProgram.hpp"

constexpr float EXPANSION_RATE = 0.04;
constexpr float SPIN = 0.01;
constexpr float MAX_SCALE = 7.0f;
constexpr unsigned N_STEPS_PER_GENERATION = 10;
constexpr unsigned N_GENERATIONS_PER_SHAPE = 10;
constexpr unsigned N_SHAPE_TYPES = 15;
constexpr float SPEED = 0.01f;

class Object
{
public:
    Object(const Xyz::Vector2f& position,
           const Xyz::Vector2f& speed,
           unsigned shapeType)
        : m_Position(position), m_Speed(speed),
          m_Angle(0.0f), m_Scale(1.0f),
          m_ShapeType(shapeType)
    {}

    bool isDead() const
    {
        return m_Scale > MAX_SCALE;
    }

    void updateLocation()
    {
        m_Position += m_Speed;
        m_Angle += SPIN;
        m_Scale += EXPANSION_RATE;
    }

    Xyz::Matrix4f matrix()
    {
        return Xyz::translate4(getX(m_Position), getY(m_Position), 0.0f)
               * Xyz::rotateZ(m_Angle)
               * Xyz::scale4(m_Scale, m_Scale, 1.0f);
    }

    unsigned shapeType() const
    {
        return m_ShapeType;
    }
private:
    Xyz::Vector2f m_Position;
    Xyz::Vector2f m_Speed;
    float m_Angle;
    float m_Scale;
    unsigned m_ShapeType;
};

Xyz::Vector2f makeSpeedVector(float angle, float length)
{
    return Xyz::makeVector2(std::cos(angle) * length,
                            std::sin(angle) * length);
}

void addObjects(std::deque<Object>& objects, float angle, unsigned shapeType)
{
    auto center = 0.1f * Xyz::makeVector2(std::cos(4.0f * angle),
                                          std::sin(4.0f * angle));
    for (int i = 0; i < 4; ++i)
    {
        objects.emplace_back(
                center,
                makeSpeedVector(angle + i * Xyz::PI_32 / 2, SPEED),
                shapeType);
    }
    objects.emplace_back(center,
                         makeSpeedVector(angle * 4.0f, 1.5f * SPEED),
                         shapeType);
}

struct ElementArrayEntry
{
    GLsizei startIndex;
    GLsizei count;
};

class ExpandingPolygonsApplication : public Tungsten::SdlApplication
{
public:
    void setup() override
    {
        Tungsten::ArrayBufferBuilder builder(3);
        for (unsigned i = 3; i < N_SHAPE_TYPES + 3; ++i)
        {
            auto count0 = GLsizei(builder.elementCount());
            Tungsten::addHollowRegularPolygon(builder, i, 0.01, 0.02);
            auto count1 = GLsizei(builder.elementCount());
            m_ElementCounts.push_back(ElementArrayEntry{count0, count1 - count0});
        }
        m_VertexArray = Tungsten::generateVertexArray();
        Tungsten::bindVertexArray(m_VertexArray);

        m_Buffers = Tungsten::generateBuffers(2);
        Tungsten::setBuffers(m_Buffers[0], m_Buffers[1], builder);

        m_Program.setup();
        Tungsten::defineVertexAttributePointer(m_Program.positionAttribute, 3,
                                               GL_FLOAT, false, 0, 0);
        Tungsten::enableVertexAttribute(m_Program.positionAttribute);

        addObjects(m_Objects, m_Angle, m_ShapeType);
        m_Program.minColor.set(Xyz::makeVector4<float>(0.0, 0.0, 0.0, 1.0));
        m_Program.maxColor.set(Xyz::makeVector4<float>(1.0, 1.0, 1.0, 1.0));
    }

    void update() override
    {
        for (auto& object : m_Objects)
            object.updateLocation();
        while (!m_Objects.empty() && m_Objects.front().isDead())
            m_Objects.pop_front();
        if (m_Step++ == N_STEPS_PER_GENERATION)
        {
            m_Step = 0;
            addObjects(m_Objects, m_Angle, m_ShapeType);
            if (m_Generations++ == N_GENERATIONS_PER_SHAPE)
            {
                m_ShapeType = (m_ShapeType + 1) % N_SHAPE_TYPES;
                m_Generations = 0;
            }
        }
        m_Angle += SPIN;
        if (m_Angle > 2 * Xyz::PI_32)
            m_Angle -= 2 * Xyz::PI_32;
    }

    void draw() override
    {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto projection = Xyz::scale4<float>(1.0f, aspectRatio(), 1.0f)
                          * Xyz::makeFrustumMatrix<float>(-1, 1, -1, 1, 1, 5)
                          * Xyz::makeLookAtMatrix(Xyz::makeVector3<float>(0, 0, 1),
                                                  Xyz::makeVector3<float>(0, 0, 0),
                                                  Xyz::makeVector3<float>(0, 1, 0));

        m_Program.projectionMatrix.set(projection);
        unsigned shapeType = UINT_MAX;
        GLsizei offset = 0;
        GLsizei count = 0;
        for (auto& object : m_Objects)
        {
            if (shapeType != object.shapeType())
            {
                shapeType = object.shapeType();
                offset = m_ElementCounts[shapeType].startIndex;
                count = m_ElementCounts[shapeType].count;
            }
            m_Program.modelMatrix.set(object.matrix());
            Tungsten::drawElements(GL_TRIANGLES, count,
                                   GL_UNSIGNED_SHORT, offset);
        }
    }

private:
    std::vector<Tungsten::BufferHandle> m_Buffers;
    Tungsten::VertexArrayHandle m_VertexArray;
    FadingShaderProgram m_Program;
    std::vector<ElementArrayEntry> m_ElementCounts;
    std::deque<Object> m_Objects;
    float m_Angle = 0;
    unsigned m_Step = 0;
    unsigned m_ShapeType = 0;
    unsigned m_Generations = 0;
};

int main()
{
    auto app = ExpandingPolygonsApplication{};
    try
    {
        app.run();
    }
    catch (Tungsten::GlError& ex)
    {
        std::cout << ex.what() << "\n";
        return 1;
    }

    return 0;
}
