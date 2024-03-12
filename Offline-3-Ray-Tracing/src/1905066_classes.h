#ifndef CLASSES
#define CLASSES

#ifdef __unix__
    #include <GL/glut.h>
#elif defined(_WIN32) || defined(WIN32)
    #include <windows.h>
    #include <glut.h>
#endif

#include "1905066_vector3D.h"
#include "1905066_color.h"
#include "1905066_ray.h"
#include "1905066_light.h"
#include <vector>

#define COEFF_AMBIENT 		0
#define COEFF_DIFFUSE 		1
#define COEFF_SPECULAR 		2
#define COEFF_REFLECTION 	3

#define PI 2*acos(0.0)
#define EPSILON 0.000001
#define INF 1000000000

struct Object;

extern std::vector<Object*> objects;
extern std::vector<PointLight*> pointLights;
extern std::vector<SpotLight*> spotLights;


struct Object
{
    Vector3D reference_point;
    double height, width, length;
    Color color;
    std::vector<double> coEfficients{0.0, 0.0, 0.0, 0.0};
    int shine;

    double refraction_index = 1.0;
    double refrection_coefficient = 0.0;    

    Object(){}
    
    virtual void draw() = 0;
    
    void setColor(Color color)
    {
        this->color = color;
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }

    void setCoEfficients(double ambient, double diffuse, double specular, double reflection)
	{
		coEfficients[COEFF_AMBIENT] = ambient;
		coEfficients[COEFF_DIFFUSE] = diffuse;
		coEfficients[COEFF_SPECULAR] = specular;
		coEfficients[COEFF_REFLECTION] = reflection;
	}


    virtual double intersect(Ray ray, Color &color, int level){
        double t_min = get_intersection_distance(ray);
        if(t_min < 0) return -1;
        if(level == 0) return t_min;

        Vector3D intersection_point = ray.start + ray.dir * t_min;
        Color insection_point_color = getColor(intersection_point);
        color = insection_point_color * coEfficients[COEFF_AMBIENT];

        Vector3D normal = getNormalAt(intersection_point);


        bool isInside = false;
        if(Vector3D::dot(normal, ray.dir) > 0) {
            normal = -normal;
            isInside = true;
        }


        for(auto light : pointLights){

            Vector3D light_direction = (intersection_point - light->light_pos).normal();
            
            if(Vector3D::dot(light_direction, normal) > 0) continue;
            
            Ray ray2(light->light_pos + light_direction * EPSILON, light_direction);

            bool isBlocked = false;
            double own_distance = (intersection_point - light->light_pos).value();  
            
            for(auto obj : objects){
                if(obj == this) continue;
                double t = obj->get_intersection_distance(ray2);
                if(t > 0 && t < own_distance){
                    isBlocked = true;
                    break;
                }
            }

            if(!isBlocked){
                double lambert_value = std::max(0.0, Vector3D::dot(light_direction * (-1), normal));
                Vector3D reflection =  light_direction - normal * 2 * Vector3D::dot(light_direction, normal);
                reflection = reflection.normal();

                double phong = std::max(0.0, Vector3D::dot(reflection, ray.dir * (-1)));

                color = color + light->color * coEfficients[COEFF_DIFFUSE] * lambert_value * insection_point_color;
                color = color + light->color * coEfficients[COEFF_SPECULAR] * pow(phong, shine) * insection_point_color;
            }
        }

       
        for(auto spotLight : spotLights){
            auto light = spotLight->point_light;
            Vector3D light_direction = (intersection_point - light.light_pos).normal();

            if(Vector3D::dot(light_direction, normal) > 0) continue;
            
            double dot_product = Vector3D::dot(light_direction, (spotLight->light_direction).normal());
            double angle = acos(dot_product) * 180 / PI;

            if(abs(angle) > spotLight->cut_off_angle) continue;

        
            Ray ray2(light.light_pos + light_direction * EPSILON, light_direction);

            bool isBlocked = false;
            double own_distance = (intersection_point - light.light_pos).value();  
            
            for(auto obj : objects){
                if(obj == this) continue;
                double t = obj->get_intersection_distance(ray2);
                if(t > 0 && t < own_distance){
                    isBlocked = true;
                    break;
                }
            }

            if(!isBlocked){
                double spot_light_intensity = 1 - (1-dot_product)  / (1-cos(spotLight->cut_off_angle * PI / 180));
                
                spot_light_intensity = std::max(0.0, spot_light_intensity);
                spot_light_intensity = std::min(1.0, spot_light_intensity);

              
                double lambert_value = std::max(0.0, Vector3D::dot(light_direction * (-1), normal));
                Vector3D reflection =  light_direction - normal * 2 * Vector3D::dot(light_direction, normal);
                reflection.normalize();
                double phong = std::max(0.0, Vector3D::dot(reflection, ray.dir * (-1)));

                color = color + light.color * spot_light_intensity * coEfficients[COEFF_DIFFUSE] * lambert_value * insection_point_color;
                color = color + light.color * spot_light_intensity * coEfficients[COEFF_SPECULAR] * pow(phong, shine) * insection_point_color;
            }
        }


        // Vector3D reflected_ray_direction = ray.dir - normal * 2 * Vector3D::dot(ray.dir, normal);
        // reflected_ray_direction.normalize();

        Vector3D reflected_ray_direction = Vector3D::reflect3D(ray.dir, normal).normal();

        Ray reflected_ray(intersection_point + reflected_ray_direction * EPSILON, reflected_ray_direction);
        
        Color reflected_color;
        double t = INF;
        Object* nearest = nullptr;
        for(auto obj : objects){
            if(obj == this) continue;
            double t2 = obj->get_intersection_distance(reflected_ray);
            if(t2 > 0 && t2 < t){
                t = t2;
                nearest = obj;
            }
        }

        if(nearest != nullptr){
            t = nearest->intersect(reflected_ray, reflected_color, level - 1);
            if(t > 0){
                color = color + reflected_color * coEfficients[COEFF_REFLECTION];
            }
        } 


        // handle_refraction(ray, color, level, intersection_point, normal, isInside);

        color.normalize();
        return t_min;
    }



