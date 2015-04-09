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

//****************************************************
// Global Variables
//****************************************************

int windowWidth = 700;
int windowHeight = 700;

int windowID;
unsigned int mode = GLUT_SINGLE | GLUT_RGB;
/**
float parameter;
bool objInput = false;
bool adaptive = false;
int numPatches = 0;
std::vector<float[4][4][3]> patches; // row, column, coord
**/

double xAngle = 0;
double yAngle = 0;
double zAngle = 0;


void init(){
    glEnable(GL_LIGHTING);      // enable lighting
    glEnable(GL_LIGHT0);        // enable
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float lpos[] = { 1000, 1000, 1000, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);

    glShadeModel(GL_FLAT);       // flat shading
    //glShadeModel(GL_SMOOTH);        // smooth shading
}

void display() {
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0,0.0,-4.5);
    glPushMatrix();
    glRotated(yAngle, 0, 1, 0);   //rotate by rotAngle about y-axis
    glRotated(zAngle, 0, 0, 1);
    glScalef(1.0,1.0,1.0);
    glEnable(GL_COLOR_MATERIAL);    // specify object color
    glColor3f(1.0, 0.1, 0.1);       // redish

    glutSolidTeapot(1);         // draw the teapot

    glPopMatrix();          // restore the modelview matrix
    glFlush();              // force OpenGL to render now

    glutSwapBuffers();          // make the image visible
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0,(float)w/(float)h, 0.5f, 20.0f);
    glMatrixMode(GL_MODELVIEW);

}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 's':
            std::cout << "s" << std::endl;
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
            exit(0);
            break;
        case 27:
            glutDestroyWindow(windowID);
            break;
    }
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

/**
void parse_bez_input(char* input) {
    int linecount = 0;
    bool setNum = false;

    std::string line;
    std::ifstream file (input);
    if (file.is_open()) {
        while (std::getline(file, line)) {
            linecount++;
            if (strcmp(line.c_str(), "\n") == 0)
                continue;

            if (!setNum) {
                numPatches = atoi(line.c_str());
                setNum = true;
                continue;
            }

            std::cout << line << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << input << std::endl;
        exit(EXIT_FAILURE);
    }
}

void parse_obj_input(char* input) {
    // Currently does nothing
}
**/

int main(int argc, char *argv[]) {

    if (argc < 3) {
        //std::cerr << "Not enought input parameters" << std::endl;
        // exit(EXIT_FAILURE);
    }
    /**
    char* ext = strpbrk(argv[1], ".");
    if (strcmp(ext, ".obj")) {
        objInput = true;
        parse_obj_input(argv[1]);
    } else {
        parse_bez_input(argv[1]);
    }

    // parameter = strtof(argv[2], NULL);
    if (argc > 3) {
        int argIndex = 3;
        if (strcmp(argv[argIndex], "-a") == 0)
            adaptive = true;
    }
    **/

    glutInit(&argc, argv);
    glutInitDisplayMode(mode);     // set RGB color mode

    glutCreateWindow("test");   // create the window
    glutInitWindowSize(windowWidth, windowHeight);
    init();
    glutDisplayFunc(display);       // call display() to redraw window
    glutKeyboardFunc(keyboard);     // call keyboard() when key is hit
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutMainLoop();         
    return 0; 
    /**
    glutInitDisplayMode(mode);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    **/

}
