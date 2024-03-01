#ifndef COLOR
#define COLOR

#ifdef __unix__
    #include <GL/glut.h>
#elif defined(_WIN32) || defined(WIN32)
    #include <windows.h>
    #include <glut.h>
#endif

#include <iostream>

struct Color{
    double r,g,b;
    Color()
    {
        r = g = b = 0.0;
    }

    Color(double r, double g, double b) : r(r), g(g), b(b) {}
    void set(double r, double g, double b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    void apply()
    {
        glColor3f(r, g, b);
    }

    void normalize()
    {
        if(r > 1) r = 1;
        if(g > 1) g = 1;
        if(b > 1) b = 1;

        if(r < 0) r = 0;
        if(g < 0) g = 0;
        if(b < 0) b = 0;
    }   

    // * operator overloading
    Color operator*(double a){
        return Color(r * a, g * a, b * a);
    }

    Color operator+(Color color){
        return Color(r + color.r, g + color.g, b + color.b);
    }

    Color operator*(Color color){
        return Color(r * color.r, g * color.g, b * color.b);
    }


    friend Color operator*(double a, Color color){
        return Color(color.r * a, color.g * a, color.b * a);
    }


    friend std::istream& operator>>(std::istream& is, Color &color){
        is >> color.r >> color.g >> color.b;
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, Color &color){
        os << color.r << " " << color.g << " " << color.b;
        return os;
    }

};


#define WHITE Color(1, 1, 1)
#define RED Color(1, 0, 0)
#define GREEN Color(0, 1, 0)
#define BLUE Color(0, 0, 1)
#define YELLOW Color(1, 1, 0)
#define CYAN Color(0, 1, 1)
#define MAGENTA Color(1, 0, 1)
#define BLACK Color(0, 0, 0)

#endif