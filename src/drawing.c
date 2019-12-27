#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "types.h"
#include <math.h>
#include <time.h>
#include "drawing.h"
#include "image.h"
#include <string.h>

//Broj kocki po vertikali i horizontali terena.
#define BROJ_KOCKICA_HORIZONTALNO 7
#define BROJ_KOCKICA_VERTIKALNO 5

//Makro za odredjivanje veceg od dva elementa.
#define max(A, B) (((A) > (B)) ? (A) : (B))

//Najveca moguca duzia niske.
#define MAX_LEN 50

//Niz i makroi za rad sa teksturama.
static GLuint texture_names[3];
#define FILENAME0 "backround3.bmp"
#define FILENAME1 "zid2.bmp"
#define FILENAME2 "game_over.bmp"

//Koordinate u kojima je loptica bila u trenutku pre nego sto je
//njena pozicija promenjena prilikom pomeranja po terenu.
static float x_prev;
static float z_prev;

//Koordinate loptice.
static float x_curr;
static float z_curr;

//Vektor pravca ispaljivanja loptice.
static float direction_x;
static float direction_z;

//Pomeraj loptice.
static float v_x;
static float v_z;

//Skor i najbolji skor.
static int score;
static int max_score=0;

//Potencijalna pozicija centra prve kocke.
static float x_start;
static float z_start;

//X koordinate svih kocki na terenu.
static float x_kockica[BROJ_KOCKICA_VERTIKALNO][BROJ_KOCKICA_HORIZONTALNO];

//Sve moguce z koordinate kocki na terenu.
static float z_kockica[BROJ_KOCKICA_VERTIKALNO];

//Inicijalizacija tekstura.
void init_texture(void)
{
    Image* image;
    
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

    glBindTexture(GL_TEXTURE_2D, 0);

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

    glBindTexture(GL_TEXTURE_2D, 0);

    image_done(image);

}

