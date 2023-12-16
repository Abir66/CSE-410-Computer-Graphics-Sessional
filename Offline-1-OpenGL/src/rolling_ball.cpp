#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <chrono>

#include <GL/glut.h>
#include "camera.h"
#include "sphere.h"
#include "vector3D.h"
#include "ball.h"
#include "wall.h"

Camera camera(Vector3D(0, -20, 10), Vector3D(0, 0, 0));

// Ball ball(1, 0.01, Vector3D(10 - 5 * sqrt(3), 0, 1), Vector3D(5 * sqrt(3), 5, 0));
Ball ball(1, 0.04, Vector3D(0, 0, 1), Vector3D(1, 1, 0));

int collidingWallIndex = -1;
int simulateRestart = 0;

std::chrono::steady_clock::time_point prevTime;
std::chrono::steady_clock::time_point currentTime;

#define FORWARD true
#define BACKWARD false
#define CLOCKWISE true
#define COUNTER_CLOCKWISE false


std::vector<Wall> walls = {
    Wall(10, 5, 0, 10),
    Wall(0, 10, -10, 5),
    Wall(-10, 5, -10, -5),
    Wall(-10, -5, 0, -10),
    Wall(0, -10, 10, -5),
    Wall(10, -5, 10, 5),
};

void checkCollision(bool isForward = true, double epsilon = 0.00)
{
    int sign = isForward ? 1 : -1;
    std::vector<int> collidedWalls;

    for (int i = 0; i < walls.size(); i++)
    {
        double distance = walls[i].getDistance(ball.pos.x, ball.pos.y);
        if (distance <= ball.radius + epsilon)
        {
            Vector3D v1(ball.direction.x * sign, ball.direction.y * sign, 0);
            Vector3D v2(walls[i].normal.x, walls[i].normal.y, 0);
            if (Vector3D::dot(v1,v2) > 0)
                continue;

            collidedWalls.push_back(i);
            // ball.direction = Vector3D::reflect2D(ball.direction, walls[i].normal);
        }
    }

    if (collidedWalls.size() == 0)
        return;

    Vector3D v(0, 0, 0);
    for (auto i : collidedWalls)
    {
        v = v + Vector3D::reflect2D(ball.direction, walls[i].normal);
    }
    v.normalize();
    ball.direction = v;
}

void scheduleEvent();

void handleEvent(int x)
{
    if (ball.isAuto == false || simulateRestart != x)
        return;
    // ball.reflectDirection(walls[collidingWallIndex].normal);
    checkCollision(true, 0.05);
    scheduleEvent();
}

void scheduleEvent()
{
    if (ball.isAuto == false)
        return;

    double minTime = 1000000000, minDistance = 1000000000;
    double minSpeedComponent = 1000000000;

    for (int i = 0; i < walls.size(); i++)
    {
        double distance = walls[i].getDistance(ball.pos.x, ball.pos.y) - ball.radius;

        double speedComponent = ball.speed * Vector3D::dot(walls[i].normal, ball.direction);

        // ball is going away from the wall
        if (speedComponent >= 0)
            continue;

        double time = -distance / speedComponent;

        if (time < minTime)
        {
            minTime = time;
            minDistance = distance;
            minSpeedComponent = speedComponent;
            collidingWallIndex = i;
        }
    }

    // std::cout<<"minDistance: "<<minDistance<<"\n";
    // std::cout<<"minSpeedComponent: "<<minSpeedComponent<<"\n";
    // std::cout<<"minTime: "<<minTime<<"\n";

    if (minTime < 0)
    {
        checkCollision();
        scheduleEvent();
        return;
        // ball.reflectDirection(walls[collidingWallIndex].normal);
        // scheduleEvent();
        // return;
    }

    glutTimerFunc(minTime, handleEvent, simulateRestart); // schedule the next event
}

void drawWalls()
{
    for (int i = 0; i < walls.size(); i++)
    {
        walls[i].draw();
    }
}

void drawFloor()
{
    glPushMatrix();
    glScaled(5, 5, 5);
    glBegin(GL_QUADS);
    {
        for (int i = -50; i <= 50; i++)
        {
            for (int j = -50; j <= 50; j++)
            {
                if ((i + j) % 2 == 0)
                    glColor3f(0, 0, 0);
                else
                    glColor3f(1, 1, 1);

                glVertex3f(j, i, 0);
                glVertex3f(j, i + 1, 0);
                glVertex3f(j + 1, i + 1, 0);
                glVertex3f(j + 1, i, 0);
            }
        }
    }
    glEnd();
    glPopMatrix();
}

/* Initialize OpenGL Graphics */
void initGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClearDepth(1.0f);                   // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);              // Enable depth testing for z-culling
}

/* Handler for window-repaint event. Called back when the window first appears and
   whenever the window needs to be re-painted. */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glClearColor(0, 0, 0, 0);                           // color black

    glMatrixMode(GL_MODELVIEW); // To operate on Model-View matrix
    glLoadIdentity();           // Reset the model-view matrix

    camera.lookAt();

    drawFloor();
    ball.draw();
    drawWalls();

    glutSwapBuffers(); // Swap the front and back frame buffers (double buffering)
}

void specialKeyListener(int key, int x, int y)
{
    camera.specialKeyListener(key, x, y);
    glutPostRedisplay();
}

void animate(int x)
{
    if (!ball.isAuto)
        return;
    currentTime = std::chrono::steady_clock::now();
    std::chrono::microseconds deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - prevTime);
    long long deltaTimeMicroSec = deltaTime.count(); // Delta time in microseconds

    ball.move(deltaTimeMicroSec / 1000.0);
    // checkCollision();

    prevTime = currentTime;
    glutTimerFunc(0, animate, 0);
}

void ballManualMove(bool isForward)
{
    if(ball.isAuto) return;
    ball.moveMannual(isForward);
    checkCollision(isForward);
}

void rotateBall(bool isClockwise)
{
    simulateRestart++;
    if(isClockwise) ball.rotateClockwise();
    else ball.rotateCounterClockwise();
    checkCollision();
    scheduleEvent();
}

void keyboardListener(unsigned char key, int x, int y)
{
    camera.keyListener(key, x, y);
    glutPostRedisplay();

    if (key == 32) // spacebar
    {
        ball.isAuto = !ball.isAuto;
        if (ball.isAuto)
        {
            simulateRestart++;
            checkCollision();
            scheduleEvent();
            prevTime = std::chrono::steady_clock::now();
            animate(0);
        }

        else
        {
            collidingWallIndex = -1;
        }
    }

    else if (key == 'j')
    {
        rotateBall(COUNTER_CLOCKWISE);
    }

    else if (key == 'l')
    {
        rotateBall(CLOCKWISE);
    }

    else if (key == 'i')
    {
        ballManualMove(FORWARD);
    }

    else if (key == 'k')
    {
        ballManualMove(BACKWARD);
    }
}

void mouseListener(int button, int state, int x, int y)
{
    camera.mouseListener(button, state, x, y);
    glutPostRedisplay();
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
    gluPerspective(50.0f, aspect, 0.1f, 100.0f);
}

void idle()
{
    // ball.move();
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
    glutMouseFunc(mouseListener);
    glutIdleFunc(idle);
    glutMainLoop(); // Enter the infinite event-processing loop
    return 0;
}