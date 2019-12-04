#include <stdio.h>
#include <GL/glut.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#define TIMER_ID 0
#define TIMER_INTERVAL 20
#define GORNJA_GRANICA 0.8
#define DONJA_GRANICA -0.8
#define MAX_BROJ_KVADRATA_U_REDU 7

const static float size = 0.03;
static int play = 0;
static int animation_ongoing = 0;

static float x_curr;
static float y_curr;
static float v_x;
static float v_y;
static float y_pom;

static int n;
static float x_1 =-0.98;
static float x_2 = -0.98 + (2.0 - 0.16)/7.0;
static float y_1 = 0.76 -(2.0 - 0.16)/7.0;
static float y_2 = 0.76 - (2.0-0.16)/7.0 - (2.0-0.16)/7.0;

static int broj_kvadratica = 0;

static float* niz_y;
static float* niz_x;
static int* postoji;

void on_display(void){
    glClear(GL_COLOR_BUFFER_BIT);
    
    glViewport(0,0,600,600);

    glColor3f(0,0,0);
    glBegin(GL_POLYGON);
        glVertex2f(-1,1);
        glVertex2f(-1,-1);
        glVertex2f(1,-1);
        glVertex2f(1,1);
    glEnd();
    
    //Loptica
    if(1){
        GLfloat ambient_coeffs3[] = { 0.8, 0, 0, 1 };
        GLfloat diffuse_coeffs3[] = { 0.8, 0, 0, 1 };
        GLfloat specular_coeffs3[] = { 0.2, 0, 0, 1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_coeffs3);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_coeffs3);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_coeffs3);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30);
        glPushMatrix();
            glColor3f(1,0,0);
            glTranslatef(x_curr,y_curr,0);
            glutSolidSphere(size ,50,50);
        glPopMatrix();
    }

    glColor3f(1,1,1);
    glBegin(GL_LINES);
        glVertex2f(-1, DONJA_GRANICA);
        glVertex2f(1, DONJA_GRANICA);
    glEnd();

    glColor3f(1,1,1);
    glBegin(GL_LINES);
        glVertex2f(-1, GORNJA_GRANICA);
        glVertex2f(1, GORNJA_GRANICA);
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
        glColor3f(1,1,1);
        
        glLineWidth(8);
        
        glBegin(GL_LINES);
            glVertex2f(-0.96,0.96);
            glVertex2f(-0.96, 0.84);
        glEnd();

        glColor3f(1,1,1);
        glBegin(GL_LINES);
            glVertex2f(-0.90,0.96);
            glVertex2f(-0.90, 0.84);
        glEnd();

        glLineWidth(1);
    }
    
    for(int i = 0; i < 2*n; i+=2){
        glColor3f(1, 0, 0);
        glLineWidth(3);
        if(postoji[i/2] == 1){
            glBegin(GL_LINE_LOOP);
                glVertex2f(niz_x[i],niz_y[i]);
                glVertex2f(niz_x[i+1],niz_y[i]);
                glVertex2f(niz_x[i+1],niz_y[i+1]);
                glVertex2f(niz_x[i],niz_y[i+1]);
            glEnd();
        }

        float pomeraj = fabs(x_2 - x_1) + 0.02;

        if(i+3 < 2*n){
            niz_x[i+2] = niz_x[i] + pomeraj;
            niz_x[i+3] = niz_x[i+1] + pomeraj;
            niz_y[i+2] = niz_y[i];
            niz_y[i+3] = niz_y[i+1];
        }
        glLineWidth(1);
    }
    
    if(niz_y[n-1] <= -0.76){
        glColor3f(1,1,1);
        glBegin(GL_POLYGON);
            glVertex2f(-1,1);
            glVertex2f(-1,-1);
            glVertex2f(1,-1);
            glVertex2f(1,1);
        glEnd();
    }

    glutSwapBuffers();
}

static void on_timer4(int value){
    for(int i =0; i<2*n; i++){
        niz_y[i] -= (2.0 - 0.16)/7.0 + 0.02;
    }

    glutPostRedisplay();
}

static void on_timer(int value){
    if(value != TIMER_ID)
        return;

    y_curr += v_y;
    if(y_curr >= 0.76 || y_curr <= -0.76)
        v_y *= -1;

    x_curr += v_x;
    if(x_curr <= -(1-size/2) || x_curr >= 1 - size/2)
        v_x *= -1;

    glutPostRedisplay();
    
    if(y_curr < -0.76){
        animation_ongoing = 0;
        play = 0;
        glutTimerFunc(TIMER_INTERVAL, on_timer4, TIMER_ID);
    }

    if(animation_ongoing)
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
}

static void on_timer1(int value){
    if(value != TIMER_ID)
        return;

    x_curr += 0.01;

    glutPostRedisplay();
}

static void on_timer2(int value){
    if(value != TIMER_ID)
        return;

    x_curr -= 0.01;

    glutPostRedisplay();
}

void on_keyboard(unsigned char key, int x, int y){
    switch(key){
        case 27:
            exit(0);
            break;
        case 's':
            if(animation_ongoing){
                animation_ongoing = 0;
                play = 0;
            }
            break;
        case ' ':
            if(!animation_ongoing){
                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                animation_ongoing = 1;
                play = 1;
            }
            break;
        case 'd':
            y_pom = y_curr;
            if(x_curr<1-0.035 && !animation_ongoing && y_curr == y_pom){
                glutTimerFunc(TIMER_INTERVAL, on_timer1, TIMER_ID);
            }
            break;
        case 'a':
            y_pom = y_curr;
            if(x_curr > -1+0.035 && !animation_ongoing && y_curr == y_pom){
                glutTimerFunc(TIMER_INTERVAL, on_timer2, TIMER_ID);
            }
    }
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    srand(time(NULL));

    n = abs(ceil(rand()%MAX_BROJ_KVADRATA_U_REDU)) + 1;
    broj_kvadratica += n;
    niz_x = malloc(2*n*sizeof(float));
    niz_y = malloc(2*n*sizeof(float));
    postoji = malloc(n*sizeof(int));

    niz_x[0] = x_1;
    niz_x[1] = x_2;
    niz_y[0] = y_1;
    niz_y[1] = y_2;

    int indikator_jedinice = 0;
    for(int i=0; i<n; i++){
        postoji[i] = abs((int)rand()%2);
        if(postoji[i] == 1)
            indikator_jedinice = 1;
    }

    if(indikator_jedinice == 0)
        postoji[abs((int)rand()%n)] = 1;

    glutKeyboardFunc(on_keyboard);
    glutDisplayFunc(on_display);

    x_curr = -(1-size/2) + (2-size)*rand()/(float)RAND_MAX;
    y_curr = -0.77;

    v_x = 0.02;
    v_y = 0.02;

    glClearColor(0.75,0.75,0.75,0.75);
    glEnable(GL_DEPTH_TEST);

    glutMainLoop();
    
    return 0;
}