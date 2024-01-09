#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

struct Vector3D
{
    double x,y,z,w;
    Vector3D(double x = 0, double y = 0, double z = 0, double w = 1) : x(x), y(y), z(z), w(w) {}
    Vector3D(const Vector3D &p) : x(p.x), y(p.y), z(p.z), w(p.w) {}

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

    void reset_w()
    {
        x /= w;
        y /= w;
        z /= w;
        w = 1;
    }

    double value()
    {
        return sqrt(x * x + y * y + z * z);
    }

    
    friend std::ostream& operator<<(std::ostream& os, Vector3D& vec)
    {
        // set precision to 7 decimal places
        os << std::fixed << std::setprecision(7);   
        os << vec.x << " " << vec.y << " " << vec.z << std::endl;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Vector3D& vec)
    {
        is >> vec.x >> vec.y >> vec.z;
        vec.w = 1;
        return is;
    }

};



#endif 