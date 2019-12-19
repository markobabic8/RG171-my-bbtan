#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "types.h"
#include <time.h>

#define BROJ_KOCKICA_HORIZONTALNO 7
#define BROJ_KOCKICA_VERTIKALNO 5
//Makro za odredjivanje veceg od dva elementa.
#define max(A, B) (((A) > (B)) ? (A) : (B))

//Koordinate loptice.
static float x_curr;
static float z_curr;

//Pomeraj loptice.
static float v_x;
static float v_z;

static float x_start;
static float z_start;
static float y_kockica;
static float x_kockica[BROJ_KOCKICA_VERTIKALNO][BROJ_KOCKICA_HORIZONTALNO];
static float z_kockica[BROJ_KOCKICA_VERTIKALNO];

float function_plane(float u, float v){
    return 0;
}

void set_vertex_and_normal(float u, float v, float (*function)(float, float)){
    float diff_u, diff_v;

    diff_u = (function(u+1, v) - function(u-1, v)) / 2.0;
    diff_v = (function(u, v+1) - function(u, v-1)) / 2.0;

    glNormal3f(-diff_u, 1, -diff_v);

    glVertex3f(u, function(u, v), v);
}

//Iscrtavanje ravni terana.
void draw_plane(Terrain terrain){
    int u, v;

    glPushMatrix();
    {
        GLfloat ambient_coeffs[] = { 0.0, 0.0, 0.0, 1 };
        GLfloat diffuse_coeffs[] = { 0, 0, 0.0, 1 };
        GLfloat specular_coeffs[] = { 0, 0, 0, 1 };
    
        GLfloat shininess = 30;

        //Podesavaju se parametri materijala. 
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    
        glShadeModel(GL_SMOOTH);

        glColor3f(0,0,0);
        //Crtamo funkciju strip po strip.
        for(u = terrain.U_FROM; u < terrain.U_TO; u++){
            glBegin(GL_TRIANGLE_STRIP);
            for(v = terrain.V_FROM; v <= terrain.V_TO; v++){
                set_vertex_and_normal(u, v,function_plane);
                set_vertex_and_normal(u+1, v,function_plane);
            }
            glEnd();
        }
    }
    glPopMatrix();
}

