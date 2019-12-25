#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "types.h"
#include <math.h>
#include <time.h>
#include "image.h"
#include <string.h>

#define BROJ_KOCKICA_HORIZONTALNO 7
#define BROJ_KOCKICA_VERTIKALNO 5
//Makro za odredjivanje veceg od dva elementa.
#define max(A, B) (((A) > (B)) ? (A) : (B))
#define MAX_LEN 50

static GLuint texture_names[3];
#define FILENAME0 "backround3.bmp"
#define FILENAME1 "zid2.bmp"
#define FILENAME2 "game_over.bmp"

//Koordinate loptice.
static float x_curr;
static float z_curr;

//Vektor pravca ispaljivanja loptice.
static float direction_x;
static float direction_z;

//Pomeraj loptice.
static float v_x;
static float v_z;

static int score;
static int max_score=0;

static float x_start;
static float z_start;
static float y_kockica;
static float x_kockica[BROJ_KOCKICA_VERTIKALNO][BROJ_KOCKICA_HORIZONTALNO];
static float z_kockica[BROJ_KOCKICA_VERTIKALNO];

float function_plane(float u, float v){
    return -0;
}

void set_vertex_and_normal(float u, float v, float (*function)(float, float)){
    float diff_u, diff_v;

    diff_u = (function(u+1, v) - function(u-1, v)) / 2.0;
    diff_v = (function(u, v+1) - function(u, v-1)) / 2.0;

    glNormal3f(-diff_u, 1, -diff_v);

    glVertex3f(u, function(u, v), v);
}

