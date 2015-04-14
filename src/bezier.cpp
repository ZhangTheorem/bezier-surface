#include "bezier.h"
using namespace std;

//Since this algorithm wants the point and the derivative,
//second "point" is the derivative

Vector Bezier::curveinterp(std::vector<Vector> *curve, float u){
	Vector a = (1.0 - u)*curve->at(0) + u*curve->at(1);
	Vector b = (1.0 - u)*curve->at(1) + u*curve->at(2);
	Vector c = (1.0 - u)*curve->at(2) + u*curve->at(3);
	Vector d = a * (1.0 - u) + b * u;
	Vector e = b * (1.0 - u) + c * u;
	Vector p = d * (1.0 - u) + e * u;
	return p;
}


std::vector<Vector> Bezier::curveinterpwithdu(std::vector<Vector> *curve, float u){
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

std::vector<Vector> Bezier::patchinterp(Vector **patch, float u, float v){
	//I think patch can be passed as 2D array
	std::vector<Vector> vcurvefinal;
	std::vector<Vector> ucurvefinal;
	std::vector<Vector> vcurve0;
	std::vector<Vector> vcurve1;
	std::vector<Vector> vcurve2;
	std::vector<Vector> vcurve3;
	std::vector<Vector> ucurve0;
	std::vector<Vector> ucurve1;
	std::vector<Vector> ucurve2;
	std::vector<Vector> ucurve3;
	vcurve0.push_back(patch[0][0]); vcurve0.push_back(patch[0][1]);
	vcurve0.push_back(patch[0][2]); vcurve0.push_back(patch[0][3]);
	vcurve1.push_back(patch[1][0]); vcurve1.push_back(patch[1][1]);
	vcurve1.push_back(patch[1][2]); vcurve1.push_back(patch[1][3]);
	vcurve2.push_back(patch[2][0]); vcurve2.push_back(patch[2][1]);
	vcurve2.push_back(patch[2][2]); vcurve2.push_back(patch[2][3]);
	vcurve3.push_back(patch[3][0]); vcurve3.push_back(patch[3][1]);
	vcurve3.push_back(patch[3][2]); vcurve3.push_back(patch[3][3]);
	vcurvefinal.push_back(curveinterp(&vcurve0, u)); vcurvefinal.push_back(curveinterp(&vcurve1, u)); 
	vcurvefinal.push_back(curveinterp(&vcurve2, u)); vcurvefinal.push_back(curveinterp(&vcurve3, u)); 
	ucurve0.push_back(patch[0][0]); ucurve0.push_back(patch[1][0]);
	ucurve0.push_back(patch[2][0]); ucurve0.push_back(patch[3][0]);
	ucurve1.push_back(patch[0][1]); ucurve1.push_back(patch[1][1]);
	ucurve1.push_back(patch[2][1]); ucurve1.push_back(patch[3][1]);
	ucurve2.push_back(patch[0][2]); ucurve2.push_back(patch[1][2]);
	ucurve2.push_back(patch[2][2]); ucurve2.push_back(patch[3][2]);
	ucurve3.push_back(patch[0][3]); ucurve3.push_back(patch[1][3]);
	ucurve3.push_back(patch[2][3]); ucurve3.push_back(patch[3][3]);
	ucurvefinal.push_back(curveinterp(&ucurve0, v)); vcurvefinal.push_back(curveinterp(&ucurve1, v)); 
	ucurvefinal.push_back(curveinterp(&ucurve2, v)); vcurvefinal.push_back(curveinterp(&ucurve3, v)); 
	std::vector<Vector> pdPdv = curveinterpwithdu(&vcurvefinal, v);
	std::vector<Vector> pdPdu = curveinterpwithdu(&ucurvefinal, u);
	Vector n = Vector::cross(pdPdu.at(1), pdPdv.at(1));
	n.normalize();
	std::vector<Vector> pn; 
	pn.push_back(pdPdu.at(0));
	pn.push_back(n);
	return pn; //this returns a vector of point and the normal
}
	

void Bezier::uniform_subdivide(Vector **patch, float step){
	int numdiv = (1 + 0.0003) / step;
	//for eatch parametric value of u
	for(int i = 0; i <= numdiv; i++){
		float u = i * step;
		for(int j = 0; j <= numdiv; j++){
			float v = j * step;
			std::vector<Vector> pn = patchinterp(patch, u, v);
			//do something with p and normal
			//says to save.... but in an array?
			v = u + v;
		}
	}
}

void Bezier::adaptive_subdivide(Vector **patch, float error){
	
}

