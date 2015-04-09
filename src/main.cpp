#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>

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
unsigned int mode = GLUT_SINGLE | GLUT_RGB;
float parameter;

double xAngle = 0;
double yAngle = 0;
double zAngle = 0;

GLfloat ctrlpoints[5][3] = {
        { -4.0, -4.0, 0.0}, { -2.0, 4.0, 0.0}, 
        {2.0, -4.0, 0.0}, {4.0, 4.0, 0.0}, {-4.5, 3.0, 0.0}};

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
// Input Parsing
//****************************************************

void parse_input(char* input) {

    // Read from the file
    FILE* file = fopen(input, "r");

    // Error if file does not exist
    if (file == NULL) {
        std::cerr << "File does not exist: " << input << std::endl;
        exit(EXIT_FAILURE);
    }

    // // Declarations
    // char line[256];
    // char *tokenised_line;
    // int linecount = 1;

    // // Print out each line
    // while (fgets(line, sizeof(line), file)) {

    // // Tokenise the line, starting at header
    // tokenised_line = strtok(line, " \n\t\r");

    // if (tokenised_line == NULL) {
    //     // Do nothing
    // } else if (strcmp(tokenised_line, "cam") == 0) {
    // } else {
    //     std::cerr << "Command \"" << tokenised_line << "\" unrecognized. Line " <<
    //         linecount << " ignored." << std::endl;
    // }

    // linecount++;

    // }

    // Close the file
    fclose(file);

}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        //std::cerr << "Not enought input parameters" << std::endl;
        // exit(EXIT_FAILURE);
    }
    // parse_input(argv[1]);
    // parameter = strtof(argv[2], NULL);

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