void init_texture(void)
{
    Image * image;
    
    glEnable(GL_TEXTURE_2D);

    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_MODULATE);

    image = image_init(0, 0);

    image_read(image, FILENAME0);

    glGenTextures(3, texture_names);

    glBindTexture(GL_TEXTURE_2D, texture_names[0]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
    /* Iskljucujemo teksturu */
    glBindTexture(GL_TEXTURE_2D, 0);

    /*Ucitavamo drugu strukturu*/
    image_read(image, FILENAME1);

    glBindTexture(GL_TEXTURE_2D, texture_names[1]);
     glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
    /* Oslobadjamo objekat iz memorije */
    
    image_read(image, FILENAME2);

    glBindTexture(GL_TEXTURE_2D, texture_names[2]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
    /* Iskljucujemo teksturu */
    glBindTexture(GL_TEXTURE_2D, 0);

    image_done(image);

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

    init_texture();
    score = 0;

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
    x_curr = terrain->U_FROM + 1.2 + rand()%(terrain->U_TO - 1 + abs(terrain->U_FROM + 1));
    if(x_curr >= terrain->U_TO - 1.2)
        x_curr = terrain->U_TO - 1.2;
    z_curr = 12.5;
    v_x = 0;
    v_z = 0.005;

    float intezitet = sqrt(v_x*v_x + v_z*v_z);

    v_x = v_x / (intezitet*4);
    v_z = v_z / (intezitet*4);

    x_start = terrain->U_FROM + 3;
    z_start = terrain->V_FROM + 7;

    //Konstanta uz v_x i v_z je tu samo da bi linija koja predstavlja vektor pravca bila optimalne duzine.
    direction_x = x_curr-18*v_x;
    direction_z = z_curr-18*v_z;

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

    GLfloat light_position[] = {0, 40, 0, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1 };
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    GLfloat ambient_coeffs[] = { 0.3, 0.7, 0.3, 1 };
    GLfloat diffuse_coeffs[] = { 1, 1, 1, 1 };
    GLfloat specular_coeffs[] = { 0.1, 0.1, 0.1, 1 };

    GLfloat shininess = 30;

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    
}

//Iscrtavanje omotaca terena.
void draw_terrain_broders(Terrain terrain){
    double ivica = 1;

    glPushMatrix();
    {
        //glDisable(GL_LIGHTING);
        glColor3f(0.9,0.9,0.9);

        const int coef = 10;
        
        //Iscrtavanje gornjeg zida.
        glPushMatrix();
            GLfloat light_position[] = {0, 30, 30, 0 };
            glLightfv(GL_LIGHT0, GL_POSITION, light_position);

            GLfloat light_ambient[] = { 0.8, 0.8, 0.8, 1 };
            GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1 };
            GLfloat light_specular[] = { 0.8, 0.8, 0.8, 1 };

            glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
            glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
            //Obavezno transliramo jer da nismo translirali iscrtavanje bi pocelo
            //u koordinantnom pocetku.
            glTranslatef((terrain.U_FROM+terrain.U_TO)/2,0,terrain.V_FROM);

            //U_TO - U_FROM je sirina odnosno deo x-ose koji zaklapa ivica.
            //3 je visina odnosno deo y-ose koji zaklapa ivica.
            //1 je debljina odnosno deo z-ose koji zaklapa ivica.
            //dodajemo + 1 da bismo imali lepe coskove.
            glScalef(terrain.U_TO-terrain.U_FROM+1, 4, 1);
            
            glPushMatrix();
                glBindTexture(GL_TEXTURE_2D, texture_names[1]);
                glBegin(GL_QUADS);
                    glTexCoord2f(0,0);
                    glNormal3f(1,1,2);
                    glVertex3f(-ivica/2, -ivica/2, ivica/2);
                    glTexCoord2f(0, ivica);
                    glNormal3f(2,1,1);
                    glVertex3f(-ivica/2, ivica/2, ivica/2);
                    glTexCoord2f(ivica*coef , ivica);
                    glNormal3f(2,-1,-1);
                    glVertex3f(ivica/2, ivica/2, ivica/2);
                    glTexCoord2f(ivica*coef, 0);
                    glNormal3f(1,2,1);
                    glVertex3f(ivica/2, -ivica/2, ivica/2);         
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                
                glutSolidCube(ivica);
            glPopMatrix();
        glPopMatrix();

        //Iscrtavanje donjeg zida.
        glPushMatrix();
            glTranslatef((terrain.U_FROM+terrain.U_TO)/2,0,terrain.V_TO);
            glScalef(terrain.U_TO-terrain.U_FROM+1, 4, 1);
            glPushMatrix();
                glBindTexture(GL_TEXTURE_2D, texture_names[1]);
                glBegin(GL_QUADS);
                    glTexCoord2f(0,0);
                    glVertex3f(-ivica/2, -ivica/2, ivica/2);
                    glTexCoord2f(0, ivica);
                    glVertex3f(-ivica/2, ivica/2, ivica/2);
                    glTexCoord2f(ivica*coef , ivica);
                    glVertex3f(ivica/2, ivica/2, ivica/2);
                    glTexCoord2f(ivica*coef, 0);
                    glVertex3f(ivica/2, -ivica/2, ivica/2);         
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                glutSolidCube(ivica);
            glPopMatrix();
        glPopMatrix();

        //Iscrtavanje levog zida.
        glPushMatrix();
            glTranslatef(terrain.U_FROM, 0, 0);
            glScalef(1, 4, terrain.V_TO - terrain.V_FROM);
            glPushMatrix();
                glBindTexture(GL_TEXTURE_2D, texture_names[1]);
                glBegin(GL_QUADS);
                    glTexCoord2f(0,0);
                    glVertex3f(ivica/2 + 0.01, -ivica/2, ivica/2);
                    glTexCoord2f(0, ivica);
                    glVertex3f(ivica/2+0.01, ivica/2, ivica/2);
                    glTexCoord2f(ivica*coef , ivica);
                    glVertex3f(ivica/2+0.01, ivica/2, -ivica/2);
                    glTexCoord2f(ivica*coef, 0);
                    glVertex3f(ivica/2+0.01, -ivica/2, -ivica/2);         
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                glutSolidCube(ivica);
            glPopMatrix();
        glPopMatrix();
        
        //Iscrtavanje desnog zida.
        glPushMatrix();
            glTranslatef(terrain.U_TO, 0, 0);
            glScalef(1, 4, terrain.V_TO - terrain.V_FROM);
            glPushMatrix();
                glBindTexture(GL_TEXTURE_2D, texture_names[1]);
                glBegin(GL_QUADS);
                    glTexCoord2f(0,0);
                    glVertex3f(-ivica/2 , -ivica/2, ivica/2);
                    glTexCoord2f(0, ivica);
                    glVertex3f(-ivica/2, ivica/2, ivica/2);
                    glTexCoord2f(ivica*coef , ivica);
                    glVertex3f(-ivica/2, ivica/2, -ivica/2);
                    glTexCoord2f(ivica*coef, 0);
                    glVertex3f(-ivica/2, -ivica/2, -ivica/2);         
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                glutSolidCube(ivica);
            glPopMatrix();
        glPopMatrix();
        glBindTexture(GL_TEXTURE_2D, 0);

        GLfloat light_position1[] = {0, 40, 0, 1 };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position1);

        GLfloat light_ambient1[] = { 0.1, 0.1, 0.1, 1 };
        GLfloat light_diffuse1[] = { 0.7, 0.7, 0.7, 1 };
        GLfloat light_specular1[] = { 0.9, 0.9, 0.9, 1 };

        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient1);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse1);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular1);
    }
    glPopMatrix();
}

