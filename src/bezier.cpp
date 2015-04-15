#include "bezier.h"

//Since this algorithm wants the point and the derivative,
//second "point" is the derivative

// Vector Bezier::curveinterp(std::vector<Vector> *curve, float u){
//     Vector a = (1.0 - u)*curve->at(0) + u*curve->at(1);
//     Vector b = (1.0 - u)*curve->at(1) + u*curve->at(2);
//     Vector c = (1.0 - u)*curve->at(2) + u*curve->at(3);
//     Vector d = a * (1.0 - u) + b * u;
//     Vector e = b * (1.0 - u) + c * u;
//     Vector p = d * (1.0 - u) + e * u;
//     return p;
// }


// std::vector<Vector> Bezier::curveinterpwithdu(std::vector<Vector> *curve, float u){
//     Vector a = (1.0 - u)*curve->at(0) + u*curve->at(1);
//     Vector b = (1.0 - u)*curve->at(1) + u*curve->at(2);
//     Vector c = (1.0 - u)*curve->at(2) + u*curve->at(3);
//     Vector d = a * (1.0 - u) + b * u;
//     Vector e = b * (1.0 - u) + c * u;
//     Vector p = d * (1.0 - u) + e * u;
//     Vector dPdu = (e - d) * 3.0f;
//     std::vector<Vector> pdPdu; //this is amazing 
//     pdPdu.push_back(p);
//     pdPdu.push_back(dPdu);
//     return pdPdu;
// }

// std::vector<Vector> Bezier::patchinterp(Vector **patch, float u, float v){
//     //I think patch can be passed as 2D array
//     std::vector<Vector> vcurvefinal;
//     std::vector<Vector> ucurvefinal;
//     std::vector<Vector> vcurve0;
//     std::vector<Vector> vcurve1;
//     std::vector<Vector> vcurve2;
//     std::vector<Vector> vcurve3;
//     std::vector<Vector> ucurve0;
//     std::vector<Vector> ucurve1;
//     std::vector<Vector> ucurve2;
//     std::vector<Vector> ucurve3;
//     vcurve0.push_back(patch[0][0]); vcurve0.push_back(patch[0][1]);
//     vcurve0.push_back(patch[0][2]); vcurve0.push_back(patch[0][3]);
//     vcurve1.push_back(patch[1][0]); vcurve1.push_back(patch[1][1]);
//     vcurve1.push_back(patch[1][2]); vcurve1.push_back(patch[1][3]);
//     vcurve2.push_back(patch[2][0]); vcurve2.push_back(patch[2][1]);
//     vcurve2.push_back(patch[2][2]); vcurve2.push_back(patch[2][3]);
//     vcurve3.push_back(patch[3][0]); vcurve3.push_back(patch[3][1]);
//     vcurve3.push_back(patch[3][2]); vcurve3.push_back(patch[3][3]);
//     vcurvefinal.push_back(curveinterp(&vcurve0, u)); vcurvefinal.push_back(curveinterp(&vcurve1, u)); 
//     vcurvefinal.push_back(curveinterp(&vcurve2, u)); vcurvefinal.push_back(curveinterp(&vcurve3, u)); 
//     ucurve0.push_back(patch[0][0]); ucurve0.push_back(patch[1][0]);
//     ucurve0.push_back(patch[2][0]); ucurve0.push_back(patch[3][0]);
//     ucurve1.push_back(patch[0][1]); ucurve1.push_back(patch[1][1]);
//     ucurve1.push_back(patch[2][1]); ucurve1.push_back(patch[3][1]);
//     ucurve2.push_back(patch[0][2]); ucurve2.push_back(patch[1][2]);
//     ucurve2.push_back(patch[2][2]); ucurve2.push_back(patch[3][2]);
//     ucurve3.push_back(patch[0][3]); ucurve3.push_back(patch[1][3]);
//     ucurve3.push_back(patch[2][3]); ucurve3.push_back(patch[3][3]);
//     ucurvefinal.push_back(curveinterp(&ucurve0, v)); vcurvefinal.push_back(curveinterp(&ucurve1, v)); 
//     ucurvefinal.push_back(curveinterp(&ucurve2, v)); vcurvefinal.push_back(curveinterp(&ucurve3, v)); 
//     std::vector<Vector> pdPdv = curveinterpwithdu(&vcurvefinal, v);
//     std::vector<Vector> pdPdu = curveinterpwithdu(&ucurvefinal, u);
//     Vector n = Vector::cross(pdPdu.at(1), pdPdv.at(1));
//     n.normalize();
//     std::vector<Vector> pn; 
//     pn.push_back(pdPdu.at(0));
//     pn.push_back(n);
//     return pn; //this returns a vector of point and the normal
// }

void Bezier::uniform_subdivide(Vector** patch, float step, std::vector<Vector**>* surfaces, std::vector<Vector**>* normals){
    int numdiv = 1.0f / step + 1;
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
        float u = i * step;
        vcurve[0] = bernstein_basis(patch[0][0], patch[0][1], patch[0][2], patch[0][3], u);
        vcurve[1] = bernstein_basis(patch[1][0], patch[1][1], patch[1][2], patch[1][3], u);
        vcurve[2] = bernstein_basis(patch[2][0], patch[2][1], patch[2][2], patch[2][3], u);
        vcurve[3] = bernstein_basis(patch[3][0], patch[3][1], patch[3][2], patch[3][3], u);
        for (int j = 0; j < numdiv; j++) {
            float v = j * step;
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

