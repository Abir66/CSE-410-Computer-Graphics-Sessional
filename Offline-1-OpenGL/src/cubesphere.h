#ifndef CUBESPHERE_H
#define CUBESPHERE_H

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

struct Cubesphere
{
    double radius;
    int pointsPerRow;

    // 2d vector of vertices
    std::vector<std::vector<Vector3D>> vertices;

    Cubesphere(double r, int pointsPerRow)
    {
        this->radius = r;
        this->pointsPerRow = pointsPerRow;
        init();
    }

    void init()
    {
        vertices.clear();
        vertices.resize(pointsPerRow, std::vector<Vector3D>(pointsPerRow));

        const float DEG2RAD = acos(-1) / 180.0f;

        float n1[3]; // normal of longitudinal plane rotating along Y-axis
        float n2[3]; // normal of latitudinal plane rotating along Z-axis
        float v[3];  // direction vector intersecting 2 planes, n1 x n2
        float a1;    // longitudinal angle along Y-axis
        float a2;    // latitudinal angle along Z-axis

        // rotate latitudinal plane from 45 to -45 degrees along Z-axis (top-to-bottom)
        for (int i = 0; i < pointsPerRow; ++i)
        {
            // normal for latitudinal plane
            // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
            // therefore, it is rotating (0,1,0) vector by latitude angle a2
            a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
            Vector3D n2(-sin(a2), cos(a2), 0);

            // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-right)
            for (int j = 0; j < pointsPerRow; ++j)
            {
                // normal for longitudinal plane
                // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
                // therefore, it is rotating (0,0,-1) vector by longitude angle a1
                a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));

                Vector3D n1(-sin(a1), 0, -cos(a1));

                // find direction vector of intersected line, n1 x n2
                Vector3D v = n1 * n2;
                v.normalize();
                v = v * radius;
                vertices[i][j] = Vector3D(v.x, v.y, v.z);
            }
        }
    }

    void draw()
    {
        
        glBegin(GL_QUADS);
        for (int i = 0; i < pointsPerRow - 1; ++i)
        {
            for (int j = 0; j < pointsPerRow - 1; ++j)
            {
                // draw quads
                glVertex3d(vertices[i][j].x, vertices[i][j].y, vertices[i][j].z);
                glVertex3d(vertices[i][j + 1].x, vertices[i][j + 1].y, vertices[i][j + 1].z);
                glVertex3d(vertices[i + 1][j + 1].x, vertices[i + 1][j + 1].y, vertices[i + 1][j + 1].z);
                glVertex3d(vertices[i + 1][j].x, vertices[i + 1][j].y, vertices[i + 1][j].z);
            }
        }
        glEnd();
    }
};

#endif