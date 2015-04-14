#ifndef BEZIER_H
#define BEZIER_H
#endif

#ifndef VECTOR_H
#include "vector.h"
#endif

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Bezier{
	Vector curveinterp(std::vector<Vector> *curve, float u);
	std::vector<Vector> curveinterpwithdu(std::vector<Vector> *curve, float u);
	std::vector<Vector> patchinterp(Vector **patch, float u, float v);
	void uniform_subdivide(Vector **patch, float step);
	void adaptive_subdivide(Vector **patch, float error);
};