#ifndef LIGHT
#define LIGHT

#ifdef __unix__
    #include <GL/glut.h>
#elif defined(_WIN32) || defined(WIN32)
    #include <windows.h>
    #include <glut.h>
#endif

#include <iostream>
#include "1905066_vector3D.h"
#include "1905066_color.h"



struct PointLight
{
    Vector3D light_pos;
    Color color;

    PointLight(){}
    PointLight(Vector3D light_pos, Color color) : light_pos(light_pos), color(color) {} 

    void draw()
    {
        glPushMatrix();
        glTranslatef(light_pos.x, light_pos.y, light_pos.z);
        color.apply();
        glutSolidSphere(1, 10, 10);
        glPopMatrix();
    }

    friend std::istream& operator>>(std::istream& is, PointLight& light){
        is >> light.light_pos >> light.color;
        return is;
    }
}; 


struct SpotLight{

    PointLight point_light;
    Vector3D light_direction;
    double cut_off_angle;

    SpotLight(){}
    SpotLight(PointLight point_light, Vector3D light_direction, double cut_off_angle) : point_light(point_light), light_direction(light_direction), cut_off_angle(cut_off_angle) {}

    void draw()
    {
        point_light.draw();
        // a line from light to direction
        glPushMatrix();{
            glTranslatef(point_light.light_pos.x, point_light.light_pos.y, point_light.light_pos.z);
            glLineWidth(2);
            // glColor3f(1 - point_light.color.r, 1 - point_light.color.g, 1 - point_light.color.b);
            glScaled(100,100,100);    
            glBegin(GL_LINES);
            glVertex3f(0, 0, 0);
            glVertex3f(light_direction.x, light_direction.y, light_direction.z);
            glEnd();
        }glPopMatrix();
    }

    friend std::istream& operator>>(std::istream& is, SpotLight& light){
        is >> light.point_light >> light.light_direction >> light.cut_off_angle;
        return is;
    }

};


#endif