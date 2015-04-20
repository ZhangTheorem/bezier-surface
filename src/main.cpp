#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef BEZIER_H
#include "bezier.h"
#endif

#ifndef VECTOR_H
#include "vector.h"
#endif

//****************************************************
// Global Variables
//****************************************************

int windowWidth = 700;
int windowHeight = 700;
int windowID;
unsigned int mode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;

float parameter;
bool objInput = false;
float smoothangle = 80;
bool objOutput = false;
bool adaptive = false;
bool curvature = false;
int wiremode = 0;           
int numPatches = 0;
std::vector<Vector**> patches; 
std::vector<Vector*> triangles;
std::vector<Vector*> trinormals;
std::vector<Vector*> triparams;

float lpos[] = { 1000, 1000, 1000, 0 };
double xAngle = 0;
double yAngle = 0;
double zAngle = 180;
double xShift = 0;
double yShift = -1.0;
double zShift = -5.0;
double nearVal = 1.0;
double minZ = -1.0;

//****************************************************
// OpenGL Functions
//****************************************************

void junk() {
    int i;
    i = pthread_getconcurrency();
    i++;
};

void init(){
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    glLoadIdentity();

    int numTriangles = triangles.size();
    for (int i = 0; i < numTriangles; i++) {
        Vector* triangle = triangles.at(i);
        for (int j = 0; j < 3; j++) {
            if (triangle[j].z < minZ) {
                minZ = triangle[j].z;
            }
        }
    }
}

