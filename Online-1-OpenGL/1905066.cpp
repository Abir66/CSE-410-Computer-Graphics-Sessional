
#include <bits/stdc++.h>
using namespace std;

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <glut.h>

#endif

struct Point
{
    double x, y;
};

double bigLineTheta = 0;
double bigThetaIncrease = 0.5;

double smallLineTheta = 0;
double smallThetaIncrease = 2;

Point centerShift1 = {0, 0};
Point centerShift2 = {0, 0};


void DrawCircle(double radius, double x, double y)
{
    glPushMatrix();
    glTranslated(x, y, 0);
    glScaled(radius, radius, 1);
    glBegin(GL_LINE_LOOP);{
        for(int i = 0; i < 360; i = i+5){
            double theta = i * 3.1416 / 180;
            double x = cos(theta);
            double y = sin(theta);
            glVertex2d(x,y);
        }
    }glEnd();
    glPopMatrix();
}

void Drawline(Point p1, Point p2)
{
    glBegin(GL_LINES);
    {
        glVertex2d(p1.x, p1.y);
        glVertex2d(p2.x, p2.y);
    }
    glEnd();
}
void display()
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    Point center1 = {0, 0};
    double radius1 = 0.5;

    Point center2 = {center1.x + centerShift1.x * radius1,  center1.y + centerShift1.y * radius1};
    double radius2 = 0.2;

    Point center3 = {center2.x + centerShift2.x * radius2,  center2.y + centerShift2.y * radius2};
    double radius3 = 0.1;

    glColor3f(1,0,0);
    DrawCircle(radius1, center1.x, center1.y);

    glColor3f(0,0,1);
    Drawline(center1, center2);
    DrawCircle(radius2, center2.x, center2.y);

    glColor3f(0,1,1);
    Drawline(center2, center3);
    DrawCircle(radius3, center3.x, center3.y);

    glFlush();
}

void init()
{
    glClearColor(0, 0, 0, 1);
}

void Timer(int value)
{
    bigLineTheta += bigThetaIncrease;
    smallLineTheta += smallThetaIncrease;
    
    if(bigLineTheta > 360) bigLineTheta = 0;
    if(smallLineTheta > 360) smallLineTheta = 0;

    double theta = bigLineTheta * 3.1416 / 180;
    double theta2 = smallLineTheta * 3.1416 / 180;

    centerShift1.x = cos(theta);
    centerShift1.y = sin(theta);

    centerShift2.x = cos(theta2);
    centerShift2.y = sin(theta2);

    glutPostRedisplay();
    glutTimerFunc(10,Timer,0);
}

int main(int argc, char **argv)
{

    glutInit(&argc, argv);
    glutInitWindowPosition(500, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Magic Cube");

    init();

    glutDisplayFunc(display);
    glutTimerFunc(0,Timer,0);
    glutMainLoop();
    return 0;
}
