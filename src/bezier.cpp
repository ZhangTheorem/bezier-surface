#include "bezier.h"

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define abst(x) ((x)<0 ? -(x) : (x))

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

    Vector ucurve[4];
    Vector vcurve[4];

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
            Vector dPdu = curve_derivative(ucurve[0], ucurve[1], ucurve[2], ucurve[3], u);
            Vector dPdv = curve_derivative(vcurve[0], vcurve[1], vcurve[2], vcurve[3], v);
            Vector norm = Vector::cross(dPdu, dPdv);
            if(norm.len() < 0.0001){
                dPdu = curve_derivative(ucurve[0], ucurve[1], ucurve[2], ucurve[3], u + 0.1f);
                dPdv = curve_derivative(vcurve[0], vcurve[1], vcurve[2], vcurve[3], v + 0.1f);
                norm = patchInterpolate(patch, u + 0.1, v + 0.1, true);
            }
            surface[i][j] = bernstein_basis(ucurve[0], ucurve[1], ucurve[2], ucurve[3], u);
            normal[i][j] = norm.normalize();
        }
    }
    surfaces->push_back(surface);
    normals->push_back(normal);
}

Vector Bezier::patchInterpolate(Vector** patch, float u, float v, bool normal){
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
    if(normal){
        Vector dPdu = curve_derivative(ucurve[0], ucurve[1], ucurve[2], ucurve[3], u);
        Vector dPdv = curve_derivative(vcurve[0], vcurve[1], vcurve[2], vcurve[3], v);
        Vector norm = Vector::cross(dPdu, dPdv);
        if(norm.len() < 0.0001){
            dPdu = curve_derivative(ucurve[0], ucurve[1], ucurve[2], ucurve[3], u + 0.1f);
            dPdv = curve_derivative(vcurve[0], vcurve[1], vcurve[2], vcurve[3], v + 0.1f);
            norm = patchInterpolate(patch, u + 0.1, v + 0.1, true);
        }
        return norm.normalize();
    }
    else{
        Vector point = bernstein_basis(ucurve[0], ucurve[1], ucurve[2], ucurve[3], u);
    
        return point;
    }
    
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

void Bezier::adaptive_subdivide(Vector** patch, float error, std::vector<Vector*>* triangles, std::vector<Vector*>* trinormals){
    std::vector<Vector> top;
    std::vector<Vector> ptop;
    top.push_back(patch[0][0]); top.push_back(patch[3][0]); top.push_back(patch[0][3]);
    ptop.push_back(Vector(0,0,0)); ptop.push_back(Vector(0,1,0)); ptop.push_back(Vector(1,0,0));
    triangle_subdivide(patch, error, &top, &ptop, triangles, trinormals);

    std::vector<Vector> bot;
    std::vector<Vector> pbot;
    bot.push_back(patch[0][3]); bot.push_back(patch[3][0]); bot.push_back(patch[3][3]);
    pbot.push_back(Vector(1,0,0)); pbot.push_back(Vector(0,1,0)); pbot.push_back(Vector(1,1,0));
    triangle_subdivide(patch, error, &bot, &pbot, triangles, trinormals);

}

void Bezier::triangle_subdivide(Vector** patch, float error, std::vector<Vector>* triangle, std::vector<Vector>* ptriangle,
                                    std::vector<Vector*>* triangles, std::vector<Vector*>* trinormals){
    //following the picture I drew:
    Vector v1 = triangle->at(0);
    Vector v2 = triangle->at(1);
    Vector v3 = triangle->at(2);
    Vector v1v2 = (v1 + v2)*0.5; //midpoint
    Vector v2v3 = (v2 + v3)*0.5;
    Vector v1v3 = (v3 + v1)*0.5;
    Vector pv1 = ptriangle->at(0);
    Vector pv2 = ptriangle->at(1);
    Vector pv3 = ptriangle->at(2);
    float p12u = (pv1.x + pv2.x)*0.5; //midpoint of u
    float p12v = (pv1.y + pv2.y)*0.5; //midpoint of v
    Vector pv1v2 = patchInterpolate(patch, p12u, p12v, false);
    float p23u = (pv2.x + pv3.x)*0.5; //midpoint of u
    float p23v = (pv2.y + pv3.y)*0.5; //midpoint of v
    Vector pv2v3 = patchInterpolate(patch, p23u, p23v, false);
    float p13u = (pv3.x + pv1.x)*0.5; //midpoint of u
    float p13v = (pv3.y + pv1.y)*0.5; //midpoint of v
    Vector pv1v3 = patchInterpolate(patch, p13u, p13v, false);

    Vector p12 = Vector(p12u, p12v, 0);
    Vector p23 = Vector(p23u, p23v, 0);
    Vector p13 = Vector(p13u, p13v, 0);
    
    bool e1 = true;
    bool e2 = true;
    bool e3 = true;
    if((pv1v2 - v1v2).len() >= error){
        e1 = false;
    }

    if((pv2v3 - v2v3).len() >= error){
        e2 = false;
    }

    if((pv1v3 - v1v3).len() >= error){
        e3 = false;
    }
    
    if (e1 && e2 && e3){
        Vector *tri = new Vector[3];
        Vector *norm = new Vector[3];
        tri[0] = v1;
        tri[1] = v2;
        tri[2] = v3;
        triangles->push_back(tri);
        norm[0] = patchInterpolate(patch, pv1.x, pv1.y, true);
        norm[1] = patchInterpolate(patch, pv2.x, pv2.y, true);
        norm[2] = patchInterpolate(patch, pv3.x, pv3.y, true);
        trinormals->push_back(norm);
        return;
    }
    else if(!e1 && e2 && e3){
        std::vector<Vector> top;
        std::vector<Vector> ptop;
        top.push_back(v1);
        top.push_back(v3);
        top.push_back(pv1v2);
        ptop.push_back(pv1);
        ptop.push_back(pv3);
        ptop.push_back(p12);
        triangle_subdivide(patch, error, &top, &ptop, triangles, trinormals);

        std::vector<Vector> bot;
        std::vector<Vector> pbot;
        bot.push_back(v2);
        bot.push_back(v3);
        bot.push_back(pv1v2);
        pbot.push_back(pv2);
        pbot.push_back(pv3);
        pbot.push_back(p12);
        triangle_subdivide(patch, error, &bot, &pbot, triangles, trinormals);

    }
    else if(e1 && !e2 && e3){
        std::vector<Vector> top;
        std::vector<Vector> ptop;
        top.push_back(v1);
        top.push_back(v2);
        top.push_back(pv2v3);
        ptop.push_back(pv1);
        ptop.push_back(pv2);
        ptop.push_back(p23);
        triangle_subdivide(patch, error, &top, &ptop, triangles, trinormals);

        std::vector<Vector> bot;
        std::vector<Vector> pbot;
        bot.push_back(v1);
        bot.push_back(pv2v3);
        bot.push_back(v3);
        pbot.push_back(pv1);
        pbot.push_back(p23);
        pbot.push_back(pv3);
        triangle_subdivide(patch, error, &bot, &pbot, triangles, trinormals);
    }
    else if(e1 && e2 && !e3){
        std::vector<Vector> top;
        std::vector<Vector> ptop;
        top.push_back(v1);
        top.push_back(pv1v3);
        top.push_back(v2);
        ptop.push_back(pv1);
        ptop.push_back(p13);
        ptop.push_back(pv2);
        triangle_subdivide(patch, error, &top, &ptop, triangles, trinormals);

        std::vector<Vector> bot;
        std::vector<Vector> pbot;
        bot.push_back(v2);
        bot.push_back(pv1v3);
        bot.push_back(v3);
        pbot.push_back(pv2);
        pbot.push_back(p13);
        pbot.push_back(pv3);
        triangle_subdivide(patch, error, &bot, &pbot, triangles, trinormals);
    }
    else if(!e1 && !e2 && e3){
        std::vector<Vector> top;
        std::vector<Vector> ptop;
        top.push_back(v1);
        top.push_back(pv1v2);
        top.push_back(pv2v3);
        ptop.push_back(pv1);
        ptop.push_back(p12);
        ptop.push_back(p23);
        triangle_subdivide(patch, error, &top, &ptop, triangles, trinormals);


        std::vector<Vector> mid;
        std::vector<Vector> pmid;
        mid.push_back(pv1v2);
        mid.push_back(v2);
        mid.push_back(pv2v3);
        pmid.push_back(p12);
        pmid.push_back(pv2);
        pmid.push_back(p23);
        triangle_subdivide(patch, error, &mid, &pmid, triangles, trinormals);

        std::vector<Vector> bot;
        std::vector<Vector> pbot;
        bot.push_back(v1);
        bot.push_back(pv2v3);
        bot.push_back(v3);
        pbot.push_back(pv1);
        pbot.push_back(p23);
        pbot.push_back(pv3);
        triangle_subdivide(patch, error, &bot, &pbot, triangles, trinormals);
    }
    else if(e1 && !e2 && !e3){
        std::vector<Vector> top;
        std::vector<Vector> ptop;
        top.push_back(pv1v3);
        top.push_back(pv2v3);
        top.push_back(v3);
        ptop.push_back(p13);
        ptop.push_back(p23);
        ptop.push_back(pv3);
        triangle_subdivide(patch, error, &top, &ptop, triangles, trinormals);

        std::vector<Vector> mid;
        std::vector<Vector> pmid;
        mid.push_back(v1);
        mid.push_back(v2);
        mid.push_back(pv1v3);
        pmid.push_back(pv1);
        pmid.push_back(pv2);
        pmid.push_back(p13);
        triangle_subdivide(patch, error, &mid, &pmid, triangles, trinormals);

        std::vector<Vector> bot;
        std::vector<Vector> pbot;
        bot.push_back(pv1v3);
        bot.push_back(v2);
        bot.push_back(pv2v3);
        pbot.push_back(p13);
        pbot.push_back(pv2);
        pbot.push_back(p23);
        triangle_subdivide(patch, error, &bot, &pbot, triangles, trinormals);
    }
    else if(!e1 && e2 && !e3){
        std::vector<Vector> top;
        std::vector<Vector> ptop;
        top.push_back(v1);
        top.push_back(pv1v2);
        top.push_back(pv1v3);
        ptop.push_back(pv1);
        ptop.push_back(p12);
        ptop.push_back(p13);
        triangle_subdivide(patch, error, &top, &ptop, triangles, trinormals);

        std::vector<Vector> mid;
        std::vector<Vector> pmid;
        mid.push_back(pv1v2);
        mid.push_back(v3);
        mid.push_back(pv1v3);
        pmid.push_back(p12);
        pmid.push_back(pv3);
        pmid.push_back(p13);
        triangle_subdivide(patch, error, &mid, &pmid, triangles, trinormals);

        std::vector<Vector> bot;
        std::vector<Vector> pbot;
        bot.push_back(pv1v2);
        bot.push_back(v2);
        bot.push_back(v3);
        pbot.push_back(p12);
        pbot.push_back(pv2);
        pbot.push_back(pv3);
        triangle_subdivide(patch, error, &bot, &pbot, triangles, trinormals);
    }
    else{
        std::vector<Vector> top;
        std::vector<Vector> ptop;
        top.push_back(v1);
        top.push_back(pv1v2);
        top.push_back(pv1v3);
        ptop.push_back(pv1);
        ptop.push_back(p12);
        ptop.push_back(p13);
        triangle_subdivide(patch, error, &top, &ptop, triangles, trinormals);

        std::vector<Vector> mid;
        std::vector<Vector> pmid;
        mid.push_back(pv1v2);
        mid.push_back(v2);
        mid.push_back(pv2v3);
        pmid.push_back(p12);
        pmid.push_back(pv2);
        pmid.push_back(p23);
        triangle_subdivide(patch, error, &mid, &pmid, triangles, trinormals);

        std::vector<Vector> jung;
        std::vector<Vector> pjung;
        jung.push_back(pv1v2);
        jung.push_back(pv2v3);
        jung.push_back(pv1v3);
        pjung.push_back(p12);
        pjung.push_back(p23);
        pjung.push_back(p13);
        triangle_subdivide(patch, error, &jung, &pjung, triangles, trinormals);

        std::vector<Vector> bot;
        std::vector<Vector> pbot;
        bot.push_back(pv2v3);
        bot.push_back(v3);
        bot.push_back(pv1v3);
        pbot.push_back(p23);
        pbot.push_back(pv3);
        pbot.push_back(p13);
        triangle_subdivide(patch, error, &bot, &pbot, triangles, trinormals);
    }

}