//Granicna linija.
void draw_border_line(Terrain terrain){
    
    glPushMatrix();
        
        glColor3f(0.9,0.9,0.9);

        glLineWidth(8);
        glBegin(GL_LINES);
            glVertex3f(terrain.U_FROM+0.8, 0, terrain.V_TO - 2);
            glNormal3f(0,1,0);
            glVertex3f(terrain.U_TO-0.8, 0, terrain.V_TO - 2);
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

static void renderBitmapString(int x, int y,int z,void* font, char *string)
{
    int len; //duzina stringa
    glDisable(GL_LIGHTING); //Privremeno iskljucujemo osvetljenje da bi postavili boju teksta
    glColor3f(1,1,1); //Postavljanje boje teksta
    glRasterPos3f(x,y,z);
    len = strlen(string);
    for (int i = 0; i < len; i++) 
    {
        glutBitmapCharacter(font, string[i]);
    }
    glEnable(GL_LIGHTING); //Ponovo ukljucujemo osvetljenje
}

void draw_score(){
    char word[MAX_LEN];

    if(max_score < score)
        max_score = score;

    sprintf(word, "Best: %d \nNow: %d", max_score, score);
    const int x = -2;
    const int y = 13;
    const int z = 0;
    renderBitmapString(x,y,z,GLUT_BITMAP_TIMES_ROMAN_24,word);
}

void write_instructions(float* animation_parametar, float *animation_parametar2){
    char word[MAX_LEN];

    if(!*animation_parametar){
        sprintf(word, "Start on 'G'.");
        const int x = -3;
        const int y = 10;
        const int z = 0;
    renderBitmapString(x,y,z,GLUT_BITMAP_TIMES_ROMAN_24,word);
    }

    if(*animation_parametar2 >= 100){
        sprintf(word, "Press 'R' for new game.");
        const int x = -5;
        const int y = 10;
        const int z = 0;
        renderBitmapString(x,y,z,GLUT_BITMAP_TIMES_ROMAN_24,word);
    }
}

int timer_ball(Terrain terrain, int animation_ongoing){
    z_curr += -v_z;
    if(z_curr >= terrain.V_TO - 2.5 || z_curr <= terrain.V_FROM + 1)
        v_z *= -1;

    x_curr += -v_x;
    if(x_curr >=  terrain.U_TO - 1 || x_curr <= terrain.U_FROM + 1)
        v_x *= -1;

    if(animation_ongoing == 0){
    if(x_curr >= terrain.U_TO - 1.2)
        x_curr = terrain.U_TO - 1.2;
    else if(x_curr <= terrain.U_FROM + 1.2)
        x_curr = terrain.U_FROM + 1.2;
    }

    int indikator = 0;
    int indikator_z = 0;

    direction_x = x_curr-18*v_x;
    direction_z = z_curr-18*v_z;

    for(int i=0; i<BROJ_KOCKICA_VERTIKALNO; i++){
        for(int j=0; j<BROJ_KOCKICA_HORIZONTALNO; j++){
            if(((x_curr+0.15 >=  x_kockica[i][j] - 4.3/2.0 || x_curr-0.15 >=  x_kockica[i][j] - 4.3/2.0) 
                && (x_curr+0.15 <= x_kockica[i][j] + 4.3/2.0 || x_curr-0.15 <=  x_kockica[i][j] + 4.3/2.0 )) 
                && ((z_curr+0.15 >= z_kockica[i] - 4.3/2.0 || z_curr-0.15 >= z_kockica[i] - 4.3/2.0) 
                && (z_curr-0.15 <= z_kockica[i] + 4.3/2.0 || z_curr-0.15 <= z_kockica[i] + 4.3/2.0))
                && x_kockica[i][j] != 0){

                //Nacin na koji odredjujemo da li je loptica udarila u donju/gornju stranicu kocke.
                //U prvi if ulazimo ako su oba ova uslova ispunjena, znaci ako je loptica pokusala da udje na povrsinu kocke.
                //Loptica moze uci na povrsinu kocke ili sa jedne od bocnih strana ili sa gornje/donje strane.
                //Ukoliko je loptica pokusala da udje sa gornje ili donje strane treba da promenimo z pravac, a inace treba da promenimo
                //x pravc.
                //Ukoliko loptica pokusa da udje na povrsinu kocke sa donje strane ona ce sigurno u nekom trenutku morati da bude jednaka sa
                //z koordinatom donje ivice kocke koja je jednaka z_kockice[i] + 4.3/2. Dakle mi to mozemo staviti u ovaj if ispod, analogna tome
                //je prica i za gornju ivicu kocke.

                //Udarac u donju stranu.
                if(z_curr-0.15 <= z_kockica[i] + 4.3/2 && z_curr+0.15 >= z_kockica[i] + 4.3/2)
                    indikator_z = 1;
                //Udarac u gornju stranu.
                else if(z_curr-0.15 <= z_kockica[i] - 4.3/2 && z_curr+0.15 >= z_kockica[i] - 4.3/2)
                    indikator_z = 1;
                
                indikator = 1;
                x_kockica[i][j] = 0;
                break;
            }
        }
    }

    if(indikator){
        if(indikator_z)
            v_z*=-1;
        else
            v_x*=-1;
    }
    indikator = 0;
    indikator_z = 0;
    
    glutPostRedisplay();

    if(z_curr >= terrain.V_TO - 2.5){
        ++score;
        timer_lowering();
        return 0;
        //play = 0;
    }
    return 1;
}

void timer_direction_vector(Terrain terrain, int direction){
    if(direction == 1){
        v_x -= 0.02;
    }
    else if(direction == 2){
        v_x += 0.02;
    }

    float intezitet = sqrt(v_x*v_x + v_z*v_z);

    v_x = v_x / (intezitet*4);
    v_z = v_z / (intezitet*4);

    

    if(v_x >= 0.22)
        v_x = 0.22;
    else if(v_x <= -0.22)
        v_x = -0.22;
    
    direction_x = x_curr-18*v_x;
    direction_z = z_curr-18*v_z;
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

//Zelim da napravim vektor kojim ce korisnik odredjivati pocetni smer kretanja loptice.
void draw_direction_vector(Terrain terrain){
    glPushMatrix();
        GLdouble cliping_plane0[] = {1,0,0,17};
        GLdouble cliping_plane1[] = {-1,0,0,17};
        
        glEnable(GL_CLIP_PLANE0);
        glClipPlane(GL_CLIP_PLANE0, cliping_plane0);

        glEnable(GL_CLIP_PLANE1);
        glClipPlane(GL_CLIP_PLANE1, cliping_plane1);

        glColor3f(1,0,0);
        glLineWidth(20);

        glBegin(GL_LINES);
            glVertex3f(x_curr, 0, z_curr);
            glNormal3f(0,1,0);
            glVertex3f(direction_x, 0, direction_z);
            glNormal3f(0,1,0);
        glEnd();

        glDisable(GL_CLIP_PLANE0);
        glDisable(GL_CLIP_PLANE1);
    glPopMatrix();
}

void draw_semaphore(){
    draw_score();
    
    glPushMatrix();
        glColor3f(0,0,0);
        glRotatef(-45,1,0,0);
        glTranslatef(0,14,-3);
        glScalef(4,2,0);
        glutSolidCube(2);
    glPopMatrix();
}

int draw_backround(){
    int indikator = 0;

    for(int i = 0; i < BROJ_KOCKICA_HORIZONTALNO; i++){
            if(x_kockica[BROJ_KOCKICA_VERTIKALNO-1][i] != 0){
                indikator = 1;
                break;
            }
    }

    float ivica = 1.0;
    const float coef = 10.0;
    glPushMatrix();
        glDisable(GL_LIGHTING);
        glTranslatef(-7,-20,-20);
        glRotatef(45,-1,0,0);
        glScalef(140,70,0);
        glColor3f(1,1,1);

        if(!indikator)
            glBindTexture(GL_TEXTURE_2D, texture_names[0]);
        else
            glBindTexture(GL_TEXTURE_2D, texture_names[2]);
        glBegin(GL_QUADS);
                    glNormal3f(1,1,2);
                    glTexCoord2f(0,0);
                    glVertex3f(-ivica/2, -ivica/2, ivica/2);
                    glTexCoord2f(0, ivica);
                    glVertex3f(-ivica/2, ivica/2, ivica/2);
                    glTexCoord2f(ivica , ivica);
                    glVertex3f(ivica/2, ivica/2, ivica/2);
                    glTexCoord2f(ivica, 0);
                    glVertex3f(ivica/2, -ivica/2, ivica/2);
        glEnd();
        glBindTexture(GL_TEXTURE_2D,0);
        glutSolidCube(ivica);
        glEnable(GL_LIGHTING);
    glPopMatrix();

    return indikator;
}