void display() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);    
    glLoadIdentity();
    glTranslatef(xShift, yShift, zShift);
    gluLookAt(0.0, 0.0, minZ - 0.1, 0, 0, -1, 0, 1, 0);
    glRotated(-90, 1, 0, 0);

    int numTriangles = triangles.size();
    Vector* triparam;
    glPushMatrix();
        glRotated(xAngle, 1, 0, 0);
        glRotated(zAngle, 0, 0, 1);
        for (int i = 0; i < numTriangles; i++) {
            Vector* triangle = triangles.at(i);
            Vector* trinormal = trinormals.at(i);
            if (!objInput)
                triparam = triparams.at(i);
            Vector color1 = Vector(0, 0.9, -0.9);
            Vector color2 = Vector(0, 0.9, -0.9);
            Vector color3 = Vector(0, 0.9, -0.9);
            if (curvature) {
                if (objInput) {
                    // stuff
                } else {
                    color1 = Bezier::curve_interpolate(patches.at(triparam[0].z), triparam[0].x, triparam[0].y);
                    Vector::print(color1); std::cout << std::endl;
                    color2 = Bezier::curve_interpolate(patches.at(triparam[1].z), triparam[1].x, triparam[1].y);
                    color3 = Bezier::curve_interpolate(patches.at(triparam[2].z), triparam[2].x, triparam[2].y);
                }
            }
            glBegin(GL_TRIANGLES);
            glColor3f(1 - color1.x, 1 - fabs(color1.y), 1 + color1.z);
            glNormal3f(trinormal[0].x, trinormal[0].y, trinormal[0].z);
            glVertex3f(triangle[0].x, triangle[0].y, triangle[0].z);
            glColor3f(1 - color1.x, 1 - fabs(color1.y), 1 + color1.z);
            glNormal3f(trinormal[1].x, trinormal[1].y, trinormal[1].z);
            glVertex3f(triangle[1].x, triangle[1].y, triangle[1].z);
            glColor3f(1 - color1.x, 1 - fabs(color1.y), 1 + color1.z);
            glNormal3f(trinormal[2].x, trinormal[2].y, trinormal[2].z);
            glVertex3f(triangle[2].x, triangle[2].y, triangle[2].z);
            glEnd();

            if (wiremode == 2) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(1.0, 1.0);
                glBegin(GL_TRIANGLES);
                glColor3f(0.0, 0.0, 0.0);
                glNormal3f(trinormal[0].x, trinormal[0].y, trinormal[0].z);
                glVertex3f(triangle[0].x, triangle[0].y, triangle[0].z);
                glNormal3f(trinormal[1].x, trinormal[1].y, trinormal[1].z);
                glVertex3f(triangle[1].x, triangle[1].y, triangle[1].z);
                glNormal3f(trinormal[2].x, trinormal[2].y, trinormal[2].z);
                glVertex3f(triangle[2].x, triangle[2].y, triangle[2].z);
                glEnd();
                glDisable(GL_POLYGON_OFFSET_FILL);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
        }
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    float width = (w == 0) ? 1.0 : (float) w;
    float height = (h == 0) ? 1.0 : (float) h;

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(fmin(-width/height, -1.0), fmax(width/height, 1.0),
            fmin(-height/width, -1.0), fmax(height/width, 1.0),
            fmax(nearVal, 0.01), 20.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
    int window[4];
    switch (key) {
        case 's':
            GLint model;
            glGetIntegerv(GL_SHADE_MODEL, &model);
            if (model == GL_SMOOTH)
                glShadeModel(GL_FLAT);
            else
                glShadeModel(GL_SMOOTH);
            break;
        case 'w':
            if (wiremode != 1) {
                wiremode = 1;
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                wiremode = 0;
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            break;
        case 'h':
            if (wiremode != 2) {
                wiremode = 2;
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                wiremode = 0;
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            break;
        case '+':
            nearVal += 0.1;
            glGetIntegerv(GL_VIEWPORT, window);
            glutReshapeWindow(window[2], window[3]);
            break;
        case '-':
            nearVal -= 0.1;
            glGetIntegerv(GL_VIEWPORT, window);
            glutReshapeWindow(window[2], window[3]);
            break;
        // case 'c':
        //     if (curvature)
        //         curvature = false;
        //     else
        //         curvature = true;
        //     break;
        case 27:
            glutDestroyWindow(windowID);
            break;
    }

    if (glutGetWindow())
        glutPostRedisplay();
}

void special(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
                xShift -= 0.1;
            } else {
                zAngle -= 5;
            }
            break;
        case GLUT_KEY_RIGHT:
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
                xShift += 0.1;
            } else {
                zAngle += 5;
            }
            break;
        case GLUT_KEY_UP:
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
                yShift += 0.1;
            } else {
                xAngle += 5;
            }
            break;
        case GLUT_KEY_DOWN:
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
                yShift -= 0.1;
            } else {
                xAngle -= 5;
            }
            break;
    }
    glutPostRedisplay();
}

//****************************************************
// Input Parsing & Main
//****************************************************

bool isFloat(char* input) {
    if (!isdigit(input[0])) {
        if (input[0] == '+' || input[0] == '-') {
            if (!isdigit(input[1])) {
                if (input[1] == '.') {
                    if (isdigit(input[2]))
                        return true;
                }
                return false;
            }
            return true;
        } else if (input[0] == '.') {
            if (isdigit(input[1]))
                return true;
        } else if (input[0] == 'e' || input[0] == 'E') {
            if (!isdigit(input[1])) {
                if (input[1] == '+' || input[1] == '-') {
                    if (isdigit(input[2]))
                        return true;
                }
                return false;
            }
            return true;
        }
        return false;
    }
    return true;
}