    void handle_refraction(Ray ray, Color &color, int level, Vector3D intersection_point, Vector3D normal, bool isInside){
        
        Vector3D i = -ray.dir;
        
        Vector3D n = normal;
        
        // double refraction_index = this->refraction_index;
        double refraction_index = 1.3;
        if(isInside) {
            refraction_index = 1.0 / refraction_index;
            n = -n;
        }
        
        double n_dot_i = Vector3D::dot(n,i);

        double sqrt_term = 1 - refraction_index*refraction_index*(1 - n_dot_i*n_dot_i);

        // full internal reflection
        if(sqrt_term < 0) return;

        Vector3D refracted_ray_dir = (n_dot_i * refraction_index - sqrt(sqrt_term)) * n - refraction_index * i;

        Ray refracted_ray(intersection_point + refracted_ray_dir * EPSILON, refracted_ray_dir);

        Color refracted_color;
        
        double t = INF;
        Object* nearest = nullptr;
        for(auto obj : objects){
            if(obj == this) continue;
            double t2 = obj->get_intersection_distance(refracted_ray);
            if(t2 > 0 && t2 < t){
                t = t2;
                nearest = obj;
            }
        }

        if(nearest != nullptr){
            t = nearest->intersect(refracted_ray, refracted_color, level-1);
            if(t > 0){
                color = color + refracted_color * 0.5;
            }
        }

        color.normalize();
    }

    virtual double get_intersection_distance(Ray ray) {
        return -1;      
    }

    virtual Color getColor(Vector3D p){
        return color;
    }

    virtual Vector3D getNormalAt(Vector3D &intersection_point) = 0;

   
    ~Object(){
        coEfficients.clear();
        std::cout << "Object destroyed" << std::endl;
    }

};


struct Triangle : public Object
{
    Vector3D points[3];
    
    Triangle(){}
    
    Triangle(Vector3D a, Vector3D b, Vector3D c)
    {
        points[0] = a;
        points[1] = b;
        points[2] = c;
    }

    void draw()
    {
        color.apply();
        glBegin(GL_TRIANGLES);
        {
            for (int i = 0; i < 3; i++)
            {
                glVertex3f(points[i].x, points[i].y, points[i].z);
            }
        }
        glEnd();
    }

    double get_intersection_distance(Ray ray) {
        return ray_triangle_intersection(ray, points[0], points[1], points[2]);
    }

    Vector3D getNormalAt(Vector3D &intersection_point){
        Vector3D normal = (points[1] - points[0]) * (points[2] - points[0]);
        return normal.normal();
    }   

