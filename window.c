/*!\file window.c
 * \brief géométries lumière diffuse et transformations de base en GL4Dummies
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 15 2016 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
/* Prototypes des fonctions statiques contenues dans ce fichier C */
#define NUM_PARTICLES 1000

typedef struct {
    GLfloat position[3];
    GLfloat color[4];
    GLuint sphereId;
} Particle;

static void init(void);
static void resize(int w, int h);
static void draw(void);
static void quit(void);
/*!\brief dimensions de la fenêtre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief quelques objets géométriques */
static Particle particles[NUM_PARTICLES];

/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.*/
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 0, 0, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
/*!\brief initialise les paramètres OpenGL et les données */
static void init(void) {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  _pId  = gl4duCreateProgram("<vs>shaders/dep3d.vs", "<fs>shaders/dep3d.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_wW, _wH);
  for (int i = 0; i < NUM_PARTICLES; ++i) {
      particles[i].position[0] = (GLfloat)(rand() % 20 - 10); // Position x aléatoire entre -10 et 10
      particles[i].position[1] = (GLfloat)(rand() % 20 - 10); // Position y aléatoire entre -10 et 10
      particles[i].position[2] = (GLfloat)(rand() % 20 - 10); // Position z aléatoire entre -10 et 10
      particles[i].color[0] = (GLfloat)rand() / RAND_MAX; // Composante rouge aléatoire entre 0 et 1
      particles[i].color[1] = (GLfloat)rand() / RAND_MAX; // Composante verte aléatoire entre 0 et 1
      particles[i].color[2] = (GLfloat)rand() / RAND_MAX; // Composante bleue aléatoire entre 0 et 1
      particles[i].color[3] = 1.0f; // Opacité à 1


       particles[i].sphereId = gl4dgGenSpheref(30, 30);
        
  }
  
}
/*!\brief Cette fonction paramétre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.*/
static void resize(int w, int h) {
  _wW  = w; _wH = h;
  glViewport(0, 0, _wW, _wH);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _wH / _wW, 0.5 * _wH / _wW, 1.0, 1000.0);
  //gl4duOrthof(-3.5, 3.5, -3.5 * _wH / _wW, 3.5 * _wH / _wW, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
}
/*!\brief dessine dans le contexte OpenGL actif. */

static void draw(void) {
    static GLfloat a = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl4duBindMatrix("modelViewMatrix");
    gl4duLoadIdentityf(); // Réinitialise la matrice modèle à l'identité

    glUseProgram(_pId);
    gl4duTranslatef(0, 0, -25.0);
    gl4duPushMatrix(); // Sauvegarde la matrice modèle
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        printf("Particle %d - Position: (%f, %f, %f)\n", i, particles[i].position[0], particles[i].position[1], particles[i].position[2]); // Affiche les positions des particules
        gl4duLoadIdentityf(); // Réinitialise la matrice modèle pour chaque particule
        gl4duTranslatef(particles[i].position[0], particles[i].position[1], particles[i].position[2]);
        glUniform4fv(glGetUniformLocation(_pId, "couleur"), 1, particles[i].color);
        gl4dgDraw(particles[i].sphereId);
    }
    gl4duPopMatrix(); // Restaure la matrice modèle sauvegardée
    gl4duSendMatrices();
    a++;
}


/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  gl4duClean(GL4DU_ALL);
}
