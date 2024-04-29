/*!\file window.c
 *
 * \brief Bruit de Perlin appliqu� en GPU
 * \author  MATHANARUBAN Jonny
 * \date 28/04/2024
 */
#include <GL4D/gl4du.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>


#define R_MASK 0xff000000
#define G_MASK 0x00ff0000
#define B_MASK 0x0000ff00
#define A_MASK 0x000000ff

#define ECHANTILLONS 1024
/*!\brief amplitude des échantillons du signal sonore */
static Sint16 _hauteurs[ECHANTILLONS];
/*!\brief dimensions de la fenêtre */
static int _wW = ECHANTILLONS, _wH = 256;
/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void         init(void);
static void         keydown(int keycode);
static void         draw(void);
static void         quit(void);

static void initAudio(const char * filename);
static void mixCallback(void *udata, Uint8 *stream, int len);
/* fonctions externes dans noise.c */
extern void initNoiseTextures(void);
extern void useNoiseTextures(GLuint pid, int shift);
extern void unuseNoiseTextures(int shift);
extern void freeNoiseTextures(void);


extern void initNoiseTexturesforSurface(void);
extern void useNoiseTexturesforSurface(GLuint pid, int shift);
extern void unuseNoiseTexturesforSurface(int shift);
extern void freeNoiseTexturesforSurface(void);

static void initText(GLuint * ptId, const char * text);
static GLuint _textTexId = 0;



/*!\brief identifiant de la g�om�trie */
static GLuint _geom = 0;
/*!\brief identifiant des GLSL program */
static GLuint _pId = 0;
static GLuint _tid = 0;
static GLuint _xid = 0;

/*!\brief arrete l'animation */
static GLuint _pause = 0;
/*!\brief coefficient de zoom */
static GLfloat _zoom = 30.0;
static GLfloat _zoom2=30.0;
static GLuint _disk = 0;
static GLint _blur = 0;
static GLuint _quad = 0;

/*!\brief temps */
static GLfloat _temps = 0.1;
static Mix_Music * _mmusic = NULL;



/*!\brief La fonction principale cr�� la fen�tre d'affichage,
 * initialise GL et les donn�es, affecte les fonctions d'�v�nements et
 * lance la boucle principale d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "DL4D-Disco_Ball", 0, 0, 
			 1280, 1024, SDL_WINDOW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(draw);
  initAudio("theoryofeverything.mid");
  gl4duwMainLoop();
  return 0;
}

/*!\brief initialise les param�tres OpenGL */
static void init(void) {
    initNoiseTextures();
    initNoiseTexturesforSurface();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
    glViewport(0, 0, 1280, 1024);
    gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
    gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
    gl4duBindMatrix("projectionMatrix");
    gl4duLoadIdentityf();
    gl4duFrustumf(-0.5, 0.5, -0.5, 0.5, 1.0, 1000.0);
    gl4duBindMatrix("modelViewMatrix");
    _pId  = gl4duCreateProgram("<vs>shaders/trounoir.vs", "<fs>shaders/trounoir.fs", NULL);
    _tid = gl4duCreateProgram("<vs>shaders/trounoir.vs","<fs>shaders/surface.fs",NULL);
    _xid = gl4duCreateProgram("<vs>shaders/credits.vs", "<fs>shaders/credits.fs", NULL);
    _geom =  gl4dgGenSpheref(30, 30);
    _disk = gl4dgGenDiskf(30);
    _quad = gl4dgGenQuadf();
    _quad = gl4dgGenQuadf();
  initText(&_textTexId, 
	   "Il y a bien longtemps dans une galaxie lointaine, tres lointaine...\n\n\n"
	   "Un trou qui dechire le ciel est apparu , un trou noir dansant"
	   "La distorsion provoque par Perlin sama provoque un effet psychadelique"
	   "  \n\n\nAlors qu'ils luttent contre la puissance sombre de"
	   " l'ennemi , ces héros courageux peuvent encore éveiller l'habileté et"
	   " la force nécessaires pour forger un nouvel avenir..."
       " Merci Pour votre visionnage "
       "\n\n\n"
       "source pour la musique:"
       "https://freemidi.org/");
}




