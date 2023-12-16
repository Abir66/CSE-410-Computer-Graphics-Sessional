#ifndef WALL_H
#define WALL_H

#ifdef __unix__
#include <GL/glut.h>
#elif defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <glut.h>
#endif

#include <iostream>
#include <math.h>
#include "vector3D.h"

struct Wall
{

    double startX, startY, endX, endY;
    double height = 1;
    Vector3D normal;
    double a, b, c;

    // wall is defined by two points
    // wall is a plane with height 1
    Wall(double startX, double startY, double endX, double endY)
    {
        this->startX = startX;
        this->startY = startY;
        this->endX = endX;
        this->endY = endY;

        initNormal();
        initLineEquation();
    }

    void initNormal()
    {
        Vector3D v1(startX, startY, 0);
        Vector3D v2(endX, endY, 0);

        normal = Vector3D(startY - endY, endX - startX, 0);
        // if dot product of normal and v1 is negative, then normal is pointing inwards
        if (Vector3D::dot(normal, v1) > 0)
            normal = normal * -1;
        normal.normalize();
    }

    void initLineEquation()
    {
        if (endX - startX == 0)
        {
            a = 1;
            b = 0;
            c = -startX;
        }
        else
        {
            a = (endY - startY) / (endX - startX);
            b = -1;
            c = startY - a * startX;
        }
    }

    double getDistance(double x, double y)
    {
        // distance from point (x, y) to the wall
        // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
        return abs(a * x + b * y + c) / sqrt(a * a + b * b);
    }

    void draw()
    {
        glColor3d(1, 0, 0);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glPushMatrix();
        {
            glBegin(GL_QUADS);
            {
                glVertex3f(startX, startY, 0);
                glVertex3f(startX, startY, height);
                glVertex3f(endX, endY, height);
                glVertex3f(endX, endY, 0);
            }
            glEnd();
        }
        glPopMatrix();

        glColor3d(0.5, 0, 0);
        glCullFace(GL_FRONT);
        glPushMatrix();
        {
            glBegin(GL_QUADS);
            {
                glVertex3f(startX, startY, 0);
                glVertex3f(startX, startY, height);
                glVertex3f(endX, endY, height);
                glVertex3f(endX, endY, 0);
            }
            glEnd();
        }
        glPopMatrix();
        glDisable(GL_CULL_FACE);
    }

    void print()
    {
        std::cout << "(" << startX << ", " << startY << ") -> (" << endX << ", " << endY << ")\n";
        // print equation of the line
        std::cout << a << "x + " << b << "y + " << c << " = 0\n";
    }
};

#endif