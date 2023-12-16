#ifndef SPHERE_H
#define SPHERE_H

#ifdef __unix__
#include <GL/glut.h>
#elif defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <glut.h>
#endif

#include <iostream>
#include <math.h>
#include <vector>

struct Sphere
{
    double radius;
    int sectorCount, stackCount;

    double color[2][3] = {
        {1, 0, 0},
        {0, 1, 0}};

    std::vector<float> vertices;
    std::vector<int> indices;

    Sphere(double r, int sectorCount, int stackCount)
    {
        this->radius = r;
        this->sectorCount = sectorCount;
        this->stackCount = stackCount;
        init();
    }

    void init()
    {
        // clear memory of prev arrays
        vertices.clear();
        indices.clear();

        float x, y, z, xy;                           // vertex position
        float nx, ny, nz, lengthInv = 1.0f / radius; // vertex normal
        float s, t;                                  // vertex texCoord

        float sectorStep = 2 * PI / sectorCount;
        float stackStep = PI / stackCount;
        float sectorAngle, stackAngle;

        for (int i = 0; i <= stackCount; ++i)
        {
            stackAngle = PI / 2 - i * stackStep; // starting from pi/2 to -pi/2

            xy = radius * cosf(stackAngle); // r * cos(u)
            z = radius * sinf(stackAngle);  // r * sin(u)

            for (int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep; // starting from 0 to 2pi

                // vertex position (x, y, z)
                x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
                y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }

        int k1, k2;
        for (int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1); // beginning of current stack
            k2 = k1 + sectorCount + 1;  // beginning of next stack

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if (i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                // k1+1 => k2 => k2+1
                if (i != (stackCount - 1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }
    }

    void draw()
    {
        int numVertices = indices.size();
        int numTriangles = numVertices / 3;

        // set the color
        glColor3f(color[0][0], color[0][1], color[0][2]);
        int cnt = 0, cnt2 = 0;
        int colorIndex = 0;

        for (int i = 0; i < numTriangles; i++, cnt++, cnt2++)
        {
            int index1 = indices[i * 3];
            int index2 = indices[i * 3 + 1];
            int index3 = indices[i * 3 + 2];

            if (cnt2 == sectorCount * stackCount)
            {
                colorIndex = (colorIndex + 1) % 2;
                glColor3f(color[colorIndex][0], color[colorIndex][1], color[colorIndex][2]);
            }

            if (cnt % 8 == 0)
            {
                cnt = 0;
                colorIndex = (colorIndex + 1) % 2;
                glColor3f(color[colorIndex][0], color[colorIndex][1], color[colorIndex][2]);
            }

            glBegin(GL_TRIANGLES);
            {
                glVertex3f(vertices[index1 * 3], vertices[index1 * 3 + 1], vertices[index1 * 3 + 2]);
                glVertex3f(vertices[index2 * 3], vertices[index2 * 3 + 1], vertices[index2 * 3 + 2]);
                glVertex3f(vertices[index3 * 3], vertices[index3 * 3 + 1], vertices[index3 * 3 + 2]);
            }
            glEnd();
        }
    }

    void rotate(double angle, Vector3D axis)
    {
        angle = angle * M_PI / 180.0;
        axis.normalize();
        double s = sin(angle);
        double c = cos(angle);
        double oneMinusC = 1 - c;

        // Apply rotation to all vertices of the sphere
        for (size_t i = 0; i < vertices.size() / 3; ++i)
        {

            Vector3D point(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);

            // Rodrigues' rotation formula
            // https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula

            Vector3D rotated = point * c + (axis * point) * s + axis * Vector3D::dot(axis, point) * oneMinusC;

            vertices[i * 3] = rotated.x;
            vertices[i * 3 + 1] = rotated.y;
            vertices[i * 3 + 2] = rotated.z;
        }
    }
};

#endif