void parse_bez_input(char* input) {
    int linecount = 0;
    int patchIndex = 0;
    int rowIndex = 0;
    bool setNum = false;
    Vector** patch;
    std::string line;
    std::ifstream file (input);

    if (file.is_open()) {
        std::cout << "Parsing .bez file..." << std::endl;

        while (std::getline(file, line)) {
            linecount++;
            char* tokens = new char[line.length() + 1];
            strcpy(tokens, line.c_str());
            tokens = strtok(tokens, " \n\t\r");

            if (tokens == NULL) {
                delete[] tokens;
                continue;
            }

            if (!setNum) {
                if (!isdigit(tokens[0])) {
                    std::cerr << "Line " << linecount <<
                            " was not formatted correctly." << std::endl;
                    file.close();
                    exit(EXIT_FAILURE);
                }
                numPatches = atoi(tokens);
                setNum = true;
                if (numPatches == 0)
                    std::cout << "Warning: 0 patches indicated." << std::endl;
                if (strtok(NULL, " \n\t\r") != NULL) {
                    std::cerr << "Line " << linecount <<
                            " contains extra values, which were ignored." <<
                            std::endl;
                }
                continue;
            }

            if (patchIndex < numPatches) {
                int i = 0;
                if (rowIndex == 0) {
                    patch = new Vector*[4];
                    for (int r = 0; r < 4; r++) {
                        patch[r] = new Vector[4];
                    }
                }

                while (tokens != NULL) {
                    if (i >= 12) {
                        std::cerr << "Line " << linecount <<
                                " contains extra values, which were ignored." <<
                                std::endl;
                        break;
                    }
                    if (!isFloat(tokens)) {
                        std::cerr << "Line " << linecount <<
                                " was not formatted correctly." << std::endl;
                        file.close();
                        exit(EXIT_FAILURE);
                    }
                    float x = atof(tokens);
                    tokens = strtok(NULL, " \n\t\r");
                    float y = atof(tokens);
                    tokens = strtok(NULL, " \n\t\r");
                    float z = atof(tokens);
                    tokens = strtok(NULL, " \n\t\r");
                    patch[rowIndex][i / 3] = Vector(x, y, z);
                    i+=3;
                }

                if (i < 12) {
                    std::cerr << "Line " << linecount <<
                            " does not contain enough values." << std::endl;
                    file.close();
                    exit(EXIT_FAILURE);
                }

                rowIndex++;
                if (rowIndex == 4) {
                    patches.push_back(patch);
                    patchIndex++;
                    rowIndex = 0;
                }
            }

            delete[] tokens;
        }

        file.close();
        if (patchIndex < numPatches) {
            std::cerr << "File does not contain the indicated number " <<
                    "of patches." << std::endl;
            exit(EXIT_FAILURE);
        }
        std::cout << "Finished parsing .bez file." << std::endl;
    } else {
        std::cerr << "Unable to open file: " << input << std::endl;
        exit(EXIT_FAILURE);
    }
}

void update_normals(std::vector<Vector>* normalList, Vector normal) {
    int numNormals = normalList->size();
    std::vector<int> normnum;
    for (int i = 0; i < numNormals; i++) {
        Vector othernorm = normalList->at(i);
        float costheta = Vector::dot(normal, othernorm) / normal.len() / othernorm.len();
        if (costheta > cos(smoothangle * M_PI / 180.0)) {
            normnum.push_back(i);
        }
    }

    int numFlagged = normnum.size();
    Vector final = normal;
    for (int i = 0; i < numFlagged; i++) {
        final += normalList->at(normnum.at(i));
    }
    final /= (numFlagged + 1);
    for (int i = 0; i < numFlagged; i++) {
        normalList->at(normnum.at(i)) = final;
    }
    normalList->push_back(final);
}

