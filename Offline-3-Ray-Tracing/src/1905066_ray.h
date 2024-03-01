#ifndef RAY
#define RAY

#ifdef __unix__
    #include <GL/glut.h>
#elif defined(_WIN32) || defined(WIN32)
    #include <windows.h>
    #include <glut.h>
#endif

#include "1905066_vector3D.h"

struct Ray
{
    Vector3D start, dir;

    Ray(Vector3D start, Vector3D dir) {
        this->start = start;
        this->dir = dir.normal();
    }

};



double determinant(double ara[3][3]){
	double c1 = ara[0][0] * (ara[1][1] * ara[2][2] - ara[1][2] * ara[2][1]);
	double c2 = ara[0][1] * (ara[1][0] * ara[2][2] - ara[1][2] * ara[2][0]);
	double c3 = ara[0][2] * (ara[1][0] * ara[2][1] - ara[1][1] * ara[2][0]);
	return c1 - c2 + c3;
}


double ray_triangle_intersection(Ray ray, Vector3D a, Vector3D b, Vector3D c){
    double beta_matrix[3][3] = {
        {a.x - ray.start.x, a.x - c.x, ray.dir.x},
        {a.y - ray.start.y, a.y - c.y, ray.dir.y},
        {a.z - ray.start.z, a.z - c.z, ray.dir.z}
    };
    double gamma_matrix[3][3] = {
        {a.x - b.x, a.x - ray.start.x, ray.dir.x},
        {a.y - b.y, a.y - ray.start.y, ray.dir.y},
        {a.z - b.z, a.z - ray.start.z, ray.dir.z}
    };
    double t_matrix[3][3] = {
        {a.x - b.x, a.x - c.x, a.x - ray.start.x},
        {a.y - b.y, a.y - c.y, a.y - ray.start.y},
        {a.z - b.z, a.z - c.z, a.z - ray.start.z}
    };
    double A_matrix[3][3] {
        {a.x - b.x, a.x - c.x, ray.dir.x},
        {a.y - b.y, a.y - c.y, ray.dir.y},
        {a.z - b.z, a.z - c.z, ray.dir.z}
    };

    double A = determinant(A_matrix);
    double beta = determinant(beta_matrix) / A;
    double gamma = determinant(gamma_matrix) / A;
    double t = determinant(t_matrix) / A;

    if (beta + gamma < 1 && beta > 0 && gamma > 0 && t > 0){
        return t;
    }
    
    return -1;
    
}




#endif