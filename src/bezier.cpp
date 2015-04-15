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

void Bezier::adaptive_subdivide(Vector** patch, float error){

}

