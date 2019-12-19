#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "drawing.h"
#include "types.h"

#define TIMER_ID 0
#define TIMER_INTERVAL 20

Terrain terrain; 

static int animation_ongoing = 0;

static void on_display(void){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.7,0.7,0.7,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,22,21,
              0, 0, 0,
              0, 1, 0);
    
    //draw_coord_sys();
    
    draw_terrain(terrain);
    draw_ball();
    
    draw_cubes();

    glutSwapBuffers();
}

static void on_timer(int value){
    if(value != TIMER_ID)
        return;

    animation_ongoing = timer_ball(terrain);



    if(animation_ongoing)
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
}

static void on_keyboard(unsigned char key, int x, int y){
    switch(key){
        case 27:
            exit(0);
            break;
        case ' ':
            if(!animation_ongoing){
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                animation_ongoing = 1;
            }
        case 'p':
            animation_ongoing = 0;
            break;
        case 'b':
            if(animation_ongoing == 0){
                animation_ongoing = 1;
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
            }
    }
}

static void on_reshape(int width, int height){
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float)width/height, 1, 1000);
    glutFullScreen();
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(800,600);
    glutInitWindowPosition(0,0);

    glutCreateWindow("MB-tan");

    initialize(&terrain);

    glutDisplayFunc(on_display);
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);

    glEnable(GL_COLOR_MATERIAL);

    //glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    glEnable(GL_DEPTH_TEST);

    //Bez ovoga nece da se iscrta scena.
    glEnable(GL_NORMALIZE);
    glutMainLoop();

    return 0;
}