//Glavna inicijalizacija.
void initialize(Terrain *terrain){
    srand(time(NULL));

    init_texture();

    //Skor na pocetku postavljamo na 0. Maks skor ne mozemo ovde postaviti na 0
    //jer ne zelimo da se maks skor resetuje pritiskom na dugme 'R' ili 'r'
    //vec zelimo da on bude sacuvan sve do izlaska iz igre.
    score = 0;

    //Inicijalizacija promenljivih koje nam pomazu u iscrtavanju terena.
    terrain->U_FROM = -17;
    terrain->V_FROM = -15;
    terrain->U_TO = 17;
    terrain->V_TO = 15;

    //Inicijalizacija x i z koordinata kocki na terenu.
    for(int i=0; i<BROJ_KOCKICA_VERTIKALNO; i++)
        for(int j = 0; j<BROJ_KOCKICA_HORIZONTALNO; j++){
            x_kockica[i][j] = 0;
            z_kockica[i] = 0;
        }
    
    //Inicijalizacija koordinata loptice. Pocetna x koordinata loptice je slucajni broj.
    x_prev = 0;
    z_prev = 0;
    x_curr = terrain->U_FROM + 1.2 + rand()%(terrain->U_TO - 1 + abs(terrain->U_FROM + 1));
    if(x_curr >= terrain->U_TO - 1.2)
        x_curr = terrain->U_TO - 1.2;
    z_curr = 12.5;

    //Inicijalizacija pomeraja loptice.
    v_x = 0;
    v_z = 0.005;

    //Intenzitet vektora pomeraja.
    float intezitet = sqrt(v_x*v_x + v_z*v_z);

    //Na ovaj nacin smo sigurni da ce vektor pomeraja na pocetku biti jedinicni, sto je veoma bitno.
    v_x = v_x / (intezitet*4);
    v_z = v_z / (intezitet*4);

    //Potencijalna pocetna pozicija x koordinate kockice. 
    x_start = terrain->U_FROM + 3;

    //Z koordinata najvise kocke.
    z_start = terrain->V_FROM + 7;

    //Pomocu ovoga iscrtavamo liniju koja odredjuje vektor pravca.
    //Konstanta uz v_x i v_z je tu samo da bi linija koja predstavlja vektor pravca bila optimalne duzine.
    direction_x = x_curr-18*v_x;
    direction_z = z_curr-18*v_z;

    //Inicijalizacija prve vrste kockica.
    //Osigurac sluzi da obezbedi da ne dodje do situacije u kojoj se ne iscrtava nijedna kockica.
    int osigurac = 0;
    double pom;
    for(int i=0;i<BROJ_KOCKICA_HORIZONTALNO; i++){
        pom = x_start + i*4.3 + i*0.3;
        if(rand()%2){
            x_kockica[0][i] = pom;
            osigurac = 1;
        }
    }

    //Situacija kada je svaki slucajno generisan broj bio paran pa zbog toga nismo
    //iscrtali nijednu kocku, iscrtavamo bar jednu kako bi igra bila zanimljiva.
    if(!osigurac)
        x_kockica[0][0] = x_start;


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

//Funkcija koja nam pomaze u iscrtavanju ravni terena.
float function_plane(float u, float v){
    return 0;
}

//Parametrizacija ravni terena.
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

//Iscrtavanje zidova oko terena zajedno sa teksturama.
void draw_terrain_broders(Terrain terrain){
    double ivica = 1;

    glPushMatrix();
    {
        //glDisable(GL_LIGHTING);
        glColor3f(0.9,0.9,0.9);

        const int coef = 10;
        
        //Iscrtavanje gornjeg zida.
        glPushMatrix();
            //Menjamo ostvetljenje zbog lepseg izgleda terena.
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
            //4 je visina odnosno deo y-ose koji zaklapa ivica.
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

        //Vracamo osvetljenje na staro.
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

//Granicna linija. Od nje krece loptica i ako kocke dodju do nje igra je zavrsena.
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

//Iscrtavanje kompletnog terena.
void draw_terrain(Terrain terrain){
    draw_terrain_broders(terrain);
    draw_border_line(terrain);
    draw_plane(terrain);
}

//Iscrtavanje koordinatnog sistema.
//Moze biti korisno za orijentaciju u prostoru prilikom pravljenja projekta.
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

//Iscrtavanje loptice.
void draw_ball(){
    glColor3f(1,1,1);
    glPushMatrix();
        glTranslatef(x_curr,0.2, z_curr);
        glutSolidSphere(0.4,50,50);
    glPopMatrix();
}

//Iscrtavanje kocki.
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

//Iscrtavanje vektora kojim ce korisnik odredjivati pocetni smer kretanja loptice.
void draw_direction_vector(){
    glPushMatrix();
        //Ovde nam je potrebna kliping ravan. Korisna je u situaciji kad se loptica nalazi uz sam levi ili desni zid i
        //tada, kada pomerimo liniju kojom je odredjen vektor pravca u stranu ka zidu, da nema kliping ravni ona bi
        //izlazila van terena sto ne izgleda kako treba.
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

        //Iskljucujemo kliping ravan.
        glDisable(GL_CLIP_PLANE0);
        glDisable(GL_CLIP_PLANE1);
    glPopMatrix();
}

//Iscrtavanje semafora na kom se nalazi trenutni i najbolji skor od pokretanja igre.
void draw_semaphore(){
    draw_score();
    
    glPushMatrix();
        glColor3f(0,0,0);
        glRotatef(-45,1,0,0);
        glTranslatef(0,14.5,-3);
        glScalef(5,2,0);
        glutSolidCube(2);
    glPopMatrix();
}

//Crtanje pozadine.
int draw_backround(){
    int indikator = 0;

    //Proveravamo da li je ispunjen uslov za kraj igre, odnosno da li je neka od kocki dosla do granicne linije.
    for(int i = 0; i < BROJ_KOCKICA_HORIZONTALNO; i++){
            if(x_kockica[BROJ_KOCKICA_VERTIKALNO-1][i] != 0){
                indikator = 1;
                break;
            }
    }

    float ivica = 1.0;
    glPushMatrix();
        glDisable(GL_LIGHTING);
        glTranslatef(-7,-20,-20);
        glRotatef(45,-1,0,0);
        glScalef(140,70,0);
        glColor3f(1,1,1);

        //Ako je nijedna od kocki nije dosla do granicne linije pozadina je slika prirode, 
        //inace pozadina postaje crna i na njoj je ispisano 'GAME OVER'.
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
        //glutSolidCube(ivica);
        glEnable(GL_LIGHTING);
    glPopMatrix();

    //Vracamo indikator za kraj igre koji se koristi u main.c.
    return indikator;
}

//Pomocna funkcija za ispisivanje teksta na ekranu.
//Kod preuzet sa interneta.
static void renderBitmapString(int x, int y,int z,void* font, char *string){
    //Duzina stringa.
    int len;

    //Iskljucujemo osvetljenje jer se u suprotnom nece lepo postaviti boja teksta.
    glDisable(GL_LIGHTING);
    
    glColor3f(1,1,1);
    glRasterPos3f(x,y,z);
    len = strlen(string);
    for (int i = 0; i < len; i++) 
    {
        glutBitmapCharacter(font, string[i]);
    }

    //Ponovo ukljucujemo osvetljenje.
    glEnable(GL_LIGHTING);
}

//Ispisivanje trenutnog i najboljeg skora.
//Ova funkcija se poziva u funkciji 'draw_semaphore' .
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

//Ispisivanje instrukcija neophodnih korisnicima.
void write_instructions(float* animation_parametar, float *animation_parametar2){
    char word[MAX_LEN];

    //Ovo se ispisuje pre pocetka igre.
    if(!*animation_parametar){
        sprintf(word, "Start on 'G'.");
        const int x = -3;
        const int y = 10;
        const int z = 0;
    renderBitmapString(x,y,z,GLUT_BITMAP_TIMES_ROMAN_24,word);
    }

    //Ovo se ispisuje po zavrsetku igre.
    if(*animation_parametar2 >= 100){
        sprintf(word, "Press 'R' for new game.");
        const int x = -5;
        const int y = 10;
        const int z = 0;
        renderBitmapString(x,y,z,GLUT_BITMAP_TIMES_ROMAN_24,word);
    }
}

//U nastavku su funkcije koje predstavljaju podrsku za timer funkcije iz main.c i u njima glavna logika igrice.

//Ovo je funkcija koja definise kako ce se kocke spustati tokom igre.
void timer_lowering(){
    //Kockice i-te vrste postaju kocke vrste i+1.
    for(int i=BROJ_KOCKICA_VERTIKALNO-1; i>0;i--){
        for(int j=BROJ_KOCKICA_HORIZONTALNO-1; j>=0;j--){
            x_kockica[i][j] = x_kockica[i-1][j];
        }
    }

    //Azuriramo prvu vrstu matrice tako da svi njeni elementi budu 0,
    //odnosno da nema kocki u njoj.
    for(int i=0;i<BROJ_KOCKICA_HORIZONTALNO;i++)
        x_kockica[0][i] = 0;

    srand(time(NULL));

    //Definisemo nove slucajno generisane kocke na mogucim pozicijama u prvoj vrsti.
    double pom;
    for(int i=0;i<BROJ_KOCKICA_HORIZONTALNO; i++){
        pom = x_start + i*4.3 + i*0.3;
        if(rand()%2){
            x_kockica[0][i] = pom;
        }
    }
}

//Funkcija koja definise pomeranje loptice, njeno odbijanje o objekte(kocke i zidove terena) i njenu moc da unistava kocke.
int timer_ball(Terrain terrain, int animation_ongoing){
    //Pre svega moramo da cuvamo x i z koordinatu loptice pre njihove izmene.
    x_prev = x_curr;
    z_prev = z_curr;

    //Menjamo koordinatu z i x i to tako sto dodajemo pomeraje za x i za z.
    //Ukoliko se desi da loptica pokusa da prodje ogradjeni deo terena znak jednog od pomeraja se menja.
    //I to znak z pomeraja ako loptica udari u granicnu liniju ili gornju granicu terena ili x pomeraja ako loptica pokusa
    //da udari u levu ili desnu granicu pomeraja.
    z_curr += -v_z;
    if(z_curr >= terrain.V_TO - 2.5 || z_curr <= terrain.V_FROM + 1)
        v_z *= -1;

    x_curr += -v_x;
    if(x_curr >=  terrain.U_TO - 1 || x_curr <= terrain.U_FROM + 1)
        v_x *= -1;

    //Ukoliko je animacija stala odnosno nasa loptica se nalazi na granicnoj liniji
    //na ovaj nacin cemo se osigurati da loptica nece biti preblizu bocne linije jer
    //moze prouzrokovati odredjene bagove.
    if(animation_ongoing == 0){
    if(x_curr >= terrain.U_TO - 1.2)
        x_curr = terrain.U_TO - 1.2;
    else if(x_curr <= terrain.U_FROM + 1.2)
        x_curr = terrain.U_FROM + 1.2;
    }

    //Indikator da li je loptica udarila u kocku.
    int indikator = 0;
    //Indikator da li je loptica udarila u kocku sa donje ili gornje strane.
    int indikator_z = 0;

    //Ovime je odredjena linija koja predstavlja vektor pravca. Konstanta uz pomeraje je samo tu
    //da bi odredila njenu duzinu.
    direction_x = x_curr-18*v_x;
    direction_z = z_curr-18*v_z;

    //Kolizija.

    //Pre svega ispitujemo da li je loptica udarila u kocku i ako jeste ulazimo u prvi if.
    //Nakon toga bitno nam je na koji ce se nacin loptica odbiti o kocku odnosno da li ju je udarila
    //sa bocne strane ili sa donje/gornje strane. Ukoliko ju je udarila sa bocne strane mora doci do promene
    //x koordinate, a ukoliko ju je udarila sa donje/gornje strane mora doci do promene z koordinate.
    //Zato imamo indikator_z koji nam odredjuje da li je loptica udarila sa gornje donje strane ili ne i to na sledeci nacin:
    //u prvom ifu ispitujemo da li je doslo do udarca u donju stranu dakle da li je loptica udarila u donju granicu neke kocke,
    //ali takodje veoma bitna, ali ne tako ocigledna stvar je i ispitati odakle je loptica dosla odnosno da li je ona
    //udarila kocku sa boka u neku od koordinata donje granice(sto je moguce) ili ju je udarila odozdo i zato moramo da
    //pamtimo prethodne x i y koordinatu loptice koje smo sacuvali na vrhu funkcije, analogna prica je i za udarac u gornju ivicu.
    for(int i=0; i<BROJ_KOCKICA_VERTIKALNO; i++){
        for(int j=0; j<BROJ_KOCKICA_HORIZONTALNO; j++){
            if(((x_curr+0.15 >=  x_kockica[i][j] - 4.3/2.0 || x_curr-0.15 >=  x_kockica[i][j] - 4.3/2.0) 
                && (x_curr+0.15 <= x_kockica[i][j] + 4.3/2.0 || x_curr-0.15 <=  x_kockica[i][j] + 4.3/2.0 )) 
                && ((z_curr+0.15 >= z_kockica[i] - 4.3/2.0 || z_curr-0.15 >= z_kockica[i] - 4.3/2.0) 
                && (z_curr-0.15 <= z_kockica[i] + 4.3/2.0 || z_curr-0.15 <= z_kockica[i] + 4.3/2.0))
                && x_kockica[i][j] != 0){

                //Udarac u donju stranu.
                if((z_curr-0.15 <= z_kockica[i] + 4.3/2 && z_curr+0.15 >= z_kockica[i] + 4.3/2) && !(z_prev-0.15 <= z_kockica[i] + 4.3/2 && z_prev+0.15 >= z_kockica[i] -4.3/2 ))
                    indikator_z = 1;
                //Udarac u gornju stranu.
                else if(z_curr-0.15 <= z_kockica[i] - 4.3/2 && z_curr+0.15 >= z_kockica[i] - 4.3/2 && !(z_prev-0.15 <= z_kockica[i] +4.3/2 && z_prev+0.15 >= z_kockica[i] -4.3/2 ))
                    indikator_z = 1;
                
                indikator = 1;
                //Na ovaj nacin kocka nestaje sa terena jer se iscrtavaju samo kocje cija x koordinata nije 0.
                x_kockica[i][j] = 0;
                break;
            }
        }
    }

    //U slucaju da je loptica udarila sa gornje/donje strane u kocku menja se z koordinata,
    //inace menja se x koordinata.
    if(indikator){
        if(indikator_z)
            v_z*=-1;
        else
            v_x*=-1;
    }
    indikator = 0;
    indikator_z = 0;
    
    //Ponovo se iscrtava sve.
    glutPostRedisplay();

    //Po povratku loptice na granicnu liniju poziva se timer_lowering koji spusta
    //kocke za jedno polje ka dole i uvecava se skor, a samim tim i best skor ukoliko
    //u nekoj od prethodnih partija nije postavljen na vece.
    if(z_curr >= terrain.V_TO - 2.5){
        ++score;
        timer_lowering();
        return 0;
        //play = 0;
    }
    return 1;
}

//Podrska za pomeranje linije koja predstavlja vektor pravca u levo i u desno.
void timer_direction_vector(int direction){
    //Smer ce biti jedan ukoliko pritiskamo 'a'/'A' za pomeraj u levo.
    if(direction == 1){
        v_x -= 0.02;
    }
    //Smer ce biti jedan ukoliko pritiskamo 'd'/'D' za pomeraj u desno.
    else if(direction == 2){
        v_x += 0.02;
    }

    //Vrlo je bitno da racunamo intenzitet.
    float intezitet = sqrt(v_x*v_x + v_z*v_z);

    //I da delimo svaku koordinatu sa intenzitetom kako bismo imali jedinicni vektor.
    //U suprotnom bi se jedna koordinata stalno vise povecavala i animacija pomeranja
    //loptice bi bila promenljive brzine(nekad cak i ekstremno brza).
    v_x = v_x / (intezitet*4);
    v_z = v_z / (intezitet*4);

    //Uvodimo ogranicenje koliko nisko moze sa se usmeri vektor pravca
    //koje za nijansu otezava igru i tako je cini zanimljivijom.
    if(v_x >= 0.225)
        v_x = 0.225;
    else if(v_x <= -0.225)
        v_x = -0.225;
    
    //Kako smo azurirali pomeraj x i pomeraj z azuriramo i koordinate po kojima
    //se iscrtava linija koja predstavlja vektor pravca po kom se ispaljuje loptica.
    direction_x = x_curr-18*v_x;
    direction_z = z_curr-18*v_z;
}