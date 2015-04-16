#include "bezier.h"

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

void Bezier::uniform_subdivide(Vector** patch, float step, std::vector<Vector**>* surfaces, std::vector<Vector**>* normals){
    int numdiv = ceil(1.0f / step) + 1;
    Vector** surface = new Vector*[numdiv];
    for (int r = 0; r < numdiv; r++) {
        surface[r] = new Vector[numdiv];
    }
    Vector** normal = new Vector*[numdiv];
    for (int r = 0; r < numdiv; r++) {
        normal[r] = new Vector[numdiv];
    }

    Vector* ucurve = new Vector[4];
    Vector* vcurve = new Vector[4];

    for (int i = 0; i < numdiv; i++) {
        float u = min(i * step, 1);
        vcurve[0] = bernstein_basis(patch[0][0], patch[0][1], patch[0][2], patch[0][3], u);
        vcurve[1] = bernstein_basis(patch[1][0], patch[1][1], patch[1][2], patch[1][3], u);
        vcurve[2] = bernstein_basis(patch[2][0], patch[2][1], patch[2][2], patch[2][3], u);
        vcurve[3] = bernstein_basis(patch[3][0], patch[3][1], patch[3][2], patch[3][3], u);
        for (int j = 0; j < numdiv; j++) {
            float v = min(j * step, 1);
            ucurve[0] = bernstein_basis(patch[0][0], patch[1][0], patch[2][0], patch[3][0], v);
            ucurve[1] = bernstein_basis(patch[0][1], patch[1][1], patch[2][1], patch[3][1], v);
            ucurve[2] = bernstein_basis(patch[0][2], patch[1][2], patch[2][2], patch[3][2], v);
            ucurve[3] = bernstein_basis(patch[0][3], patch[1][3], patch[2][3], patch[3][3], v);
            surface[i][j] = bernstein_basis(ucurve[0], ucurve[1], ucurve[2], ucurve[3], u);
            Vector dPdu = curve_derivative(ucurve[0], ucurve[1], ucurve[2], ucurve[3], u);
            Vector dPdv = curve_derivative(vcurve[0], vcurve[1], vcurve[2], vcurve[3], v);
            normal[i][j] = Vector::cross(dPdu, dPdv).normalize();
        }
    }
    surfaces->push_back(surface);
    normals->push_back(normal);
}

Vector Bezier::patchInterpolate(Vector** patch, float u, float v){
    Vector ucurve[4] ;
    Vector vcurve[4];
    vcurve[0] = bernstein_basis(patch[0][0], patch[0][1], patch[0][2], patch[0][3], u);
    vcurve[1] = bernstein_basis(patch[1][0], patch[1][1], patch[1][2], patch[1][3], u);
    vcurve[2] = bernstein_basis(patch[2][0], patch[2][1], patch[2][2], patch[2][3], u);
    vcurve[3] = bernstein_basis(patch[3][0], patch[3][1], patch[3][2], patch[3][3], u);
    ucurve[0] = bernstein_basis(patch[0][0], patch[1][0], patch[2][0], patch[3][0], v);
    ucurve[1] = bernstein_basis(patch[0][1], patch[1][1], patch[2][1], patch[3][1], v);
    ucurve[2] = bernstein_basis(patch[0][2], patch[1][2], patch[2][2], patch[3][2], v);
    ucurve[3] = bernstein_basis(patch[0][3], patch[1][3], patch[2][3], patch[3][3], v);
    Vector point = bernstein_basis(ucurve[0], ucurve[1], ucurve[2], ucurve[3], u);
    //Vector dPdu = curve_derivative(ucurve[0], ucurve[1], ucurve[2], ucurve[3], u);
    //Vector dPdv = curve_derivative(vcurve[0], vcurve[1], vcurve[2], vcurve[3], v);
    return point;
}

