#include <iostream>
#include <fstream>

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

GLfloat ctrlpoints[5][3] = {
        { -4.0, -4.0, 0.0}, { -2.0, 4.0, 0.0}, 
        {2.0, -4.0, 0.0}, {4.0, 4.0, 0.0}, {-4.5, 3.0, 0.0}};

void init() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 5, &ctrlpoints[0][0]);
    glEnable(GL_MAP1_VERTEX_3);
}

void display() {
    int i;

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
        for (i = 0; i <= 30; i++) 
            glEvalCoord1f((GLfloat) i/30.0);
    glEnd();
    /* The following code displays the control points as dots. */
    glPointSize(5.0);
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POINTS);
        for (i = 0; i < 5; i++) 
            glVertex3fv(&ctrlpoints[i][0]);
    glEnd();
    glFlush();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho(-5.0, 5.0, -5.0*(GLfloat)h/(GLfloat)w, 
                5.0*(GLfloat)h/(GLfloat)w, -5.0, 5.0);
    else
        glOrtho(-5.0*(GLfloat)w/(GLfloat)h, 
                5.0*(GLfloat)w/(GLfloat)h, -5.0, 5.0, -5.0, 5.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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
        case 27:
            exit(0);
            break;
    }
}

void special(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
                std::cout << "shift left" << std::endl;
            } else {
                std::cout << "left" << std::endl;
            }
            break;
        case GLUT_KEY_RIGHT:
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
                std::cout << "shift right" << std::endl;
            } else {
                std::cout << "right" << std::endl;
            }
            break;
        case GLUT_KEY_UP:
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
                std::cout << "shift up" << std::endl;
            } else {
                std::cout << "up" << std::endl;
            }
            break;
        case GLUT_KEY_DOWN:
            if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
                std::cout << "shift down" << std::endl;
            } else {
                std::cout << "down" << std::endl;
            }
            break;
    }
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
        std::cerr << "Not enought input parameters" << std::endl;
        // exit(EXIT_FAILURE);
    }
    // parse_input(argv[1]);
    // parameter = strtof(argv[2], NULL);

    glutInit(&argc, argv);
    glutInitDisplayMode(mode);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;

}