//Inicijalizacija.
void initialize(Terrain *terrain){
    srand(time(NULL));

    //Inicijalizacija promenljivih koje nam pomazu u iscrtavanju terena.
    terrain->U_FROM = -17;
    terrain->V_FROM = -15;
    terrain->U_TO = 17;
    terrain->V_TO = 15;

    for(int i=0; i<BROJ_KOCKICA_VERTIKALNO; i++)
        for(int j = 0; j<BROJ_KOCKICA_HORIZONTALNO; j++){
            x_kockica[i][j] = 0;
            z_kockica[i] = 0;
        }
    
    //Inicijalizacija koordinata loptice.
    //Random x-pozicija na terenu.
    x_curr = terrain->U_FROM + 1 + rand()%(terrain->U_TO - 1 + abs(terrain->U_FROM + 1));
    z_curr = 12.5;
    v_x = 0.5;
    v_z = 0.5;

    x_start = terrain->U_FROM + 3;
    z_start = terrain->V_FROM + 7;

    //Inicijalizacija prve vrste kockica.
    double pom;
    for(int i=0;i<BROJ_KOCKICA_HORIZONTALNO; i++){
        pom = x_start + i*4.3 + i*0.3;
        if(rand()%2){
            x_kockica[0][i] = pom;
        }
    }

    //Inicijalizacija z koordinate kockica.
    for(int i=0;i<BROJ_KOCKICA_VERTIKALNO;i++){
        z_kockica[i] = z_start + i*4.3 + i*0.3;
    }

    //Inicijalizacija svetla i materijala.
    GLfloat light_position[] = {0, 22, 21, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    GLfloat light_ambient[] = { 0, 0, 0, 1 };
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

    GLfloat ambient_coeffs[] = { 0.3, 0.7, 0.3, 1 };
    GLfloat diffuse_coeffs[] = { 1, 1, 1, 1 };
    GLfloat specular_coeffs[] = { 0.1, 0.1, 0.1, 1 };

    GLfloat shininess = 30;

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_FRONT, GL_AMBIENT, light_ambient);
    glLightfv(GL_FRONT, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_FRONT, GL_SPECULAR, light_specular);

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

//Iscrtavanje omotaca terena.
void draw_terrain_broders(Terrain terrain){
    double edge = 1;

    glPushMatrix();
    {
        glColor3f(0.7,0,0);
        //Iscrtavanje gornjeg zida.
        glPushMatrix();
            //Obavezno transliramo jer da nismo translirali iscrtavanje bi pocelo
            //u koordinantnom pocetku.
            glTranslatef((terrain.U_FROM+terrain.U_TO)/2,0,terrain.V_FROM);

            //U_TO - U_FROM je sirina odnosno deo x-ose koji zaklapa ivica.
            //3 je visina odnosno deo y-ose koji zaklapa ivica.
            //1 je debljina odnosno deo z-ose koji zaklapa ivica.
            glScalef(terrain.U_TO-terrain.U_FROM, 3, 1);
            
            glPushMatrix();
                glutSolidCube(edge);
            glPopMatrix();
        glPopMatrix();

        //Iscrtavanje donjeg zida.
        glPushMatrix();
            glTranslatef((terrain.U_FROM+terrain.U_TO)/2,0,terrain.V_TO);
            glScalef(terrain.U_TO-terrain.U_FROM, 3, 1);
            glPushMatrix();
                glutSolidCube(edge);
            glPopMatrix();
        glPopMatrix();

        //Iscrtavanje levog zida.
        glPushMatrix();
            glTranslatef(terrain.U_FROM, 0, 0);
            glScalef(1, 3, terrain.V_TO - terrain.V_FROM);
            glPushMatrix();
                glutSolidCube(edge);
            glPopMatrix();
        glPopMatrix();
        
        //Iscrtavanje desnog zida.
        glPushMatrix();
            glTranslatef(terrain.U_TO, 0, 0);
            glScalef(1, 3, terrain.V_TO - terrain.V_FROM);
            glPushMatrix();
                glutSolidCube(edge);
            glPopMatrix();
        glPopMatrix();
    }
    glPopMatrix();
}

//Granicna linija.
void draw_border_line(Terrain terrain){
    
    glPushMatrix();
        
        glColor3f(0.9,0.9,0.9);

        glLineWidth(8);
        glBegin(GL_LINES);
            glVertex3f(terrain.U_FROM, 0, terrain.V_TO - 2);
            glNormal3f(0,1,0);
            glVertex3f(terrain.U_TO, 0, terrain.V_TO - 2);
            glNormal3f(0,1,0);
        glEnd();

        glLineWidth(1);
    glPopMatrix();
    
}

//Iscrtavanje terena.
void draw_terrain(Terrain terrain){

    draw_terrain_broders(terrain);
    draw_border_line(terrain);
    draw_plane(terrain);
}

//Iscrtavanje koordinatnog sistema.
//Moze biti korisno za orijentaciju u prostoru.
void draw_coord_sys(){
    
    glPushMatrix();
        glColor3f(1,0,0);
        glBegin(GL_LINES);
            glVertex3f(100,0,0);
            glVertex3f(-100,0,0);
        glEnd();
    glPopMatrix();

    glPushMatrix();
        glColor3f(0,1,0);
        glBegin(GL_LINES);
            glVertex3f(0,-100,0);
            glVertex3f(0,100,0);
        glEnd();
    glPopMatrix();

    glPushMatrix();
        glColor3f(0,0,1);
        glBegin(GL_LINES);
            glVertex3f(0,0,100);
            glVertex3f(0,0,-100);
        glEnd();
    glPopMatrix();
}

void draw_ball(){
    glColor3f(1,1,1);
    glPushMatrix();
        glTranslatef(x_curr,0.2, z_curr);
        glutSolidSphere(0.4,50,50);
    glPopMatrix();
}

void timer_lowering(){
    for(int i=BROJ_KOCKICA_VERTIKALNO-1; i>0;i--){
        for(int j=BROJ_KOCKICA_HORIZONTALNO-1; j>=0;j--){
            x_kockica[i][j] = x_kockica[i-1][j];
        }
    }

    for(int i=0;i<BROJ_KOCKICA_HORIZONTALNO;i++)
        x_kockica[0][i] = 0;

    srand(time(NULL));

    double pom;
    for(int i=0;i<BROJ_KOCKICA_HORIZONTALNO; i++){
        pom = x_start + i*4.3 + i*0.3;
        if(rand()%2){
            x_kockica[0][i] = pom;
        }
    }
}

int timer_ball(Terrain terrain){
    z_curr += -v_z;
    if(z_curr >= terrain.V_TO - 2.5 || z_curr <= terrain.V_FROM + 1)
        v_z *= -1;

    x_curr += -v_x;
    if(x_curr >=  terrain.U_TO - 1 || x_curr <= terrain.U_FROM + 1)
        v_x *= -1;

    glutPostRedisplay();

    if(z_curr >= terrain.V_TO - 2.5){
        timer_lowering();
        return 0;
        //play = 0;
    }
    return 1;
}

void draw_cubes(){
    glColor3f(1,0,0);
    for(int i=0;i<BROJ_KOCKICA_VERTIKALNO;i++){
        for(int j=0;j<BROJ_KOCKICA_HORIZONTALNO;j++){
            if(x_kockica[i][j] != 0){
                glPushMatrix();
                    glTranslatef(x_kockica[i][j],0,z_kockica[i]);
                    glutSolidCube(4.3);
                glPopMatrix();
            }
        }
    }
}