    friend std::istream& operator>>(std::istream& is, Triangle& t){
        is >> t.points[0] >> t.points[1] >> t.points[2];
        is >> t.color;
        is >> t.coEfficients[0] >> t.coEfficients[1] >> t.coEfficients[2] >> t.coEfficients[3];
        is >> t.shine;
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, Triangle& t)
    {
        os << t.points[0] << " " << t.points[1] << " " << t.points[2] << std::endl;
        os << t.color << std::endl;
        return os;
    }
};

struct  Sphere : public Object
{
    int sectorCount = 40, stackCount = 40;
    

    Sphere(){}
    
    Sphere(Vector3D center, double radius, int sectorCount = 40, int stackCount = 40)
    {
        reference_point = center;
        length = radius;
        this->sectorCount = sectorCount;
        this->stackCount = stackCount;
        
    }


    void draw()
    {
        glPushMatrix();{
            glTranslatef(reference_point.x, reference_point.y, reference_point.z);
            color.apply();
            glutSolidSphere(length, sectorCount, stackCount);
        }glPopMatrix(); 
    }

    double get_intersection_distance(Ray ray) {

        ray.start = ray.start - reference_point; 

        double a = 1;
        double b = 2 * Vector3D::dot(ray.dir, ray.start);
        double c = Vector3D::dot(ray.start , ray.start) - (length * length);

        double discriminant = b*b - 4 * a * c;

        if(discriminant < 0) return -1;

        double t = -1;

        double d = sqrt(discriminant);
        double t1 = (-b - d) / (2 * a);
        double t2 = (-b + d) / (2 * a);

        double tmin = std::min(t1, t2);
        double tmax = std::max(t1, t2);

        if(tmin > 0) t = tmin;
        else if(tmax > 0) t = tmax;
        else return -1;
        return t;
    }
    
    Vector3D getNormalAt(Vector3D &intersection_point){
        Vector3D normal = intersection_point - reference_point;
        return normal.normal();
    }

    friend std::istream& operator>>(std::istream& is, Sphere& s){
        is >> s.reference_point >> s.length >> s.color;
        is >> s.coEfficients[0] >> s.coEfficients[1] >> s.coEfficients[2] >> s.coEfficients[3];
        is >> s.shine;
      
        return is;
    }

    
};

struct Floor : public Object
{
    double floorWidth, tileWidth;
    
    int noOfTiles;
    Color color1 = WHITE, color2 = BLACK;

    Floor(){
        floorWidth = 100;
        tileWidth = 10;
        noOfTiles = std::max(int(floorWidth / tileWidth),1);
        reference_point = Vector3D(-floorWidth / 2, -floorWidth / 2, 0);
        length = tileWidth;
    }

    Floor(int floorWidth,int tileWidth){
        this->floorWidth = floorWidth;
        this->tileWidth = tileWidth;
        noOfTiles = std::max(int(floorWidth / tileWidth),1);
        reference_point = Vector3D(-floorWidth / 2.0, -floorWidth / 2.0, 0);
        length = tileWidth;
    }

    void setColor(Color color1, Color color2)
    {
        this->color1 = color1;
        this->color2 = color2;
    }


    void draw(){
        for (int i = 0; i < noOfTiles; i++)
		{
			for (int j = 0; j < noOfTiles; j++)
			{
				if (((i + j) % 2) == 0) color1.apply();
				else color2.apply();
                
				glBegin(GL_QUADS);
				{
					glVertex3f(reference_point.x + i * length, reference_point.y + j * length, 0);
                    glVertex3f(reference_point.x + i * length, reference_point.y + (j + 1) * length, 0);
                    glVertex3f(reference_point.x + (i + 1) * length, reference_point.y + (j + 1) * length, 0);
                    glVertex3f(reference_point.x + (i + 1) * length, reference_point.y + j * length, 0);
				}
				glEnd();
			}
		}
    }

    Color getColor(Vector3D p){
        int i = (p.x - reference_point.x) / length;
        int j = (p.y - reference_point.y) / length;

        if(i < 0 || j < 0) return Color(0, 0, 0);

        if((i + j) % 2 == 0) return color1;
        else return color2;
    }

    Vector3D getNormalAt(Vector3D &intersection_point){
        return Vector3D(0, 0, 1);
    }

    double get_intersection_distance(Ray ray) {
        if(ray.dir.z == 0) return -1;
        double t = -ray.start.z / ray.dir.z;
        if(t < 0) return -1;

        // check if the intersection point is within the floor
        Vector3D p = ray.start + ray.dir * t;
        if(p.x < reference_point.x || p.x > reference_point.x + floorWidth || p.y < reference_point.y || p.y > reference_point.y + floorWidth) return -1;   

        return t;
    }

};

