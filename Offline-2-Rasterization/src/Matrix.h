#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <fstream>
#include <cmath>
#include "Vector3D.h"

struct Matrix
{
    double mat[4][4] = {0.0};

    Matrix(){};

    Matrix(double mat[][4])
    {
        for(int i=0; i<4; i++){
            for(int j=0; j<4; j++){
                this->mat[i][j] = mat[i][j];
            }
        }
    }

    
    Matrix operator*(Matrix m)
    {
        Matrix res;
        for (int i = 0; i < 4; i++)
        {
            for (int k = 0; k < 4; k++)
            {
                for (int j = 0; j < 4; j++)
                {
                    res.mat[i][j] += mat[i][k] * m.mat[k][j];
                }
            }
        }
        return res;
    }

    Vector3D operator*(Vector3D &vec){

        double res[4]={0.0};
        double v[4]={vec.x, vec.y, vec.z, vec.w};

        for(int i=0; i<4; i++){
            for(int j=0; j<4; j++){
                res[i] += mat[i][j] * v[j];
            }
        }

        return Vector3D(res[0], res[1], res[2], res[3]);
    }


    static Matrix identity()
    {
        Matrix res;
        for (int i = 0; i < 4; i++)
        {
            res.mat[i][i] = 1;
        }
        return res;
    }

    static Matrix translate(double tx, double ty, double tz)
    {
        Matrix res = Matrix::identity();
        res.mat[0][3] = tx;
        res.mat[1][3] = ty;
        res.mat[2][3] = tz;
        return res;
    }

    static Matrix scale(double sx, double sy, double sz)
    {
        Matrix res = Matrix::identity();
        res.mat[0][0] = sx;
        res.mat[1][1] = sy;
        res.mat[2][2] = sz;
        return res;
    }

    static Matrix rotate(Vector3D axis, double angle)
    {
        axis.normalize();

        angle = angle * M_PI / 180.0;

        Vector3D i(1, 0, 0);
        Vector3D j(0, 1, 0);
        Vector3D k(0, 0, 1);

        i.rotate(axis, angle);
        j.rotate(axis, angle);
        k.rotate(axis, angle);

        double mat[4][4] = {
            {i.x, j.x, k.x, 0},
            {i.y, j.y, k.y, 0},
            {i.z, j.z, k.z, 0},
            {0, 0, 0, 1}
        };

        return Matrix(mat);
    }

};



#endif 