Vector Bezier::curve_derivative(Vector cpoint0, Vector cpoint1, Vector cpoint2, Vector cpoint3, float parameter) {
    float tmp = 1.0f - parameter;
    Vector A = cpoint0 * tmp + cpoint1 * parameter;
    Vector B = cpoint1 * tmp + cpoint2 * parameter;
    Vector C = cpoint2 * tmp + cpoint3 * parameter;

    Vector D = A * tmp + B * parameter;
    Vector E = B * tmp + C * parameter;

    return (E - D) * 3;
}

// Assumes 4 control points given
Vector Bezier::bernstein_basis(Vector cpoint0, Vector cpoint1, Vector cpoint2, Vector cpoint3, float parameter) {
    Vector point = Vector();
    float tmp = 1.0f - parameter;
    float weight;

    weight = pow(tmp, 3);
    point += weight * cpoint0;

    weight = 3.0f * parameter * pow(tmp, 2);
    point += weight * cpoint1;

    weight = 3.0f * pow(parameter, 2) * tmp;
    point += weight * cpoint2;

    weight = pow(parameter, 3);
    point += weight * cpoint3;

    return point;
}

void Bezier::adaptive_subdivide(Vector** patch, float error, std::vector<Vector**>* surfaces, std::vector<Vector**>* normals){
    std::vector<Vector> surface;
    std::vector<Vector> normal;
    int numdiv = ceil(1.0f / error) + 1;
    for(int i = 0; i < numdiv; i++){
        float u = min(i * error, 1);
        for(int j = 0; j < numdiv; j++){
            float v = min(j * error, 1);
            Vector v1 = patch[i][j];
            Vector v2 = patch[i][j+1];
            Vector v3 = patch[i+1][j];
            Vector v4 = patch[i+1][j+1];
            //the uv values for each thing
            //fake 3D vectors, shudn't be a problem?? 
            //I thought it'd be more consistent with the regular vectors
            Vector pv1 = Vector(u, v, 0); 
            Vector pv2 = Vector(u, v + error, 0); 
            Vector pv3 = Vector(u + error, v, 0);
            Vector pv4 = Vector(u + error, v + error, 0);
            std::vector<Vector> top;
            std::vector<Vector> ptop;
            top.push_back(v1); top.push_back(v2); top.push_back(v3);
            ptop.push_back(pv1); ptop.push_back(pv2); ptop.push_back(pv3); 
            triangle_subdivide(patch, error, &top, &ptop);
            v4 = v4 + pv4;
        }
    }

}

void Bezier::triangle_subdivide(Vector** patch, float error, std::vector<Vector>* triangle, std::vector<Vector>* ptriangle){
    //following the picture I drew:
    Vector v1 = triangle->at(0);
    Vector v2 = triangle->at(1);
    Vector v3 = triangle->at(2);
    Vector v1v2 = (v1 - v2)*0.5; //midpoint
    Vector v2v3 = (v2 - v3)*0.5;
    Vector v3v1 = (v3 - v1)*0.5;
    Vector pv1 = ptriangle->at(0);
    Vector pv2 = ptriangle->at(1);
    Vector pv3 = ptriangle->at(2);
    float p12u = (pv1.x - pv2.x)*0.5; //midpoint of u
    float p12v = (pv1.y - pv2.y)*0.5; //midpoint of v
    Vector pv1v2 = patchInterpolate(patch, p12u, p12v);
    float p23u = (pv2.x - pv3.x)*0.5; //midpoint of u
    float p23v = (pv2.y - pv3.y)*0.5; //midpoint of v
    Vector pv2v3 = patchInterpolate(patch, p23u, p23v);
    float p31u = (pv3.x - pv1.x)*0.5; //midpoint of u
    float p31v = (pv3.y - pv1.y)*0.5; //midpoint of v
    Vector pv3v1 = patchInterpolate(patch, p31u, p31v);

    //for x1 x2
    if((pv1v2 - v1v2).len() < error){
        //this is flat
    }
    else{
        //not flat
    }
    if((pv1v2 - v1v2).len() < error){
        //this is flat
    }
    else{
        //not flat
    }
    if((pv1v2 - v1v2).len() < error){
        //this is flat
    }
    else{
        //not flat
    }
}