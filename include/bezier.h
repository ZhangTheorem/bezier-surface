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
		static void uniform_subdivide(Vector**, float, std::vector<Vector**>*, std::vector<Vector**>*);
		void adaptive_subdivide(Vector** patch, float error);
		static Vector bernstein_basis(Vector, Vector, Vector, Vector, float);
		static Vector curve_derivative(Vector, Vector, Vector, Vector, float);
};