void parse_obj_input(char* input) {
    int linecount = 0;
    std::vector<Vector> vertices;
    std::vector< std::vector<Vector> > normals;
    std::vector<Vector> faces;
    std::string line;
    std::ifstream file (input);

    // Create offset and provide "nonexistent" coordinate
    vertices.push_back(Vector());
    std::vector<Vector> empty;
    normals.push_back(empty);

    if (file.is_open()) {
        std::cout << "Parsing .obj file..." << std::endl;

        while (std::getline(file, line)) {
            linecount++;
            int i = 0;
            float values[3] = {0, 0, 0};
            char* tokens = new char[line.length() + 1];
            strcpy(tokens, line.c_str());
            tokens = strtok(tokens, " \n\t\r");

            if (tokens == NULL) {
                delete[] tokens;
                continue;
            } else if (strcmp(tokens, "v") == 0) {

                tokens = strtok(NULL, " \n\t\r");
                while (tokens != NULL) {
                    if (!isFloat(tokens)) {
                        std::cerr << "Line " << linecount <<
                                " was not formatted correctly." << std::endl;
                        file.close();
                        exit(EXIT_FAILURE);
                    }
                    if (i < 3)
                        values[i] = atof(tokens);
                    i++;
                    tokens = strtok(NULL, " \n\t\r");
                }

                if (i > 3) {
                    std::cerr << "Line " << linecount <<
                            " contains extra values, which were ignored." <<
                            std::endl;
                } else if (i < 3) {
                    std::cerr << "Line " << linecount <<
                            " does not contain enough values." << std::endl;
                    file.close();
                    exit(EXIT_FAILURE);
                }

                vertices.push_back(Vector(values[0], values[1], values[2]));
                std::vector<Vector> normalList;
                normals.push_back(normalList); // To be calculated later

            } else if (strcmp(tokens, "f") == 0) {

                tokens = strtok(NULL, " \n\t\r");
                while (tokens != NULL) {
                    if (!isdigit(tokens[0])) {
                        std::cerr << "Line " << linecount <<
                                " was not formatted correctly." << std::endl;
                        file.close();
                        exit(EXIT_FAILURE);
                    }
                    if (i < 3)
                        values[i] = atof(tokens);
                    i++;
                    tokens = strtok(NULL, " \n\t\r");
                }

                if (i > 3) {
                    std::cerr << "Line " << linecount <<
                            " contains extra values, which were ignored." <<
                            std::endl;
                } else if (i < 3) {
                    std::cerr << "Line " << linecount <<
                            " does not contain enough values." << std::endl;
                    file.close();
                    exit(EXIT_FAILURE);
                }

                try {
                    Vector v1 = vertices.at((int) values[0]);
                    Vector v2 = vertices.at((int) values[1]);
                    Vector v3 = vertices.at((int) values[2]);
                    Vector *tri = new Vector[3];
                    tri[0] = v1; tri[1] = v2; tri[2] = v3;
                    triangles.push_back(tri);

                    Vector normal = Vector::cross(v2 - v1, v3 - v1).normalize();
                    update_normals(&(normals.at((int) values[0])), normal);
                    update_normals(&(normals.at((int) values[1])), normal);
                    update_normals(&(normals.at((int) values[2])), normal);
                    faces.push_back(Vector(values[0], values[1], values[2]));
                } catch (const std::out_of_range& e) {
                    std::cerr << "Line " << linecount <<
                            " does not contain valid parameters." << std::endl;
                    file.close();
                    exit(EXIT_FAILURE);
                }

            } else if (tokens[0] == '#') {
                delete[] tokens;
                continue;
            } else {
                std::cerr << "Command \"" << tokens << "\" of line " <<
                        linecount << " unrecognized." << std::endl;
                file.close();
                exit(EXIT_FAILURE);
            }

            delete[] tokens;
        }

        file.close();
        int numFaces = faces.size();
        for (int i = 0; i < numFaces; i++) {
            Vector vertnum = faces.at(i);
            Vector* norm = new Vector[3];
            norm[0] = normals.at(vertnum.x).at(0);
            norm[1] = normals.at(vertnum.y).at(0);
            norm[2] = normals.at(vertnum.z).at(0);
            normals.at(vertnum.x).erase(normals.at(vertnum.x).begin());
            normals.at(vertnum.y).erase(normals.at(vertnum.y).begin());
            normals.at(vertnum.z).erase(normals.at(vertnum.z).begin());
            trinormals.push_back(norm);
        }
        std::cout << "Finished parsing .obj file." << std::endl;
    } else {
        std::cerr << "Unable to open file: " << input << std::endl;
        exit(EXIT_FAILURE);
    }
}

