#include <iostream>
#include <fstream>
#include <cmath>
#include <stack>
#include <vector>
#include <string>
#include "Vector3D.h"
#include "Matrix.h"
#include "Triangle.h"
#include "bitmap_image.hpp"

int main(int argc, char *argv[])
{
    
    std::string scene_file = "scene.txt";
    std::string config_file = "config.txt";

    if (argc == 3)
    {
        scene_file = argv[1];
        config_file = argv[2];
    }


    std::ifstream fin;
    std::ofstream fout;
    Vector3D a, b, c;
    
    // ------------------ Stage 1 ------------------
    fin.open(scene_file);
    fout.open("stage1.txt");


    std::stack<Matrix> s;
    Matrix M = Matrix::identity();

    Vector3D look, eye, up, right;
    fin >> eye >> look >> up;

    double fovY, aspectRatio, near, far;
    fin >> fovY >> aspectRatio >> near >> far;

    std::string command;
    while (fin >> command)
    {
        if (command == "triangle")
        {
            fin >> a >> b >> c;
            Triangle tri(a, b, c);
            tri = M * tri;
            fout << tri;
            fout << std::endl;
        }
        else if (command == "translate")
        {
            double tx, ty, tz;
            fin >> tx >> ty >> tz;
            Matrix m = Matrix::translate(tx, ty, tz);
            M = M * m;
        }
        else if (command == "scale")
        {
            double sx, sy, sz;
            fin >> sx >> sy >> sz;
            Matrix m = Matrix::scale(sx, sy, sz);
            M = M * m;
        }
        else if (command == "rotate")
        {
            double angle;
            Vector3D axis;
            fin >> angle >> axis;
            Matrix m = Matrix::rotate(axis, angle);
            M = M * m;
        }
        else if (command == "push")
        {
            s.push(M);
        }
        else if (command == "pop")
        {
            M = s.top();
            s.pop();
        }
        else if (command == "end")
        {
            break;
        }
    }

    fin.close();
    fout.close();


    // ------------------ Stage 2 ------------------

    Vector3D l = look - eye;
    l.normalize();
    Vector3D r = l * up;
    r.normalize();
    Vector3D u = r * l;

    Matrix T = Matrix::translate(-eye.x, -eye.y, -eye.z);

    double mat[4][4] = {
        r.x, r.y, r.z, 0,
        u.x, u.y, u.z, 0,
        -l.x, -l.y, -l.z, 0,
        0, 0, 0, 1};

    Matrix R(mat);
    Matrix V = R * T;

    fin.open("stage1.txt");
    fout.open("stage2.txt");
    while (fin >> a >> b >> c)
    {
        a = V * a;
        b = V * b;
        c = V * c;
        fout << a << b << c << std::endl;
    }

    fin.close();
    fout.close();


    // ------------------ Stage 3 ------------------

    double fovX = fovY * aspectRatio;
    double t = near * tan(fovY / 2 * M_PI / 180);
    double rr = near * tan(fovX / 2 * M_PI / 180);

    double mat2[4][4] = {
        near / rr, 0, 0, 0,
        0, near / t, 0, 0,
        0, 0, -(far + near) / (far - near), -(2 * far * near) / (far - near),
        0, 0, -1, 0
    };

    Matrix P(mat2);

    fin.open("stage2.txt");
    fout.open("stage3.txt");
    while (fin >> a >> b >> c)
    {
        a = P * a;
        b = P * b;
        c = P * c;

        a.reset_w();
        b.reset_w();
        c.reset_w();

        fout << a << b << c << std::endl;
    }

    fin.close();
    fout.close();


    // ------------------ Stage 4 ------------------

    int screen_width, screen_height;
    std::vector<Triangle> triangles;

    fin.open(config_file);
    fin >> screen_width >> screen_height;
    fin.close();

    fin.open("stage3.txt");
    while (fin >> a >> b >> c)
    {
        triangles.push_back(Triangle(a, b, c));
    }
    fin.close();

    double left_limit = -1.0, right_limit = 1.0, bottom_limit = -1.0, top_limit = 1.0, z_max = 1.0, z_min = -1.0;
    double dx = (right_limit - left_limit) / screen_width;
    double dy = (top_limit - bottom_limit) / screen_height;
    double top_y = top_limit - dy / 2;
    double left_x = left_limit + dx / 2;

    std::vector<std::vector<double>> z_buffer(screen_height, std::vector<double>(screen_width, z_max));

    bitmap_image image(screen_width, screen_height);
    image.set_all_channels(0, 0, 0);

    for (auto &tri : triangles)
    {
        tri.set_random_color();
        
        // Find top_scanline and bottom_scanline after necessary clipping
        double max_y = std::max(std::max(tri.a.y, tri.b.y), tri.c.y);
        double min_y = std::min(std::min(tri.a.y, tri.b.y), tri.c.y);

        min_y = std::max(min_y, bottom_limit);
        max_y = std::min(max_y, top_limit);

        int top_scanline = (int)((top_y - max_y) / dy);
        int bottom_scanline = (int)((top_y - min_y) / dy);

        // for row_no from top_scanline to bottom_scanline
        for (int row_no = top_scanline; row_no <= bottom_scanline; row_no++)
        {
            double scanline_y = top_y - row_no * dy;

            // Find left_intersecting_column and right_intersecting_column after necessary clipping
            Vector3D vertices[3] = {tri.a, tri.b, tri.c};
            std::vector<std::pair<double, double>> intersecting_points;

            for (int i = 0; i < 3; i++)
            {
                Vector3D v1 = vertices[i];
                Vector3D v2 = vertices[(i + 1) % 3];

                if (v1.y == v2.y)
                {
                    if (v1.y == scanline_y)
                    {
                        intersecting_points.push_back(std::make_pair(v1.x, v1.z));
                        intersecting_points.push_back(std::make_pair(v2.x, v2.z));
                        break;
                    }
                    continue;
                }

                if ((v1.y < scanline_y && v2.y < scanline_y) || (v1.y > scanline_y && v2.y > scanline_y))
                {
                    continue;
                }

                double x = v1.x + (scanline_y - v1.y) * (v2.x - v1.x) / (v2.y - v1.y);
                double z = v1.z + (scanline_y - v1.y) * (v2.z - v1.z) / (v2.y - v1.y);
                intersecting_points.push_back(std::make_pair(x, z));
            }

            if (intersecting_points.size() < 2)
                continue;

            std::sort(intersecting_points.begin(), intersecting_points.end());

            double min_x = std::max(intersecting_points[0].first, left_limit);
            double max_x = std::min(intersecting_points[1].first, right_limit);

            int left_intersecting_column = (int)((min_x - left_x) / dx);
            int right_intersecting_column = (int)((max_x - left_x) / dx);

            double start_z = intersecting_points[0].second;
            double end_z = intersecting_points[1].second;

            // for col_no from left_intersecting_column to right_intersecting_column
            for (int col_no = left_intersecting_column; col_no <= right_intersecting_column; col_no++)
            {
                double scanline_x = left_x + col_no * dx;

                double z = (scanline_x - min_x) * (end_z - start_z) / (max_x - min_x) + start_z;

                if (z >= z_min and z <= z_max)
                {
                    // Check if current z value is closer than previous z value for this pixel
                    if (z < z_buffer[row_no][col_no])
                    {
                        // Update z-buffer and pixel color
                        z_buffer[row_no][col_no] = z;
                        image.set_pixel(col_no, row_no, tri.red, tri.green, tri.blue);
                    }
                }
            }
        }
    }

    image.save_image("out.bmp");
    fout.open("z_buffer.txt");
    for (int i = 0; i < screen_height; i++)
    {
        for (int j = 0; j < screen_width; j++)
        {
            if (z_buffer[i][j] < z_max)
                fout << z_buffer[i][j] << "\t";
        }
        fout << std::endl;
    }
    fout.close();

    return 0;
}