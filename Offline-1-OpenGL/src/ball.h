#ifndef BALL_H
#define BALL_H

#ifdef __unix__
#include <GL/glut.h>
#elif defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <glut.h>
#endif

#include <iostream>
#include <math.h>
#include <vector>
#include "sphere.h"
#include "vector3D.h"

struct Ball
{
    double radius = 1;
    Sphere sphere = Sphere(radius, 40, 40);
    Vector3D pos = Vector3D(0, 0, radius);
    double speed = 0.1;
    double mannualSpeed = 0.5;
    Vector3D direction = Vector3D(1, 1, 0);
    bool isMoving = true;
    bool isAuto = false;
    double angularSpeed = 5;
    double angle = 10;

    GLUquadricObj *quadratic;

    Ball(double radius = 1, double speed = 0.1, Vector3D pos = Vector3D(0, 0, 1), Vector3D direction = Vector3D(1, 1, 0))
    {
        this->radius = radius;
        this->pos = pos;
        this->speed = speed;
        this->direction = direction;
        this->direction.normalize();
        quadratic = gluNewQuadric();
    }

    ~Ball()
    {
        gluDeleteQuadric(quadratic);
    }

    void rotateClockwise()
    {
        Vector3D axis = Vector3D(0, 0, 1);
        direction.rotate(axis, -angularSpeed * M_PI / 180);
        direction.normalize();
    }

    void rotateCounterClockwise()
    {
        Vector3D axis = Vector3D(0, 0, 1);
        direction.rotate(axis, angularSpeed * M_PI / 180);
        direction.normalize();
    }

    void rotate(double angle)
    {
        Vector3D axis = Vector3D(0, 0, 1) * direction;
        sphere.rotate(angle, axis);
    }

    // void reflectDirection(Vector3D planeNormal)
    // {
    //     direction = direction - planeNormal * 2 * Vector3D::dot(planeNormal, direction);
    //     direction.normalize();
    // }

    void move(double deltaTime)
    {
        if (!isAuto)
            return;
        double realSpeed = speed * deltaTime;
        pos = pos + direction * realSpeed;
        double angle = 360 * realSpeed / (2 * M_PI * radius);
        rotate(angle);
    }

    void moveMannual(bool isForward = true){
        int sign = isForward ? 1 : -1;
        pos = pos + direction * mannualSpeed * sign;
        double angle = 360 * mannualSpeed  / (2 * M_PI * radius);
        rotate(angle * sign);
    }

    void drawArrow()
    {
        glPushMatrix();
        {
            gluCylinder(quadratic, 0.1f, 0.1f, 3.0f, 32, 32);
            glTranslated(0.0f, 0.0f, 3.0f);
            gluCylinder(quadratic, 0.3f, 0.0f, 1.0f, 32, 32);
        }
        glPopMatrix();
    }

    void draw()
    {
        glPushMatrix();
        {
            glTranslatef(pos.x, pos.y, pos.z);
            sphere.draw();
        }
        glPopMatrix();

        glColor3f(0.0f, 0.0f, 1.0f);
        glPushMatrix();
        {
            glTranslatef(pos.x, pos.y, pos.z);
            double angle_y = atan2(direction.y, direction.x) * 180.0 / M_PI + 90;
            glRotatef(angle_y, 0, 0, 1);
            glRotatef(90, 1, 0, 0);
            drawArrow();
        }
        glPopMatrix();

        // cyan color
        glColor3f(0.0f, 1.0f, 1.0f);
        glPushMatrix();
        {
            glTranslatef(pos.x, pos.y, pos.z);
            glScaled(1.0f, 1.0f, 1.3f);
            drawArrow();
        }
        glPopMatrix();
    }
};
#endif