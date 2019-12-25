#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "drawing.h"
#include "types.h"

#define TIMER_ID 0
#define TIMER_INTERVAL 1

Terrain terrain; 

static int animation_ongoing = 0;
static int direction = 0;
static float animation_parametar = 0;
static float animation_parametar2 = 0;
static float animation_ongoing2 = 0;

//Kretanje loptice.
static void on_timer(int value){
    if(value != TIMER_ID)
        return;

    animation_ongoing = timer_ball(terrain, animation_ongoing);

    if(animation_ongoing)
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
}

//Kretanje linije kojom korisnik na pocetku odredjuje u kom ce pravcu ici loptica.
static void on_timer2(int value){
    if(value != TIMER_ID)
        return;
    
    timer_direction_vector(direction);

    glutPostRedisplay();
}

//Spustanje terena pre pocetka igre.
static void on_timer3(int value){
    if(value != TIMER_ID)
        return;

    animation_parametar += 0.5;

    if(animation_ongoing2)
        glutTimerFunc(TIMER_INTERVAL, on_timer3, TIMER_ID);

    if(animation_parametar >= 100)
        animation_ongoing2 = 0;

    glutPostRedisplay();
}

//Povlacenje terena nakon sto je igra zavrsena.
static void on_timer4(int value){
    if(value != TIMER_ID)
        return;

    animation_parametar2 += 0.5;

    if(animation_ongoing2)
        glutTimerFunc(TIMER_INTERVAL, on_timer4, TIMER_ID);

    if(animation_parametar2 >= 100)
        animation_ongoing2 = 0;

    glutPostRedisplay();
}

static void on_keyboard(unsigned char key, int x, int y){
    switch(key){
        case 27:
            exit(0);
            break;
        //Ispaljivanje loptice.
        case ' ':
            if(!animation_ongoing && animation_parametar >= 100 && !animation_ongoing2){
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                animation_ongoing = 1;
            }
            break;
        //Reset.
        case 'r':
        case 'R':
            animation_ongoing2 = 0;
            animation_ongoing = 0;
            animation_parametar = 0;
            animation_parametar2 = 0;
            initialize(&terrain);
            glutPostRedisplay();
            break;
        //Pomeramo vektor pravca u desno.
        case 'd':
        case 'D':
            direction = 1;
            if(animation_ongoing == 0 && animation_parametar >= 100 && !animation_ongoing2)
                glutTimerFunc(TIMER_INTERVAL, on_timer2, TIMER_ID);
            break;
        //Pomeramo vektor pravca u levo.
        case 'a':
        case 'A':
            direction = 2;
            if(animation_ongoing == 0 && animation_parametar >= 100 && !animation_ongoing2)
                glutTimerFunc(TIMER_INTERVAL, on_timer2, TIMER_ID);
            break;
        //Aktiviramo spustanje terena.
        case 'g':
        case 'G':
            if(animation_ongoing2 == 0){
                animation_ongoing2 = 1;
                glutTimerFunc(TIMER_INTERVAL, on_timer3, TIMER_ID);
            }
            break;
    }
}

static void on_reshape(int width, int height){
    //Podesavamo viewport.
    glViewport(0,0,width,height);
    
    //Podesavamo projekciju.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float)width/height, 1, 1000);
    glutFullScreen();
}

static void on_display(void){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2,0.2,0.2,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,22,21,
              0, 0, 0,
              0, 1, 0);
    
    //draw_coord_sys();
    
    write_instructions(&animation_parametar, &animation_parametar2);

    //Ukoliko je kocka dosla do granicne linije igra se prekida i povratna vrednost funkcije
    //draw_backround je 1.
    int ind = draw_backround();

    //Slucaj kada nije ispunjen uslov za kraj igre.
    if(!ind){
        float terrain_parametar = animation_parametar/100 < 1 ? animation_parametar/100 : 1;
        glPushMatrix();
            glTranslatef(0,25 - 25*terrain_parametar,0);
            draw_terrain(terrain);
            draw_ball();
            draw_cubes();
            draw_semaphore();
            if(!animation_ongoing)
                draw_direction_vector();
        glPopMatrix();
    }
    //Slucaj kada je ispunjen uslov za kraj igre.
    else{
        float terrain_parametar = animation_parametar2/100 < 1 ? animation_parametar2/100 : 1;
        if(!animation_ongoing2){
            glutTimerFunc(TIMER_INTERVAL,on_timer4,TIMER_ID);
            animation_ongoing2 = 1;
        }
        glPushMatrix();
            glTranslatef(0,0,0-70*terrain_parametar);
            draw_terrain(terrain);
            draw_ball();
            draw_cubes();
            draw_semaphore();
            if(!animation_ongoing)
                draw_direction_vector();
        glPopMatrix();
    }
        

    glutSwapBuffers();
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

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_NORMALIZE);
    glutMainLoop();

    return 0;
}