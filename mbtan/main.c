#include <stdio.h>
#include <GL/glut.h>
#include <time.h>

#define TIMER_ID 0
#define TIMER_INTERVAL 20
#define GRORNJA_GRANICA 0.8
#define DONJA_GRANICA -0.8

const static float size = 0.1;
static int play = 0;
static int animation_ongoing;

static float x_curr;
static float y_curr;
static float v_x;
static float v_y;

void on_display(void){
    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3f(1,1,1);
    glBegin(GL_POLYGON);
        glVertex2f(x_curr-size/2, y_curr-size/2);
        glVertex2f(x_curr+size/2,y_curr-size/2);
        glVertex2f(x_curr+size/2,y_curr+size/2);
        glVertex2f(x_curr-size/2,y_curr+size/2);
    glEnd();

    glColor3f(1,1,1);
    glBegin(GL_LINES);
        glVertex2f(-1, -0.8);
        glVertex2f(1, -0.8);
    glEnd();

    glColor3f(1,1,1);
    glBegin(GL_LINES);
        glVertex2f(-1, 0.8);
        glVertex2f(1, 0.8);
    glEnd();

    if(play == 0){
        glColor3f(1,1,1);
        glBegin(GL_TRIANGLES);
            glVertex2f(-0.96, 0.96);
            glVertex2f(-0.96, 0.84);
            glVertex2f(-0.80, 0.90);
        glEnd();
    }
    else{
        glLineWidth(15);
        glColor3f(1,1,1);
        glBegin(GL_LINES);
            
            glVertex2f(-0.96,0.96);
            glVertex2f(-0.96, 0.84);
        glEnd();

        glColor3f(1,1,1);
        glBegin(GL_LINES);
            glLineWidth(6);
            glVertex2f(-0.90,0.96);
            glVertex2f(-0.90, 0.84);
        glEnd();
    }

    glColor3f(1, 0, 0);
    glBegin(GL_POLYGON);
        glVertex2f(0.1, -0.1);
        glVertex2f(-0.1, -0.1);
        glVertex2f(-0.1, 0.1);
        glVertex2f(0.1, 0.1);
    glEnd();
    
    glutSwapBuffers();
}

static void on_timer(int value){
    if(value != TIMER_ID)
        return;

    y_curr += v_y;
    if(y_curr >= 0.75 || y_curr <= -0.75)
        v_y *= -1;

    x_curr += v_x;
    if(x_curr <= -(1-size/2) || x_curr >= 1 - size/2)
        v_x *= -1;

    glutPostRedisplay();
    
    if(animation_ongoing)
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
}

void on_keyboard(unsigned char key, int x, int y){
    switch(key){
        case 27:
            exit(0);
            break;
        case ' ':
            if(!animation_ongoing){
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                animation_ongoing = 1;
                play = 1;
            }
    }
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    
    glutKeyboardFunc(on_keyboard);
    glutDisplayFunc(on_display);

    srand(time(NULL));

    x_curr = -(1-size/2) + (2-size)*rand()/(float)RAND_MAX;
    y_curr = -0.75;

    v_x = 0.025;
    v_y = 0.025;

    glClearColor(0,0,0,0);
    glEnable(GL_DEPTH_TEST);

    glutMainLoop();
    
    return 0;
}