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
#include <math.h>

class Bezier{
	public:
		static void uniform_subdivide(float*** patch, float step, std::vector<float***>* surfaces);
		void adaptive_subdivide(Vector **patch, float error);
		static float* bernstein_basis(float** controlPoints, float parameter);
};