#ifndef CYLINDER_H
#define CYLINDER_H

#ifdef __unix__
#include <GL/glut.h>
#elif defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <glut.h>
#endif

#include <iostream>
#include <math.h>
#include <vector>
#include "vector3D.h"

struct Cylinder
{
    double radius;
    int segments;
    double fullAngle;

    std::vector<Vector3D> points;

    Cylinder(double radius = 1, double fullAngle = 360, int segments = 50)
    {
        this->radius = radius;
        this->segments = segments;
        this->fullAngle = fullAngle * M_PI / 180.0;
        init();
    }

    void init()
    {
        points.clear();

        for (int i = 0; i < segments + 1; i++)
        {
            double angle = -fullAngle / 2 + fullAngle * i / segments;
            points.push_back(Vector3D(radius * cos(angle), radius * sin(angle), 0));
        }
    }

    void draw()
    {
        glBegin(GL_QUADS);
        {
            for (int i = 0; i < segments; i++)
            {
                glVertex3d(points[i].x, points[i].y, 0.5);
                glVertex3d(points[i + 1].x, points[i + 1].y, 0.5);
                glVertex3d(points[i + 1].x, points[i + 1].y, -0.5);
                glVertex3d(points[i].x, points[i].y, -0.5);
            }
        }
        glEnd();
    }
};

#endif