static void keydown(int keycode) {
    switch(keycode) {
    case SDLK_DOWN:
        _zoom -= 10.1;
        break;
    case SDLK_UP:
        _zoom += 10.0;
        break;
    case SDLK_p:
        _blur += 1;
        break;
    case SDLK_m:
        _blur -= 1;
        if(_blur < 0)
        _blur = 0;
        break;
    case ' ':
        _pause = !_pause;
        break;
    case SDLK_ESCAPE:
    case 'q':
        exit(0);
    default:
        break;
    }
}

/*!\brief Cette fonction dessine dans le contexte OpenGL actif.*/
static void draw(void) {
    int i;
    static GLfloat a0 = 0.0;
    static Uint32 t0 = 0;
    GLfloat dt = 0.0;
    Uint32 t;
    int x0, y0,x1,y1;
    dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
    t0 = t;
    for(i = 0; i < ECHANTILLONS; i+=4) {
    x0 = (i * (_wW - 1)) / (ECHANTILLONS - 1);
    y0 = _hauteurs[i];
    x1 = ((i + 1) * (_wW - 1)) / (ECHANTILLONS - 1);
    y1 = _hauteurs[i + 1];
  }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl4duBindMatrix("modelViewMatrix");

    // Dessin de la sphère
    gl4duLoadIdentityf();
    gl4duTranslatef(0, 0, -10);
    glUseProgram(_pId);
    gl4duPushMatrix();
    gl4duRotatef(10 , 0, 1, 0);
    gl4duRotatef(a0, 1, 0, 0);
    gl4duScalef((x0+y0)*0.0015f, (x0+y0)*0.00150f, (x0+y0)*0.00150f);
    _zoom +=  0.000051f*(x0+y0);
    glUniform1f(glGetUniformLocation(_pId, "zoom"), _zoom);
    glUniform1f(glGetUniformLocation(_pId, "temps"), _temps);
    useNoiseTexturesforSurface(_pId, 0);
    gl4duSendMatrices();
    gl4dgDraw(_geom);
    unuseNoiseTextures(0);
    gl4duPopMatrix();

    // Dessin du disque
    gl4duLoadIdentityf();
    gl4duPushMatrix();
    glUseProgram(_tid);
    gl4duTranslatef(0.0f, 0.0f, -100.0f); // Déplacement vers le bas et vers l'arrière
    gl4duRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    gl4duRotatef(-a0*1.0f, 0, 1, 0);
    gl4duScalef((x1+y1)*0.1f, (x1+y1)*0.1f,(x1+y1)*0.1f);
    _zoom2 += 0.0000059f*(x1+y1);
    glUniform1f(glGetUniformLocation(_tid, "fond"), _zoom2);
    glUniform1f(glGetUniformLocation(_tid, "temps"), _temps);
    useNoiseTexturesforSurface(_tid, 0);
    gl4duSendMatrices();
    gl4dgDraw(_disk);
    unuseNoiseTexturesforSurface(0);
    gl4duPopMatrix();
    
    if (!_pause){
        _temps += dt / 50.0;
        a0 += 360.0 * dt / (24.0 /* * 60.0 */);
    }
    gl4dfBlur(0, 0, _blur, 1, 0, GL_FALSE);
    gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
    gl4dfSobel(0, 0, GL_FALSE);
    if(_zoom > 200){
        const GLfloat inclinaison = -60.0;
    static GLfloat t0 = -1;
    GLfloat t, d;
        if(t0 < 0.0f)
            t0 = SDL_GetTicks();
        t = (SDL_GetTicks() - t0) / 1000.0f, d = -2.4f /* du retard pour commencer en bas */ + 0.1f /* vitesse */ * t;
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(_xid);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _textTexId);
        glUniform1i(glGetUniformLocation(_xid, "inv"), 1);
        glUniform1i(glGetUniformLocation(_xid, "tex"), 0);
        gl4duBindMatrix("modelViewMatrix");
        gl4duLoadIdentityf();
        gl4duScalef(1, 5, 1);
        gl4duTranslatef(0, d * cos(inclinaison * M_PI / 180.0f), -2 + d * sin(inclinaison * M_PI / 180.0f));
        gl4duRotatef(inclinaison, 1, 0, 0);
        gl4duSendMatrices();
        gl4dgDraw(_quad);
        glUseProgram(0);
        if (d >= 1.4f) {
            gl4duClean(GL4DU_ALL);
            exit(0);
        }
    }

}


