#include "bezier.h"

void Bezier::uniform_subdivide(float*** patch, float step, std::vector<float***>* surfaces){
    int numdiv = 1.0f / step + 1;
    float*** surface = new float**[numdiv];
    for (int r = 0; r < numdiv; r++) {
        surface[r] = new float*[numdiv];
        for (int c = 0; c < numdiv; c++) {
            surface[r][c] = new float[3];
        }
    }

    float** tmpControls = new float*[4];

    // For eatch parametric value of u
    for (int i = 0; i < numdiv; i++) {
        float u = i * step;
        tmpControls[0] = bernstein_basis(patch[0], u);
        tmpControls[1] = bernstein_basis(patch[1], u);
        tmpControls[2] = bernstein_basis(patch[2], u);
        tmpControls[3] = bernstein_basis(patch[3], u);
        for (int j = 0; j < numdiv; j++) {
            float v = j * step;
            surface[i][j] = bernstein_basis(tmpControls, v);
        }
    }
    surfaces->push_back(surface);
}

// Assumes 4 control points given
float* Bezier::bernstein_basis(float** controlPoints, float parameter) {
    float* point = new float[3];
    point[0] = point[1] = point[2] = 0;
    float tmp = 1.0f - parameter;
    float weight;
    float* control;

    control = controlPoints[0];
    weight = pow(tmp, 3);
    point[0] += weight * control[0];
    point[1] += weight * control[1];
    point[2] += weight * control[2];

    control = controlPoints[1];
    weight = 3.0f * parameter * pow(tmp, 2);
    point[0] += weight * control[0];
    point[1] += weight * control[1];
    point[2] += weight * control[2];

    control = controlPoints[2];
    weight = 3.0f * pow(parameter, 2) * tmp;
    point[0] += weight * control[0];
    point[1] += weight * control[1];
    point[2] += weight * control[2];

    control = controlPoints[3];
    weight = pow(parameter, 3);
    point[0] += weight * control[0];
    point[1] += weight * control[1];
    point[2] += weight * control[2];

    return point;
}

void Bezier::adaptive_subdivide(Vector **patch, float error){

}

