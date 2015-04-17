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
bool adaptive = false;
int wiremode = 0;               // filled == 0; wireframe == 1; hidden line == 2;
int numPatches = 0;
int numdiv = 0;
std::vector<Vector**> patches;  // row, column, coord
std::vector<Vector**> surfaces;
std::vector<Vector**> normals;

float lpos[] = { 1000, 1000, 1000, 1 };
double xAngle = 0;
double yAngle = 0;
double zAngle = 180;
double xShift = 0.0;
double yShift = -1.0;
double zShift = -5.0;
double fovyFactor = 1.0;

//****************************************************
// OpenGL Functions
//****************************************************

void init(){
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_POSITION, lpos);

    if (adaptive) {
        // Do adaptive shit
    } else {
        numdiv = ceil(1.0f / parameter) + 1;
        for (int i = 0; i < numPatches; i++) {
            Bezier::uniform_subdivide(patches.at(i), parameter, &surfaces, &normals);
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(xShift, yShift, zShift);
    gluLookAt(0.0, 0.0, -5.0, 0, 0, -1, 0, 1, 0);
    glRotated(-90, 1, 0, 0);

    for (int i = 0; i < numPatches; i++) {
        glPushMatrix();
            glRotated(zAngle, 0, 0, 1);
            glRotated(xAngle, 1, 0, 0);
            glColor3f(1.0, 0.1, 0.1);
            glBegin(GL_QUADS);
            Vector** surface = surfaces.at(i);
            Vector** normal = normals.at(i);
            for (int u = 0; u < numdiv - 1; u++) {
                for (int v = 0; v < numdiv - 1; v++) {
                    glNormal3f(normal[u][v].x, normal[u][v].y, normal[u][v].z);
                    glVertex3f(surface[u][v].x, surface[u][v].y, surface[u][v].z);
                    glNormal3f(normal[u+1][v].x, normal[u+1][v].y, normal[u+1][v].z);
                    glVertex3f(surface[u+1][v].x, surface[u+1][v].y, surface[u+1][v].z);
                    glNormal3f(normal[u+1][v+1].x, normal[u+1][v+1].y, normal[u+1][v+1].z);
                    glVertex3f(surface[u+1][v+1].x, surface[u+1][v+1].y, surface[u+1][v+1].z);
                    glNormal3f(normal[u][v+1].x, normal[u][v+1].y, normal[u][v+1].z);
                    glVertex3f(surface[u][v+1].x, surface[u][v+1].y, surface[u][v+1].z);
                }
            }
            glEnd();
        glPopMatrix();
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0 * fovyFactor, (float) w / (float) h, 0.1f, 100.0f);

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
                glDisable(GL_CULL_FACE);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                wiremode = 0;
                glDisable(GL_CULL_FACE);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            break;
        case 'h':
            if (wiremode != 2) {
                wiremode = 2;
                glEnable(GL_CULL_FACE);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                wiremode = 0;
                glDisable(GL_CULL_FACE);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            break;
        case '+':
            fovyFactor -= 0.1;
            glGetIntegerv(GL_VIEWPORT, window);
            glutReshapeWindow(window[2], window[3]);
            break;
        case '-':
            fovyFactor += 0.1;
            glGetIntegerv(GL_VIEWPORT, window);
            glutReshapeWindow(window[2], window[3]);
            break;
        case 'q':
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

void parse_obj_input(char* input) {
    // Currently does nothing
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Not enough input parameters." << std::endl;
        exit(EXIT_FAILURE);
    }

    char* ext = strpbrk(argv[1], ".");
    if (strcmp(ext, ".obj") == 0) {
        objInput = true;
        parse_obj_input(argv[1]);
    } else if (strcmp(ext, ".bez") == 0) {
        parse_bez_input(argv[1]);
    } else {
        std::cerr << "File format not recognized." << std::endl;
        exit(EXIT_FAILURE);
    }

    parameter = strtof(argv[2], NULL);
    if (argc > 3) {
        int argIndex = 3;
        while (argIndex < argc) {
            if (strcmp(argv[argIndex], "-a") == 0)
                adaptive = true;
            argIndex++;
        }
    }

    // test_patches();

    glutInit(&argc, argv);
    glutInitDisplayMode(mode);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    windowID = glutCreateWindow(argv[0]);

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);

    glutMainLoop();

    return 0;
}

//****************************************************
// Test Functions
//****************************************************

void test_patches() {
    for (int i = 0; i < numPatches; i++) {
        Vector** patch = patches.at(i);
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                Vector::print(patch[r][c]);
                std::cout << "     ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}