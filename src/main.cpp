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
int numPatches = 0;
std::vector<float***> patches; // row, column, coord

float lpos[] = { 1000, 1000, 1000, 1 };
double xAngle = 0;
double yAngle = 0;
double zAngle = 0;

//****************************************************
// OpenGL Functions
//****************************************************

void init(){

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_POSITION, lpos);

}

void display() {

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -4.5);

    glPushMatrix();
        glRotated(yAngle, 0, 1, 0);   //rotate by rotAngle about y-axis
        glRotated(zAngle, 0, 0, 1);
        glScalef(1.0, 1.0, 1.0);
        glColor3f(1.0, 0.1, 0.1);       // redish
        glutSolidTeapot(1);         // draw the teapot
    glPopMatrix();          // restore the modelview matrix

    glutSwapBuffers();          // make the image visible

}

void reshape(int w, int h) {

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (float) w / (float) h, 0.5f, 20.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void keyboard(unsigned char key, int x, int y) {

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
            std::cout << "w" << std::endl;
            break;
        case '+':
            std::cout << "+" << std::endl;
            break;
        case '-':
            std::cout << "-" << std::endl;
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
                std::cout << "shift left" << std::endl;
            } else {
                std::cout << "left" << std::endl;
                yAngle -= 5;
            }
            break;
        case GLUT_KEY_RIGHT:
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
                std::cout << "shift right" << std::endl;
            } else {
                std::cout << "right" << std::endl;
                yAngle += 5;
            }
            break;
        case GLUT_KEY_UP:
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
                std::cout << "shift up" << std::endl;
            } else {
                std::cout << "up" << std::endl;
                zAngle += 5;
            }
            break;
        case GLUT_KEY_DOWN:
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
                std::cout << "shift down" << std::endl;
            } else {
                std::cout << "down" << std::endl;
                zAngle -= 5;
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
    float*** patch;
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
                    patch = new float**[4];
                    for (int r = 0; r < 4; r++) {
                        patch[r] = new float*[4];
                        for (int c = 0; c < 4; c++) {
                            patch[r][c] = new float[3];
                        }
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
                    patch[rowIndex][i / 3][i % 3] = atof(tokens);
                    tokens = strtok(NULL, " \n\t\r");
                    i++;
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
        // exit(EXIT_FAILURE);
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

    // parameter = strtof(argv[2], NULL);
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
    // Currently does nothing
    for (int i = 0; i < numPatches; i++) {
        float*** patch = patches.at(i);
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                for (int j = 0; j < 3; j++) {
                    std::cout << patch[r][c][j] << " ";
                }
                std::cout << "     ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}