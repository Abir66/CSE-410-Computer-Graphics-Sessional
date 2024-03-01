#ifndef VECTOR3D
#define VECTOR3D

#ifdef __unix__
#include <GL/glut.h>
#elif defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <glut.h>
#endif

#include <iostream>
#include <math.h>

struct Vector3D
{
    double x, y, z;
    Vector3D() {}
    Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}
    Vector3D(const Vector3D &p) : x(p.x), y(p.y), z(p.z) {}

    Vector3D operator+(Vector3D vec) { return Vector3D(x + vec.x, y + vec.y, z + vec.z); }
    Vector3D operator-(Vector3D vec) { return Vector3D(x - vec.x, y - vec.y, z - vec.z); }
    Vector3D operator*(Vector3D vec) { return Vector3D(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x); }
    Vector3D operator*(double a) { return Vector3D(x * a, y * a, z * a); }
    Vector3D operator/(double a) { return Vector3D(x / a, y / a, z / a); }
    friend Vector3D operator*(double a, Vector3D vec) { return Vector3D(a * vec.x, a * vec.y, a * vec.z); }
    friend Vector3D operator/(double a, Vector3D vec) { return Vector3D(a / vec.x, a / vec.y, a / vec.z); }

    static double dot(Vector3D a, Vector3D b) { return a.x * b.x + a.y * b.y + a.z * b.z; }


    void rotate(Vector3D axis, double angle){
        axis.normalize();
        Vector3D v = *this;
        *this = v * cos(angle) + axis * Vector3D::dot(axis, v) * (1 - cos(angle)) + (axis * v) * sin(angle);

    }

    void normalize()
    {
        double length = sqrt(x * x + y * y + z * z);
        x /= length;
        y /= length;
        z /= length;
    }

    Vector3D normal(){
        Vector3D v = *this;
        v.normalize();
        return v;
    }

    double value()
    {
        return sqrt(x * x + y * y + z * z);
    }



    Vector3D operator-() { return Vector3D(-x, -y, -z); }

    
    friend std::istream& operator>>(std::istream& is, Vector3D& v){
        is >> v.x >> v.y >> v.z;
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, Vector3D& v){
        os << v.x << " " << v.y << " " << v.z << " " << v.value();
        return os;
    }

    static Vector3D reflect2D(Vector3D v, Vector3D normal)
    {
        v.z = 0;
        normal.z = 0;
        Vector3D reflected = v - normal * 2 * Vector3D::dot(normal, v);
        return reflected;
    }

    static Vector3D reflect3D(Vector3D v, Vector3D normal)
    {
        Vector3D reflected = v - normal * 2 * Vector3D::dot(normal, v);
        return reflected;
    }
};

#endif