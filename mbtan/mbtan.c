#include <stdio.h>
#include <GL/glut.h>

void on_display(void){
    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3f(1, 0, 0);
    glBegin(GL_POLYGON);
        glVertex3f(0.1, -0.1, 0);
        glVertex3f(-0.1, -0.1, 0);
        glVertex3f(-0.1, 0.1, 0);
        glVertex3f(0.1, 0.1, 0);
    glEnd();
    
    glutSwapBuffers();
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    
    glutDisplayFunc(on_display);
    
    glClearColor(0,0,0, 0);
    
    glutMainLoop();
    
    return 0;
}