void obj_output(char* output) {
    std::ofstream file (output);
    if (file.is_open()) {
        int numTriangles = triangles.size();
        for (int i = 0; i < numTriangles; i++) {
            Vector* triangle = triangles.at(i);
            file << "v " << triangle[0].x << " " << triangle[0].y << " " << triangle[0].z << "\n";
            file << "v " << triangle[1].x << " " << triangle[1].y << " " << triangle[1].z << "\n";
            file << "v " << triangle[2].x << " " << triangle[2].y << " " << triangle[2].z << "\n";
        }
        file << "\n";
        for (int i = 0; i < numTriangles; i++) {
            file << "f " << i*3 + 1 << " " << i*3 + 2 << " " << i*3 + 3 << "\n";
        }
        file << "\n";
        file.close();
    } else {
        std::cerr << "Unable to open file: " << output << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Not enough input parameters." << std::endl;
        exit(EXIT_FAILURE);
    }

    char* ext = strpbrk(argv[1], ".");
    if (ext == NULL) {
        std::cerr << "File format not recognized." << std::endl;
        exit(EXIT_FAILURE);
    } else if (strcmp(ext, ".obj") == 0) {
        objInput = true;
        parse_obj_input(argv[1]);
    } else if (strcmp(ext, ".bez") == 0) {
        parse_bez_input(argv[1]);
    } else {
        std::cerr << "File format not recognized." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!objInput && argc < 3) {
        std::cerr << "Not enough input parameters." << std::endl;
        exit(EXIT_FAILURE);
    }

    int argIndex;

    if (!objInput) {
        if (!isFloat(argv[2])) {
            std::cerr << argv[2] << " is not a valid parameter." << std::endl;
            exit(EXIT_FAILURE);
        }
        parameter = strtof(argv[2], NULL);
        argIndex = 3;
    } else {
        argIndex = 2;
    }

    char* output;

    while (argIndex < argc) {
        if (strcmp(argv[argIndex], "-a") == 0) {
            adaptive = true;
        } else if (strcmp(argv[argIndex], "-o") == 0) {
            argIndex++;
            if (objOutput) {
                std::cout << "Warning: More than one output file indicated. " <<
                        "Only the last will be written to." << std::endl;
            }
            objOutput = true;
            if (argIndex >= argc) {
                std::cerr << "No output file name provided." << std::endl;
                exit(EXIT_FAILURE);
            }
            ext = strpbrk(argv[argIndex], ".");
            if (ext == NULL || strcmp(ext, ".obj") != 0) {
                std::cerr << "Output file should have extension .obj" << std::endl;
                exit(EXIT_FAILURE);
            }
            output = argv[argIndex];
        } else if (strcmp(argv[argIndex], "-s") == 0) {
            argIndex++;
            if (!objInput) {
                std::cout << "Warning: Setting smooth angle without .obj file " <<
                        "has no effect." << std::endl;
            }
            if (argIndex >= argc) {
                std::cerr << "No smooth angle value provided." << std::endl;
                exit(EXIT_FAILURE);
            }
            if (!isFloat(argv[argIndex])) {
                std::cerr << "Smooth angle value not formatted correctly." << std::endl;
                exit(EXIT_FAILURE);
            }
            smoothangle = strtof(argv[argIndex], NULL);
        } else {
            std::cerr << "Unrecognized command: " << argv[argIndex] << std::endl;
            exit(EXIT_FAILURE);
        }
        argIndex++;
    }

    if (adaptive) {
        for (int i = 0; i < numPatches; i++) {
            Bezier::adaptive_subdivide(patches.at(i), i, parameter, &triangles, &trinormals, &triparams);
        }
    } else if (!objInput) {
        for (int i = 0; i < numPatches; i++) {
            Bezier::uniform_subdivide(patches.at(i), i, parameter, &triangles, &trinormals, &triparams);
        }
    }

    if (objOutput) {
        obj_output(output);
        return 0;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(mode);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    windowID = glutCreateWindow(argv[0]);

    init();
    glutIdleFunc(glutPostRedisplay);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);    

    glutMainLoop();

    return 0;
}
