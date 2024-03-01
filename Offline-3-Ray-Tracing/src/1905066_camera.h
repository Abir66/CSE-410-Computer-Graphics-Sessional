#ifndef CAMERA
#define CAMERA

#ifdef __unix__
#include <GL/glut.h>
#elif defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <glut.h>
#endif

#include <iostream>
#include <math.h>
#include <chrono>
#include "1905066_vector3D.h"

const double PI = acos(-1.0);
const double deg2rad = PI / 180.0;

struct Camera
{
    Vector3D pos = Vector3D(0, 0, 3);
    Vector3D u = Vector3D(0, 1, 0);
    Vector3D r = Vector3D(1, 0, 0);
    Vector3D l = Vector3D(0, 0, -1);

    double speed = 3;
    double angluarSpeed = 3;
    double look_length = 5;

    Camera()
    {
        u.normalize();
        r.normalize();
        l.normalize();
    }

    Camera(Vector3D pos, Vector3D target, double speed = 5, double angluarSpeed = 1)
    {
        this->pos = pos;
        this->l = target - pos;
        this->l.normalize();
        this->u = Vector3D(0, 0, 1);
        this->r = l * u;
        this->r.normalize();
        this->u = r * l;
        this->u.normalize();

        this->look_length = (pos - target).value();

        this->speed = speed;
        this->angluarSpeed = angluarSpeed;
    }

    inline void lookAt()
    {
        gluLookAt(pos.x, pos.y, pos.z,
                  pos.x + l.x * look_length, pos.y + l.y * look_length, pos.z + l.z * look_length,
                  u.x, u.y, u.z);
    }

    void keyListener(int key, int x, int y)
    {
        Vector3D temp;


        // use delta time to make the movement smooth

        switch (key)
        {
        case '1':
            r.rotate(u, angluarSpeed * deg2rad);
            l.rotate(u, angluarSpeed * deg2rad);
            break;
        case '2':
            r.rotate(u, -angluarSpeed * deg2rad);
            l.rotate(u, -angluarSpeed * deg2rad);
            break;

        case '3':
            l.rotate(r, angluarSpeed * deg2rad);
            u.rotate(r, angluarSpeed * deg2rad);
            break;
        case '4':
            l.rotate(r, -angluarSpeed * deg2rad);
            u.rotate(r, -angluarSpeed * deg2rad);
            break;
        case '5':
            u.rotate(l, angluarSpeed * deg2rad);
            r.rotate(l, angluarSpeed * deg2rad);
            break;
        case '6':
            u.rotate(l, -angluarSpeed * deg2rad);
            r.rotate(l, -angluarSpeed * deg2rad);
            break;

        // move up and down with keeping the reference point
        case 'w':
            pos.z = pos.z + 0.25;
            l.z = l.z - 0.25 / look_length;
            l.normalize();
            r =  l * u;
            r.normalize();
            u = r * l;
            u.normalize();
            break;

        case 's':
            pos.z = pos.z - 0.25;
            l.z = l.z + 0.25 / look_length;
            l.normalize();
            r =  l * u;
            r.normalize();
            u = r * l;
            u.normalize();

            break;

        default:
            break;
        }
    }

    void specialKeyListener(int key, int x, int y)
    {

        switch (key)
        {
        case GLUT_KEY_UP:
            pos = pos + l * speed;
            break;
        case GLUT_KEY_DOWN:
            pos = pos - l * speed;
            break;

        case GLUT_KEY_RIGHT:
            pos = pos + r * speed;
            break;
        case GLUT_KEY_LEFT:
            pos = pos - r * speed;
            break;

        case GLUT_KEY_PAGE_UP:
            pos = pos + u * speed;
            break;
        case GLUT_KEY_PAGE_DOWN:
            pos = pos - u * speed;
            break;

        default:
            break;
        }
    }

    // zoom in/out
    void mouseListener(int button, int state, int x, int y)
    {
        if (button == 3)
            pos = pos + l * speed;
        else if (button == 4)
            pos = pos - l * speed;
    }


    friend std::ostream &operator<<(std::ostream &os, Camera &c)
    {
        os << "pos: " << c.pos << "\nu: " << c.u << "\nr: " << c.r << "\nl: " << c.l;
        return os;
    }
};

#endif