#include "bezier.h"
using namespace std;

//Since this algorithm wants the point and the derivative,
//second "point" is the derivative

std::vector<Vector> Bezier::curveinterp(std::vector<Vector> *curve, float u){
	Vector a = (1.0 - u)*curve->at(0) + u*curve->at(1);
	Vector b = (1.0 - u)*curve->at(1) + u*curve->at(2);
	Vector c = (1.0 - u)*curve->at(2) + u*curve->at(3);
	Vector d = a * (1.0 - u) + b * u;
	Vector e = b * (1.0 - u) + c * u;
	Vector p = d * (1.0 - u) + e * u;
	Vector dPdu = (e - d) * 3.0f;
	std::vector<Vector> pdPdu; //this is amazing 
	pdPdu.push_back(p);
	pdPdu.push_back(dPdu);
	return pdPdu;
}

std::vector<Vector> Bezier::patchinterp(std::vector<Vector> *patch, float u, float v){
	//I think patch can be passed as array of std::vector<Vector>s
	std::vector<Vector> vcurve;
	vcurve.push_back(Vector());
	return vcurve;
}
	

void Bezier::uniform_subdivide(std::vector<Vector> *patch, float step){
	int numdiv = (1 + 0.0003) / step;
	//for eatch parametric value of u
	for(int i = 0; i <= numdiv; i++){
		float u = i * step;
		for(int j = 0; j <= numdiv; j++){
			float v = j * step;
			std::vector<Vector> pn = patchinterp(patch, u, v);
			//do something with p and normal
			v = u + v;
		}
	}
}