static void initAudio(const char * filename) {
    int mixFlags = MIX_INIT_OGG | MIX_INIT_MP3 | MIX_INIT_MOD, res;
    res = Mix_Init(mixFlags);
    if( (res & mixFlags) != mixFlags ) {
        fprintf(stderr, "Mix_Init: Erreur lors de l'initialisation de la bibliotheque SDL_Mixer\n");
        fprintf(stderr, "Mix_Init: %s\n", Mix_GetError());
    }
    if(Mix_OpenAudio(44100, AUDIO_S16LSB, 2, 1024) < 0)
        exit(4);
    if(!(_mmusic = Mix_LoadMUS(filename))) {
        fprintf(stderr, "Erreur lors du Mix_LoadMUS: %s\n", Mix_GetError());
        exit(5);
    }
    Mix_SetPostMix(mixCallback, NULL);

    if(!Mix_PlayingMusic())
        Mix_PlayMusic(_mmusic, -1);
}


static void mixCallback(void *udata, Uint8 *stream, int len) {
    int i;
    Sint16 *s = (Sint16 *)stream;
    if(len >= 2 * ECHANTILLONS)
        for(i = 0; i < ECHANTILLONS; i++)
        _hauteurs[i] = _wH / 2 + (_wH / 2) * s[i] / ((1 << 15) - 1.0);
    return;
}

/*!\brief appel�e au moment de sortir du programme (atexit), lib�re les �l�ments utilis�s */
static void quit(void) {
    freeNoiseTextures();
    freeNoiseTexturesforSurface();
    gl4duClean(GL4DU_ALL);
    if(_mmusic) {
        if(Mix_PlayingMusic())
        Mix_HaltMusic();
        Mix_FreeMusic(_mmusic);
        _mmusic = NULL;
    }
    Mix_CloseAudio();
    Mix_Quit();
}
static void initText(GLuint * ptId, const char * text) {
  static int firstTime = 1;
  SDL_Color c = {255, 255, 0, 255};
  SDL_Surface * d, * s;
  TTF_Font * font = NULL;
  if(firstTime) {
    /* initialisation de la bibliothèque SDL2 ttf */
    if(TTF_Init() == -1) {
      fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
      exit(2);
    }
    firstTime = 0;
  }
  if(*ptId == 0) {
    /* initialisation de la texture côté OpenGL */
    glGenTextures(1, ptId);
    glBindTexture(GL_TEXTURE_2D, *ptId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  /* chargement de la font */
  if( !(font = TTF_OpenFont("Early_GameBoy.ttf", 128)) ) {
    fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
    return;
  }
  /* création d'une surface SDL avec le texte */
  d = TTF_RenderUTF8_Blended_Wrapped(font, text, c, 2048);
  if(d == NULL) {
    TTF_CloseFont(font);
    fprintf(stderr, "Erreur lors du TTF_RenderText\n");
    return;
  }
  /* copie de la surface SDL vers une seconde aux spécifications qui correspondent au format OpenGL */
  s = SDL_CreateRGBSurface(0, d->w, d->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
  assert(s);
  SDL_BlitSurface(d, NULL, s, NULL);
  SDL_FreeSurface(d);
  /* transfert vers la texture OpenGL */
  glBindTexture(GL_TEXTURE_2D, *ptId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
  fprintf(stderr, "Dimensions de la texture : %d %d\n", s->w, s->h);
  SDL_FreeSurface(s);
  TTF_CloseFont(font);
  glBindTexture(GL_TEXTURE_2D, 0);
}