struct General : public Object{

    double A, B, C, D, E, F, G, H, I, J;
    // f(x, y, z) = Ax^2 + By^2 + Cz^2 + Dxy + Exz + Fyz + Gx + Hy + Iz + J = 0

    General(){}
    
    void draw(){
        // do nothing
    }

    Vector3D getNormalAt(Vector3D &intersection_point){

        double x = intersection_point.x;
        double y = intersection_point.y;
        double z = intersection_point.z;

        double df_dx = 2*A*x + D*y + E*z + G;
        double df_dy = 2*B*y + D*x + F*z + H;
        double df_dz = 2*C*z + E*x + F*y + I;

        Vector3D normal(df_dx, df_dy, df_dz);  
        return normal.normal(); 
    }

     double get_intersection_distance(Ray ray) {
        double x0 = ray.start.x, y0 = ray.start.y, z0 = ray.start.z;
        double xd = ray.dir.x, yd = ray.dir.y, zd = ray.dir.z;

        double a = A*xd*xd + B*yd*yd + C*zd*zd + D*xd*yd + E*xd*zd + F*yd*zd;
        double b = 2*(A*x0*xd + B*y0*yd + C*z0*zd) + D*(x0*yd + y0*xd) + E*(x0*zd + z0*xd) + F*(y0*zd + z0*yd) + G*xd + H*yd + I*zd;
        double c = A*x0*x0 + B*y0*y0 + C*z0*z0 + D*x0*y0 + E*x0*z0 + F*y0*z0 + G*x0 + H*y0 + I*z0 + J;

        double discriminant = b*b - 4*a*c;
        if(discriminant < 0) return -1;


        double d = sqrt(discriminant);
        double t1 = (-b - d) / (2 * a);
        double t2 = (-b + d) / (2 * a);

        double tmin = std::min(t1, t2);
        double tmax = std::max(t1, t2);

        if(tmin > 0 && isInside(ray.start + ray.dir * tmin)) return tmin;
        else if(tmax > 0 && isInside(ray.start + ray.dir * tmax)) return tmax;
        else return -1;
       
    }

    bool isInside(Vector3D p){
        if(length != 0 && (p.x < reference_point.x || p.x > reference_point.x + length)) return false;
        if(width != 0 && (p.y < reference_point.y || p.y > reference_point.y + width)) return false;
        if(height != 0 && (p.z < reference_point.z || p.z > reference_point.z + height)) return false;
        return true;
    }

    friend std::istream& operator>>(std::istream& is, General& g){
        is >> g.A >> g.B >> g.C >> g.D >> g.E >> g.F >> g.G >> g.H >> g.I >> g.J;
        is >> g.reference_point >> g.length >> g.width >> g.height;
        is >> g.color;
        is >> g.coEfficients[0] >> g.coEfficients[1] >> g.coEfficients[2] >> g.coEfficients[3];
        is >> g.shine;
        return is;
    }



};

struct Quad : public Object{

    Vector3D points[4];

    Quad(){}

    Quad(Vector3D a, Vector3D b, Vector3D c, Vector3D d){
        points[0] = a;
        points[1] = b;
        points[2] = c;
        points[3] = d;
    }

    void draw(){
        color.apply();
        glBegin(GL_QUADS);
        {
            for (int i = 0; i < 4; i++)
            {
                glVertex3f(points[i].x, points[i].y, points[i].z);
            }
        }
        glEnd();
    }

    Vector3D getNormalAt(Vector3D &intersection_point){
        Vector3D normal = (points[1] - points[0]) * (points[2] - points[0]);
        return normal.normal();
    }

    double get_intersection_distance(Ray ray) {
        double t = ray_triangle_intersection(ray, points[0], points[1], points[2]);
        if(t > 0) return t;
        return ray_triangle_intersection(ray, points[0], points[2], points[3]);
    }
        
    friend std::istream& operator>>(std::istream& is, Quad& q){
        is >> q.points[0] >> q.points[1] >> q.points[2] >> q.points[3];
        is >> q.color;
        is >> q.coEfficients[0] >> q.coEfficients[1] >> q.coEfficients[2] >> q.coEfficients[3];
        is >> q.shine;
        return is;
    }

};

#endif