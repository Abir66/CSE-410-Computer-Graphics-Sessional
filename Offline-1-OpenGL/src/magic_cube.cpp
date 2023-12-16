#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>

#include <GL/glut.h>
#include "camera.h"
#include "cubesphere.h"
#include "vector3D.h"
#include "cylinder.h"
#include <vector>

// define colors
#define RED 1, 0, 0
#define GREEN 0, 1, 0
#define BLUE 0, 0, 1
#define YELLOW 1, 1, 0
#define CYAN 0, 1, 1
#define MAGENTA 1, 0, 1
#define WHITE 1, 1, 1
#define BLACK 0, 0, 0

#define CYLINDER_ANGLE 70.5287794

Camera camera(Vector3D(1.5, 1.5, 1.5), Vector3D(0, 0, 0));

double rotAngle = 0;
double percent = 1;
bool autoRotate = false;
Cubesphere sphere(1, 20);
Cylinder cylinder(1, CYLINDER_ANGLE, 100);

/* Initialize OpenGL Graphics */
void initGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClearDepth(1.0f);                   // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);              // Enable depth testing for z-culling
}

void drawSpheres()
{
    int colors[6][3] = {{RED}, {GREEN}, {BLUE}, {WHITE}, {CYAN}, {MAGENTA}};

    double radius = (1 - percent) / sqrt(3);
    int colorIndex = 0;

    for (int i = 0; i < 4; i++)
    {
        glColor3d(colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]);
        colorIndex++;

        glPushMatrix();
        {
            glRotatef(90 * i, 0, 1, 0);
            glTranslated(percent, 0, 0);
            glScaled(radius, radius, radius);
            sphere.draw();
        }
        glPopMatrix();
    }

    for (int i = 0; i < 2; i++)
    {
        glColor3d(colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]);
        colorIndex++;
        glPushMatrix();
        {
            glRotatef(90 + 180 * i, 0, 0, 1);
            glTranslated(percent, 0, 0);
            glScaled(radius, radius, radius);
            sphere.draw();
        }
        glPopMatrix();
    }
}

void drawAllCylinders()
{
    double radius = (1 - percent) / sqrt(3);

    glColor3d(YELLOW);

    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        {
            glRotated(i * 90, 0, 0, 1);
            glRotated(-45, 0, 1, 0);
            glTranslated(percent / sqrt(2), 0, 0);
            glScaled(radius, radius, sqrt(2) * percent);
            cylinder.draw();
        }
        glPopMatrix();
    }

    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        {
            glRotated(180, 1, 0, 0);
            glRotated(i * 90, 0, 0, 1);
            glRotated(-45, 0, 1, 0);
            glTranslated(percent / sqrt(2), 0, 0);
            glScaled(radius, radius, sqrt(2) * percent);
            cylinder.draw();
        }
        glPopMatrix();
    }

    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        {
            glRotated(45 + i * 90, 0, 0, 1);
            glTranslated(percent / sqrt(2), 0, 0);
            glRotated(90, 1, 0, 0);
            glScaled(radius, radius, sqrt(2) * percent);
            cylinder.draw();
        }
        glPopMatrix();
    }
}

void drawAxes()
{
    glLineWidth(3);
    glBegin(GL_LINES);
    {
        // x axis
        glColor3f(2, 0, 0);
        glVertex3f(-2, 0, 0);
        glVertex3f(2, 0, 0);

        // y axis
        glColor3f(0, 2, 0);
        glVertex3f(0, -2, 0);
        glVertex3f(0, 2, 0);

        // z axis
        glColor3f(0, 0, 2);
        glVertex3f(0, 0, -2);
        glVertex3f(0, 0, 2);
    }
    glEnd();
}

void drawTriangle()
{
    glBegin(GL_TRIANGLES);
    {
        glVertex3f(1, 0, 0);
        glVertex3f(0, 1, 0);
        glVertex3f(0, 0, 1);
    }
    glEnd();
}

void drawPyramid()
{
    double colors[2][3] = {
        {0, 1, 1},
        {1, 0, 1}};

    int colorIndex = 0;
    double t = (1 - percent) / sqrt(6.0);
    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glColor3d(colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]);
        glRotatef(i * 90, 0, 0, 1);
        glTranslated((1 - percent) / 3, (1 - percent) / 3, (1 - percent) / 3);
        glScaled(percent, percent, percent);
        drawTriangle();
        glPopMatrix();
        colorIndex = (colorIndex + 1) % 2;
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glClearColor(0, 0, 0, 0);                           // color black

    glMatrixMode(GL_MODELVIEW); // To operate on Model-View matrix
    glLoadIdentity();           // Reset the model-view matrix

    camera.lookAt();
    // look from z axis top down
    // gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

    glPushMatrix();
    {
        glRotated(rotAngle, 0, 0, 1);

        drawAxes();
        drawSpheres();
        drawPyramid();
        glPushMatrix();
        glRotated(180, 1, 0, 0);
        drawPyramid();
        glPopMatrix();

        drawAllCylinders();
    }
    glPopMatrix();

    glutSwapBuffers(); // Swap the front and back frame buffers (double buffering)
}

void specialKeyListener(int key, int x, int y)
{
    camera.specialKeyListener(key, x, y);
    glutPostRedisplay();
}

void keyboardListener(unsigned char key, int x, int y)
{
    camera.keyListener(key, x, y);
    glutPostRedisplay();

    if (key == '.')
    {
        percent = std::max(percent - 0.1, 0.0);
    }

    else if (key == ',')
    {
        percent = std::min(percent + 0.1, 1.0);
    }

    else if (key == 32)
    {
        autoRotate = !autoRotate;
    }

    else if (key == 'a')
    {
        rotAngle -= 3;
    }

    else if (key == 'd')
    {
        rotAngle += 3;
    }
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height)
{ // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0)
        height = 1; // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping volume to match the viewport
    glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
    glLoadIdentity();            // Reset
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(60.0f, aspect, 0.1f, 100.0f);
}

void idle()
{
    if (autoRotate)
        rotAngle += 3;
    glutPostRedisplay();
}

/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char **argv)
{
    glutInit(&argc, argv);                                    // Initialize GLUT
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color
    glutInitWindowSize(640, 480);                             // Set the window's initial width & height
    glutCreateWindow("title");                                // Create window with the given title
    glutDisplayFunc(display);                                 // Register callback handler for window re-paint event
    glutReshapeFunc(reshape);                                 // Register callback handler for window re-size event

    initGL(); // Our own OpenGL initialization
    glutSpecialFunc(specialKeyListener);
    glutKeyboardFunc(keyboardListener);
    glutIdleFunc(idle);
    glutMainLoop(); // Enter the infinite event-processing loop
    return 0;
}