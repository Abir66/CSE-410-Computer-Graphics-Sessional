#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include "1905066_bitmap_image.hpp"

#include <GL/glut.h>
#include "1905066_camera.h"
#include "1905066_vector3D.h"
#include "1905066_classes.h"

#include <filesystem>


#define PARALLEL 1

#ifdef PARALLEL
    #include <execution>
#endif


Camera camera(Vector3D(20, 20, 10), Vector3D(0, 0, 10),5,3);


int recursion_level;
int image_width, image_height;
int window_width = 768, window_height = 768;
double view_angle = 50;


std::vector<Object*> objects;
std::vector<PointLight*> pointLights;
std::vector<SpotLight*> spotLights;

std::vector<int> image_horizontal_iterator, image_vertical_iterator;
std::string input_scene = "scene.txt";
int image_count = 0;

void loadData()
{
	std::ifstream fin(input_scene);
	fin >> recursion_level >> image_height;

	image_width = image_height;

	int obj_count, point_light_count, spot_light_count;
	std::string objType;
    
    fin >> obj_count;

	for(int i=0;i<obj_count;i++)
	{
		
		fin >> objType;

        if(objType == "sphere"){
            Sphere *sphere = new Sphere();
            fin >> *sphere;
            objects.push_back(sphere);
		}

        else if(objType == "triangle"){
            Triangle *triangle = new Triangle();
            fin >> *triangle;
            objects.push_back(triangle);
		}

		else if(objType == "general"){
			General *general = new General();
            fin >> *general;    
            objects.push_back(general);
		}

        else if(objType == "quad"){
            Quad *quad = new Quad();
            fin >> *quad;
            objects.push_back(quad);
        }
	}

    fin>>point_light_count;
    for(int i=0;i<point_light_count;i++){
        PointLight *pointLight = new PointLight();
        fin >> *pointLight;
        pointLights.push_back(pointLight);
    }

    fin>>spot_light_count;
    for(int i=0;i<spot_light_count;i++){
        SpotLight *spotLight = new SpotLight();
        fin >> *spotLight;
        spotLights.push_back(spotLight);
    }


    Floor *floor = new Floor(500, 20);
    floor->setCoEfficients(0.7, 0.4, 0.2, 0.2);
    floor->setShine(5);
    objects.push_back(floor);


    fin.close();


    image_horizontal_iterator.resize(image_width);
    image_vertical_iterator.resize(image_height);

    for(int i=0;i<image_width;i++) image_horizontal_iterator[i] = i;
    for(int i=0;i<image_height;i++) image_vertical_iterator[i] = i;     

}

void set_pixel(bitmap_image &image, int i, int j, Vector3D curPixel){
    Object* nearest = nullptr;
    double t, tMin = INF;

    Ray ray(camera.pos, curPixel - camera.pos);

    Color color;

    nearest = nullptr;
    for(auto objPtr : objects){
        t = objPtr->get_intersection_distance(ray);
        
        if(t > 0 &&  t < tMin){
            tMin = t;
            nearest = objPtr;
        }
    }

    if(nearest != nullptr){
        double t = nearest->intersect(ray, color, recursion_level);
        image.set_pixel(i, j, 255*color.r, 255*color.g, 255*color.b);
    }

}


void capture(){
    bitmap_image image(image_width, image_height);
   
    for(int i=0;i<image_width;i++){
        for(int j=0;j<image_height;j++){
            image.set_pixel(i, j, 0, 0, 0);
        }
    }


    auto start_time = std::chrono::steady_clock::now();

    double plane_distance = (window_height/2.0) / tan(PI * (view_angle/2.0) /180.0);
    Vector3D top_left = camera.pos + camera.l*plane_distance - camera.r*(window_width/2) + camera.u*(window_height/2);

    double du = window_width / (image_width * 1.0);
    double dv = window_height / (image_height * 1.0);

    top_left = top_left + camera.r*(0.05 * du) - camera.u*(0.05 * dv);




#ifdef PARALLEL
    // https://youtu.be/46ddlUImiQA?list=PLlrATfBNZ98edc5GshdBtREv5asFW3yXl
    // use cpp 17
    // need to install libtbb-dev
    // g++ -std=c++17 -ltbb 1905066_main.cpp -o main -lGL -lGLU -lglut
    // need to link tbb library
    // -I/usr/include/tbb -L/usr/lib/x86_64-linux-gnu -ltbb

    std::for_each(std::execution::par,  image_vertical_iterator.begin(), image_vertical_iterator.end(),
         [&](int i){
            std::for_each(image_horizontal_iterator.begin(), image_horizontal_iterator.end(),
                [&](int j){
                    Vector3D curPixel = top_left + camera.r*(i*du) - camera.u*(j*dv);
                    set_pixel(image, i, j, curPixel);
                }
            );
    });
    
    
#else

    for(int i=0;i<image_height;i++){
        for(int j=0;j<image_width;j++){
            Vector3D curPixel = top_left + camera.r*(i*du) - camera.u*(j*dv);
            set_pixel(image, i, j, curPixel);
        }
    }



#endif

    auto end_time = std::chrono::steady_clock::now();
    std::chrono::microseconds time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    std::cout<<"Time taken to render: "<<time.count()/1000000.0<<" seconds"<<std::endl;

    std::string image_name = "output_" + std::to_string(image_count++) + ".bmp";
    std::cout<<"image saved as "<<image_name<<std::endl;
    image.save_image(image_name);
}



/* Initialize OpenGL Graphics */
void initGL()
{
    loadData();
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

    for(auto obj : objects) obj->draw();
    for(auto light : pointLights) light->draw();
    for(auto light : spotLights) light->draw();
    
    glutSwapBuffers(); // Swap the front and back frame buffers (double buffering)
}

void specialKeyListener(int key, int x, int y)
{
    camera.specialKeyListener(key, x, y);
    glutPostRedisplay();
}

void animate(int x)
{
    glutTimerFunc(0, animate, 0);
}


void keyboardListener(unsigned char key, int x, int y)
{
    camera.keyListener(key, x, y);

    if (key == 'q')
    {
        exit(0);
    }

    else if(key =='0') capture();
   
}

void mouseListener(int button, int state, int x, int y)
{
    camera.mouseListener(button, state, x, y);
    
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
    gluPerspective(view_angle, aspect, 0.1f, 1000.0f);
}

void idle()
{
    // ball.move();
    glutPostRedisplay();
}


void clear_memory()
{
    for(auto obj : objects) delete obj;
    for(auto light : pointLights) delete light;
    for(auto light : spotLights) delete light;

    objects.clear();
    pointLights.clear();    
    spotLights.clear();
}

/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char **argv)
{
    glutInit(&argc, argv);                                    // Initialize GLUT
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color
    glutInitWindowSize(window_width, window_height);                             // Set the window's initial width & height
    glutCreateWindow("title");                                // Create window with the given title
    glutDisplayFunc(display);                                 // Register callback handler for window re-paint event
    glutReshapeFunc(reshape);                                 // Register callback handler for window re-size event
    initGL(); // Our own OpenGL initialization
    glutSpecialFunc(specialKeyListener);
    glutKeyboardFunc(keyboardListener);
    glutMouseFunc(mouseListener);
    glutIdleFunc(idle);
    atexit(clear_memory);
    glutMainLoop(); // Enter the infinite event-processing loop
    return 0;
}