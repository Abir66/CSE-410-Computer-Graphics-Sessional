#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <iostream>
#include <fstream>
#include "Vector3D.h"
#include "Matrix.h"

static unsigned long int g_seed = 1;
inline int get_random()
{
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

struct Triangle
{
    Vector3D a, b, c;
    int red, green, blue;
    Triangle() {}

    Triangle(Vector3D a, Vector3D b, Vector3D c)
    {
        this->a = a;
        this->b = b;
        this->c = c;
    }

    void set_random_color()
    {
        red = get_random() % 256;
        green = get_random() % 256;
        blue = get_random() % 256;
    }

    friend std::ostream &operator<<(std::ostream &os, Triangle &tri)
    {
        os << tri.a << tri.b << tri.c;
        return os;
    }

    friend Triangle operator*(Matrix &m, Triangle &tri)
    {
        Triangle res;
        res.a = m * tri.a;
        res.b = m * tri.b;
        res.c = m * tri.c;
        return res;
    }
};

#endif