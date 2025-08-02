#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"

#define XSIDE	80			// length of the x side of the grid
#define X0      (-XSIDE/2.)		// where one side starts
#define NX	100			// how many points in x
#define DX	( XSIDE/(float)NX )	// change in x between the points

#define YGRID	0.f

#define ZSIDE	80			// length of the z side of the grid
#define Z0      (-ZSIDE/2.)		// where one side starts
#define NZ	100		// how many points in z
#define DZ	( ZSIDE/(float)NZ )	// change in z between the points


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// title of these windows:

const char* WINDOWTITLE = "OpenGL / GLUT Sample -- Shanmukha";
const char* GLUITITLE = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT = 4;
const int MIDDLE = 2;
const int RIGHT = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

char* ColorNames[] =
{
	(char*)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE = GL_LINEAR;
const GLfloat FOGDENSITY = 0.30f;
const GLfloat FOGSTART = 1.5f;
const GLfloat FOGEND = 4.f;

// for lighting:

const float	WHITE[] = { 1.,1.,1.,1. };

// for animation:

const int MS_PER_CYCLE = 10000;		// 10000 milliseconds = 10 seconds
const int MSEC = 20000;

// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
GLuint	BoxList;				// object display list
GLuint  GridDL;                 // grid display list
GLuint	Spherelist, EarthDL;	        // display lists
GLuint	EarthTex;		            // texture object
GLuint  SpaceshuttleDL;
GLuint  asteroidlist;
GLuint ltsource;
GLuint lightstate = 0;
GLuint Toruslist;

int     shadetype = 0;
bool textureMode = true;
bool lightMode = true;
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
int		NowProjection;		    // ORTHO or PERSP
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
float	Time;					// used for animation, this has a value between 0. and 1.
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees


// function prototypes:

void	Animate();
void	Display();
void	DoAxesMenu(int);
void	DoColorMenu(int);
void	DoDepthBufferMenu(int);
void	DoDepthFightingMenu(int);
void	DoDepthMenu(int);
void	DoDebugMenu(int);
void	DoMainMenu(int);
void	DoProjectMenu(int);
void	DoRasterString(float, float, float, char*);
void	DoStrokeString(float, float, float, float, char*);
float	ElapsedSeconds();
void	InitGraphics();
void	InitLists();
void	InitMenus();
void	Keyboard(unsigned char, int, int);
void	MouseButton(int, int, int, int);
void	MouseMotion(int, int);
void	Reset();
void	Resize(int, int);
void	Visibility(int);
const int textureSize = 64;
unsigned char checkerboard[textureSize][textureSize][3];


void			Axes(float);
void			HsvRgb(float[3], float[3]);
void			Cross(float[3], float[3], float[3]);
float			Dot(float[3], float[3]);
float			Unit(float[3], float[3]);
float			Unit(float[3]);


// utility to create an array from 3 separate values:

float*
Array3(float a, float b, float c)
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:

float*
MulArray3(float factor, float array0[])
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}


float*
MulArray3(float factor, float a, float b, float c)
{
	static float array[4];

	float* abc = Array3(a, b, c);
	array[0] = factor * abc[0];
	array[1] = factor * abc[1];
	array[2] = factor * abc[2];
	array[3] = 1.;
	return array;
}


// these are here for when you need them -- just uncomment the ones you need:

#include "setmaterial.cpp"
#include "setlight.cpp"
#include "osusphere.cpp"
#include "osucone.cpp"
#include "osutorus.cpp"
#include "bmptotexture.cpp"
#include "loadobjfile.cpp"
#include "keytime.cpp"
#include "glslprogram.cpp"

Keytimes Spaceshuttledirx;
Keytimes Spaceshuttlediry;
Keytimes Spaceshuttledirz;
Keytimes Zrotation;
Keytimes eyeviewx;
Keytimes eyeviewy;
Keytimes eyeviewz;
Keytimes lookplacex;
Keytimes lookplacez;
Keytimes lookplacey;
Keytimes ast1x;
Keytimes ast1y;
Keytimes ast2x;
Keytimes ast2y;
Keytimes lightdir;
Keytimes lightdirx;


// main program:

int
main(int argc, char* argv[])
{
	// turn on the glut package:
	// (do this before checking argc and argv since glutInit might
	// pull some command line arguments out)

	glutInit(&argc, argv);

	// setup all the graphics stuff:

	InitGraphics();

	// create the display lists that **will not change**:

	InitLists();

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset();

	// setup all the user interface stuff:

	InitMenus();

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow(MainWindow);
	glutMainLoop();

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate()
{
	// put animation stuff in here -- change some global variables for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;							// makes the value of ms between 0 and MS_PER_CYCLE-1
	Time = (float)ms / (float)MS_PER_CYCLE;		// makes the value of Time between 0. and slightly less than 1.

	// for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

	// force a call to Display( ) next time it is convenient:

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// draw the complete scene:

void
Display()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting Display.\n");

	// set which window we want to do the graphics into:
	glutSetWindow(MainWindow);

	// erase the background:
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
#ifdef DEMO_DEPTH_BUFFER
	if (DepthBufferOn == 0)
		glDisable(GL_DEPTH_TEST);
#endif


	// specify shading to be flat:

	if (shadetype == 0)
		glShadeModel(GL_FLAT);
	else
		glShadeModel(GL_SMOOTH);

	// set the viewport to be a square centered in the window:

	GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);

	int msec = glutGet(GLUT_ELAPSED_TIME) % MSEC;

	// turn that into a time in seconds:
	float nowTime = (float)msec / 1000.;

	// set the viewing volume:
	// remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	if (NowProjection == ORTHO)
		glOrtho(-2.f, 2.f, -2.f, 2.f, 0.1f, 1000.f);
	else
		gluPerspective(70.f, 1.f, 0.1f, 1000.f);

	// place the objects into the scene:

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set the eye position, look-at position, and up-vector:

	gluLookAt(eyeviewx.GetValue(nowTime), eyeviewy.GetValue(nowTime), eyeviewz.GetValue(nowTime), lookplacex.GetValue(nowTime), lookplacey.GetValue(nowTime), lookplacez.GetValue(nowTime),0.f,0.f,1.f );

	// rotate the scene:

	glRotatef((GLfloat)Yrot, 0.f, 1.f, 0.f);
	glRotatef((GLfloat)Xrot, 1.f, 0.f, 0.f);

	// uniformly scale the scene:

	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

	// set the fog parameters:

	if (DepthCueOn != 0)
	{
		glFogi(GL_FOG_MODE, FOGMODE);
		glFogfv(GL_FOG_COLOR, FOGCOLOR);
		glFogf(GL_FOG_DENSITY, FOGDENSITY);
		glFogf(GL_FOG_START, FOGSTART);
		glFogf(GL_FOG_END, FOGEND);
		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}

	// possibly draw the axes:

	if (AxesOn != 0)
	{
		glColor3fv(&Colors[NowColor][0]);
		glCallList(AxesList);
	}
	if (lightstate != 0)
		SetPointLight(GL_LIGHT0, 0, lightdir.GetValue(nowTime), 0, 1, 1, 1);
	else
		SetSpotLight(GL_LIGHT0, 0, lightdir.GetValue(nowTime), 0, 0.f, -1.f, 0.f, 1, 1, 1);

	// since we are using glScalef( ), be sure the normals get unitized:

	glEnable(GL_NORMALIZE);

	if (nowTime >= 11)

	{
		glColor3f(1, 1, 1);
		glBegin(GL_POINTS);

		glVertex3f(-24, 39, 1);
		glVertex3f(-25, 39.5, 2);
		glVertex3f(-26, 39, 3);
		glVertex3f(-27, 40, -1);
		glVertex3f(-28, 41, -2);
		glVertex3f(-29, 40, -3);
		glVertex3f(-30, 42, 3);
		glVertex3f(-31, 43, 2);
		glVertex3f(-32, 41, 1);
		glVertex3f(-33, 42, -3);
		glVertex3f(-34, 40, -2);
		glVertex3f(-35, 41, -1);
		glVertex3f(-36, 43, 3);
		glVertex3f(-37, 33, 2);
		glVertex3f(-38, 40, 1);
		glVertex3f(-39, 44, -3);
		glVertex3f(-40, 38, -3);
		glVertex3f(-41, 37, -3);
		glVertex3f(-42, 45, 3);
		glVertex3f(-43, 31, 3);
		glVertex3f(-24, 36, 3);
		glVertex3f(-25, 46, -6);
		glVertex3f(-26, 36, -6);
		glVertex3f(-27, 44, -6);
		glVertex3f(-28, 41, 6);
		glVertex3f(-29, 47, 6);
		glVertex3f(-30, 46, 4);
		glVertex3f(-31, 36, -4);
		glVertex3f(-32, 38, -7);
		glVertex3f(-33, 39, -6);
		glVertex3f(-34, 35, 1);
		glVertex3f(-35, 33, 2);
		glVertex3f(-36, 35, 3);
		glVertex3f(-37, 34, -1);
		glVertex3f(-38, 34, -2);
		glVertex3f(-39, 34, -3);
		glVertex3f(-40, 33, 1);
		glVertex3f(-41, 33, 1);
		glEnd();
	}
	if (nowTime >= 13)
	glBegin(GL_LINES);

	glColor3f(1, 1, 0);
	glVertex3f(-24, 39, 0);
	glVertex3f(-40, 39, 0);

	glColor3f(1, 0, 0);
	glVertex3f(-24, 37, 0);
	glVertex3f(-40, 37,0);

	glColor3f(1, 1, 1);
	glVertex3f(-24, 35, 0);
	glVertex3f(-40, 35, 0);

	glColor3f(1, 0, 1);
	glVertex3f(-24, 33, 0);
	glVertex3f(-40, 33, 0);

	glColor3f(0, 1, 0);
	glVertex3f(-24, 31, 0);
	glVertex3f(-40, 31, 0);

	glColor3f(1, 1, 0);
	glVertex3f(-24, 41, 0);
	glVertex3f(-40, 41, 0);

	glColor3f(1, 0, 0);
	glVertex3f(-24, 43, 0);
	glVertex3f(-40, 43, 0);

	glColor3f(1, 1, 1);
	glVertex3f(-24, 45, 0);
	glVertex3f(-40, 45, 0);

	glColor3f(1, 0, 1);
	glVertex3f(-24, 47, 0);
	glVertex3f(-40, 47, 0);

	glColor3f(0, 1, 0);
	glVertex3f(-24, 39, 3);
	glVertex3f(-40, 39, 3);

	glColor3f(1, 1, 0);
	glVertex3f(-24, 37, 3);
	glVertex3f(-40, 37, 3);

	glColor3f(1, 0, 0);
	glVertex3f(-24, 35, 3);
	glVertex3f(-40, 35, 3);

	glColor3f(1, 1, 1);
	glVertex3f(-24, 33, 3);
	glVertex3f(-40, 33, 3);

	glColor3f(1, 0, 1);
	glVertex3f(-24, 31, 3);
	glVertex3f(-40, 31, 3);

	glColor3f(1, 1, 1);
	glVertex3f(-24, 41, 3);
	glVertex3f(-40, 41, 3);

	glColor3f(1, 1, 0);
	glVertex3f(-24, 43, 3);
	glVertex3f(-40, 43, 3);

	glColor3f(1, 0, 0);
	glVertex3f(-24, 45, 3);
	glVertex3f(-40, 45, 3);

	glColor3f(1, 1, 1);
	glVertex3f(-24, 47, 3);
	glVertex3f(-40, 47, 3);

	glColor3f(0, 1, 0);
	glVertex3f(-24, 39, -6);
	glVertex3f(-40, 39, -6);

	glColor3f(1, 1, 0);
	glVertex3f(-24, 37, -6);
	glVertex3f(-40, 37, -6);

	glColor3f(1, 0, 0);
	glVertex3f(-24, 35, -6);
	glVertex3f(-40, 35, -6);
	glEnd();

	if (nowTime >= 14)
	glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	glVertex3f(-24, 33, -6);
	glVertex3f(-40, 33, -6);

	glColor3f(1, 0, 1);
	glVertex3f(-24, 31, -6);
	glVertex3f(-40, 31, -6);

	glColor3f(1, 1, 1);
	glVertex3f(-24, 41, -6);
	glVertex3f(-40, 41, -6);

	glColor3f(1, 1, 0);
	glVertex3f(-24, 43, -6);
	glVertex3f(-40, 43, -6);

	glColor3f(1, 0, 0);
	glVertex3f(-24, 45, -6);
	glVertex3f(-40, 45, -6);

	glColor3f(0, 1, 0);
	glVertex3f(-24, 39, 6);
	glVertex3f(-40, 39, 6);

	glColor3f(1, 1, 0);
	glVertex3f(-24, 37, 6);
	glVertex3f(-40, 37, 6);

	glColor3f(1, 0, 0);
	glVertex3f(-24, 35, 6);
	glVertex3f(-40, 35, 6);

	glColor3f(1, 1, 1);
	glVertex3f(-24, 33, 6);
	glVertex3f(-40, 33, 6);
	glEnd();

	if (nowTime >= 15)
	glBegin(GL_LINES);
	glColor3f(1, 0, 1);
	glVertex3f(-24, 31, 6);
	glVertex3f(-40, 31, 6);

	glColor3f(1, 1, 1);
	glVertex3f(-24, 41, 6);
	glVertex3f(-40, 41, 6);

	glColor3f(1, 1, 0);
	glVertex3f(-24, 43, 6);
	glVertex3f(-40, 43, 6);

	glColor3f(1, 0, 0);
	glVertex3f(-24, 45, 6);
	glVertex3f(-40, 45, 6);

	glColor3f(1, 1, 1);
	glVertex3f(-24, 47, 6);
	glVertex3f(-40, 47, 6);

	glColor3f(0, 1, 0);
	glVertex3f(-24, 39, -3);
	glVertex3f(-40, 39, -3);

	glColor3f(1, 1, 0);
	glVertex3f(-24, 37, -3);
	glVertex3f(-40, 37, -3);

	glColor3f(1, 0, 0);
	glVertex3f(-24, 35, -3);
	glVertex3f(-40, 35, -3);

	glColor3f(1, 1, 1);
	glVertex3f(-24, 33, -3);
	glVertex3f(-40, 33, -3);

	glColor3f(1, 0, 1);
	glVertex3f(-24, 31, -3);
	glVertex3f(-40, 31, -3);

	glColor3f(1, 1, 1);
	glVertex3f(-24, 41, -3);
	glVertex3f(-40, 41, -3);

	glColor3f(1, 1, 0);
	glVertex3f(-24, 43, -3);
	glVertex3f(-40, 43, -3);

	glColor3f(1, 0, 0);
	glVertex3f(-24, 45, -3);
	glVertex3f(-40, 45, -3);
 
	glEnd();

	// draw the box object by calling up its display list:

	if (textureMode)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	if (lightMode)
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	else
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	glPushMatrix();
	glTranslatef(0, 6, 0);
	SetMaterial(1.f, 1.f, 1.f, 30.f);
	glCallList(EarthDL);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);


	glPushMatrix();
	glColor3f(1.f, 1.f, 1.f);
	glTranslatef(0, 100, 0);
	glCallList(ltsource);
	glPopMatrix();

	glPushMatrix();
	glShadeModel(GL_FLAT);
	glTranslatef(ast1x.GetValue(nowTime), ast1y.GetValue(nowTime), 0);
	SetMaterial(1.0, 1.0, 0, 90);
	glCallList(asteroidlist);
	glPopMatrix();

	glPushMatrix();
	glShadeModel(GL_FLAT);
	glTranslatef(-9.5, 35.5, 0);
    glRotatef(90, 0, 0, 1);
	SetMaterial(0.0, 1.0, 1.0, 90);
	glCallList(asteroidlist);
	glPopMatrix();

	glPushMatrix();
	glShadeModel(GL_FLAT);
	glTranslatef(ast2x.GetValue(nowTime), ast2y.GetValue(nowTime), 0);
	SetMaterial(1.0, 0.5, 0, 90);
	glCallList(asteroidlist);
	glPopMatrix();

	glPushMatrix();
	glShadeModel(GL_FLAT);
	glTranslatef(-18, 30, 0);
    glRotatef(0, 90, 0, 1);
	SetMaterial(1.0, 0., 1.0, 90);
	glCallList(asteroidlist);
	glPopMatrix();

	glPushMatrix();
	glShadeModel(GL_FLAT);
	glTranslatef(5, 30, 0);
	SetMaterial(0.5, 1.5, 0.0, 90);
	glCallList(asteroidlist);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-24, 39, 0);
	glRotatef(90, 0, 0, 1);
	SetMaterial(1.f, 1.0f, 1.f, 30.f);
	glCallList(Toruslist);		// a dl can call another dl that has been previously created
	glPopMatrix();
	glEndList();

	if (nowTime<=19.2)
	{
		glPushMatrix();  //Spaceshuttle 
		glTranslatef(Spaceshuttledirx.GetValue(nowTime), Spaceshuttlediry.GetValue(nowTime), Spaceshuttledirz.GetValue(nowTime));
		glRotatef(Zrotation.GetValue(nowTime), 0, 0, 1);
		glRotatef(90, 0, 0, 1);
		glScalef(0.015, 0.015, 0.015);
		glCallList(SpaceshuttleDL);
		glPopMatrix();
	}


	glDisable(GL_LIGHTING);

#ifdef DEMO_Z_FIGHTING
	if (DepthFightingOn != 0)
	{
		glPushMatrix();
		glRotatef(90.f, 0.f, 1.f, 0.f);
		glCallList(BoxList);
		glPopMatrix();
	}
#endif


	// draw some gratuitous text that just rotates on top of the scene:
	// i commented out the actual text-drawing calls -- put them back in if you have a use for them
	// a good use for thefirst one might be to have your name on the screen
	// a good use for the second one might be to have vertex numbers on the screen alongside each vertex

	glDisable(GL_DEPTH_TEST);
	glColor3f(0.f, 1.f, 1.f);
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.f, 100.f, 0.f, 100.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.f, 1.f, 1.f);
	//DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

	// swap the double-buffered framebuffers:

	glutSwapBuffers();

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush();
}


void
DoAxesMenu(int id)
{
	AxesOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoColorMenu(int id)
{
	NowColor = id - RED;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDebugMenu(int id)
{
	DebugOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthBufferMenu(int id)
{
	DepthBufferOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthFightingMenu(int id)
{
	DepthFightingOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoDepthMenu(int id)
{
	DepthCueOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// main menu callback:

void
DoMainMenu(int id)
{
	switch (id)
	{
	case RESET:
		Reset();
		break;

	case QUIT:
		// gracefully close out the graphics:
		// gracefully close the graphics window:
		// gracefully exit the program:
		glutSetWindow(MainWindow);
		glFinish();
		glutDestroyWindow(MainWindow);
		exit(0);
		break;

	default:
		fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


void
DoProjectMenu(int id)
{
	NowProjection = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// use glut to display a string of characters using a raster font:

void
DoRasterString(float x, float y, float z, char* s)
{
	glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString(float x, float y, float z, float ht, char* s)
{
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
	float sf = ht / (119.05f + 33.33f);
	glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
	char c;			// one character to print
	for (; (c = *s) != '\0'; s++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
	}
	glPopMatrix();
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds()
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet(GLUT_ELAPSED_TIME);

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow(MainWindow);

	int numColors = sizeof(Colors) / (3 * sizeof(float));
	int colormenu = glutCreateMenu(DoColorMenu);
	for (int i = 0; i < numColors; i++)
	{
		glutAddMenuEntry(ColorNames[i], i);
	}

	int axesmenu = glutCreateMenu(DoAxesMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthcuemenu = glutCreateMenu(DoDepthMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int debugmenu = glutCreateMenu(DoDebugMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int projmenu = glutCreateMenu(DoProjectMenu);
	glutAddMenuEntry("Orthographic", ORTHO);
	glutAddMenuEntry("Perspective", PERSP);

	int mainmenu = glutCreateMenu(DoMainMenu);
	glutAddSubMenu("Axes", axesmenu);
	glutAddSubMenu("Axis Colors", colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu("Depth Buffer", depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu("Depth Fighting", depthfightingmenu);
#endif

	glutAddSubMenu("Depth Cue", depthcuemenu);
	glutAddSubMenu("Projection", projmenu);
	glutAddMenuEntry("Reset", RESET);
	glutAddSubMenu("Debug", debugmenu);
	glutAddMenuEntry("Quit", QUIT);

	// attach the pop-up menu to the right mouse button:

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	// set the initial window configuration:

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

	// open the window and set its title:

	MainWindow = glutCreateWindow(WINDOWTITLE);
	glutSetWindowTitle(WINDOWTITLE);

	// set the framebuffer clear values:

	glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow(MainWindow);
	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc(Visibility);
	glutEntryFunc(NULL);
	glutSpecialFunc(NULL);
	glutSpaceballMotionFunc(NULL);
	glutSpaceballRotateFunc(NULL);
	glutSpaceballButtonFunc(NULL);
	glutButtonBoxFunc(NULL);
	glutDialsFunc(NULL);
	glutTabletMotionFunc(NULL);
	glutTabletButtonFunc(NULL);
	glutMenuStateFunc(NULL);
	glutTimerFunc(-1, NULL, 0);

	Spaceshuttledirx.Init();
	Spaceshuttledirx.AddTimeValue(0.0, 0.0);
	Spaceshuttledirx.AddTimeValue(1.0, 0.0);
	Spaceshuttledirx.AddTimeValue(2.0, 0);
	Spaceshuttledirx.AddTimeValue(3.0, 0);
	Spaceshuttledirx.AddTimeValue(4.0, -0.1);
	Spaceshuttledirx.AddTimeValue(5.0, -3);
	Spaceshuttledirx.AddTimeValue(6.0, -6);
	Spaceshuttledirx.AddTimeValue(7.0, -9);
	Spaceshuttledirx.AddTimeValue(8.0, -12);
	Spaceshuttledirx.AddTimeValue(9.0, -15);
	Spaceshuttledirx.AddTimeValue(10.0, -18);
	Spaceshuttledirx.AddTimeValue(11.0, -21);
	Spaceshuttledirx.AddTimeValue(12.0, -27);
	Spaceshuttledirx.AddTimeValue(13.0, -35);
	Spaceshuttledirx.AddTimeValue(14.0, -40);
	Spaceshuttledirx.AddTimeValue(15.0, -45);
	Spaceshuttledirx.AddTimeValue(16.0, -50);
	Spaceshuttledirx.AddTimeValue(17.0, -55);
	Spaceshuttledirx.AddTimeValue(18.0, -60);
	Spaceshuttledirx.AddTimeValue(19.0, -65);
	Spaceshuttledirx.AddTimeValue(20.0, -70);


	Spaceshuttlediry.Init();
	Spaceshuttlediry.AddTimeValue(0.0, 16.5);
	Spaceshuttlediry.AddTimeValue(1.0, 18);
	Spaceshuttlediry.AddTimeValue(2.0, 21);
	Spaceshuttlediry.AddTimeValue(3.0, 24);
	Spaceshuttlediry.AddTimeValue(4.0, 27);
	Spaceshuttlediry.AddTimeValue(5.0, 30);
	Spaceshuttlediry.AddTimeValue(6.0, 33);
	Spaceshuttlediry.AddTimeValue(7.0, 36);
	Spaceshuttlediry.AddTimeValue(8.0, 39);
	Spaceshuttlediry.AddTimeValue(9.0, 39);
	Spaceshuttlediry.AddTimeValue(10.0, 39);
	Spaceshuttlediry.AddTimeValue(11.0, 39);
	Spaceshuttlediry.AddTimeValue(12.0, 39);
	Spaceshuttlediry.AddTimeValue(13.0, 39);
	Spaceshuttlediry.AddTimeValue(14.0, 39);
	Spaceshuttlediry.AddTimeValue(15.0, 39);
	Spaceshuttlediry.AddTimeValue(16.0, 39);
	Spaceshuttlediry.AddTimeValue(17.0, 39);
	Spaceshuttlediry.AddTimeValue(18.0, 39);
	Spaceshuttlediry.AddTimeValue(19.0, 39);
	Spaceshuttlediry.AddTimeValue(20.0, 39);

	Spaceshuttledirz.Init();
	Spaceshuttledirz.AddTimeValue(0.0, 0.00);
	Spaceshuttledirz.AddTimeValue(1.0, 0.00);
	Spaceshuttledirz.AddTimeValue(2.0, 0.0);
	Spaceshuttledirz.AddTimeValue(3.0, 0.0);
	Spaceshuttledirz.AddTimeValue(4.0, 0.0);
	Spaceshuttledirz.AddTimeValue(5.0, 0);
	Spaceshuttledirz.AddTimeValue(6.0, 0);
	Spaceshuttledirz.AddTimeValue(7.0, 0);
	Spaceshuttledirz.AddTimeValue(8.0, 0);
	Spaceshuttledirz.AddTimeValue(9.0, 0);
	Spaceshuttledirz.AddTimeValue(10.0, 0);
	Spaceshuttledirz.AddTimeValue(11.0, 0);
	Spaceshuttledirz.AddTimeValue(12.0, 0);
	Spaceshuttledirz.AddTimeValue(13.0, 0);
	Spaceshuttledirz.AddTimeValue(14.0, 0);
	Spaceshuttledirz.AddTimeValue(15.0, 0);
	Spaceshuttledirz.AddTimeValue(16.0, 0);
	Spaceshuttledirz.AddTimeValue(17.0, 0);
	Spaceshuttledirz.AddTimeValue(18.0, 0);
	Spaceshuttledirz.AddTimeValue(19.0, 0);
	Spaceshuttledirz.AddTimeValue(20.0, 0);


	Zrotation.Init();
	Zrotation.AddTimeValue(0.0, 0.00);
	Zrotation.AddTimeValue(1.0, 0.00);
	Zrotation.AddTimeValue(2.0, 0.0);
	Zrotation.AddTimeValue(3.0, 0.0);
	Zrotation.AddTimeValue(4.0, 18.0);
	Zrotation.AddTimeValue(5.0, 36);
	Zrotation.AddTimeValue(6.0, 54);
	Zrotation.AddTimeValue(7.0, 72);
	Zrotation.AddTimeValue(8.0, 90);
	Zrotation.AddTimeValue(9.0, 90);
	Zrotation.AddTimeValue(10.0, 90);
	Zrotation.AddTimeValue(11.0, 90);
	Zrotation.AddTimeValue(12.0, 90);
	Zrotation.AddTimeValue(13.0, 90);
	Zrotation.AddTimeValue(14.0, 90);
	Zrotation.AddTimeValue(15.0, 90);
	Zrotation.AddTimeValue(16.0, 90);
	Zrotation.AddTimeValue(17.0, 90);
	Zrotation.AddTimeValue(18.0, 90);
	Zrotation.AddTimeValue(19.0, 90);
	Zrotation.AddTimeValue(20.0, 90);
	 
	eyeviewx.Init();
	eyeviewx.AddTimeValue(0.0, 0);
	eyeviewx.AddTimeValue(1.0, 0);
	eyeviewx.AddTimeValue(2.0, 0);
	eyeviewx.AddTimeValue(3.0, 0);
	eyeviewx.AddTimeValue(4.0, 15);
	eyeviewx.AddTimeValue(5.0, 12);
	eyeviewx.AddTimeValue(6.0, 9);
	eyeviewx.AddTimeValue(7.0, 6);
	eyeviewx.AddTimeValue(8.0, 3);
	eyeviewx.AddTimeValue(9.0, 0);
	eyeviewx.AddTimeValue(10.0,-3);
	eyeviewx.AddTimeValue(10.9, -3);
	eyeviewx.AddTimeValue(10.92, -15);
	eyeviewx.AddTimeValue(10.94, 8);
	eyeviewx.AddTimeValue(10.96, -3);
	eyeviewx.AddTimeValue(11.0, -6);
	eyeviewx.AddTimeValue(12.0, -9);
	eyeviewx.AddTimeValue(13.0, -12);
	eyeviewx.AddTimeValue(14.0, -15);
	eyeviewx.AddTimeValue(15.0, -18);
	eyeviewx.AddTimeValue(16.0, -21);
	eyeviewx.AddTimeValue(17.0, -24);
	eyeviewx.AddTimeValue(18.0, -27);
	eyeviewx.AddTimeValue(19.0, -35);
	eyeviewx.AddTimeValue(20.0, -45);

	eyeviewy.Init();
	eyeviewy.AddTimeValue(0.0, 18);
	eyeviewy.AddTimeValue(1.0, 18);
	eyeviewy.AddTimeValue(2.0, 20);
	eyeviewy.AddTimeValue(3.0, 20);
	eyeviewy.AddTimeValue(4.0, 25);
	eyeviewy.AddTimeValue(5.0, 30);
	eyeviewy.AddTimeValue(6.0, 33);
	eyeviewy.AddTimeValue(7.0, 36);
	eyeviewy.AddTimeValue(8.0, 39);
	eyeviewy.AddTimeValue(9.0, 39);
	eyeviewy.AddTimeValue(10.0,39);
	eyeviewy.AddTimeValue(11.0, 39);
	eyeviewy.AddTimeValue(11.01, 20);
	eyeviewy.AddTimeValue(11.2, 80);
	eyeviewy.AddTimeValue(11.3, 20);
	eyeviewy.AddTimeValue(11.4, 80);
	eyeviewy.AddTimeValue(11.5, 20);
	eyeviewy.AddTimeValue(11.6, 80);
	eyeviewy.AddTimeValue(11.7, 20);
	eyeviewy.AddTimeValue(11.8, 80);
	eyeviewy.AddTimeValue(11.9, 20);
	eyeviewy.AddTimeValue(12.0, 39);
	eyeviewy.AddTimeValue(13.0, 39);
	eyeviewy.AddTimeValue(14.0, 39);
	eyeviewy.AddTimeValue(15.0, 39);
	eyeviewy.AddTimeValue(16.0, 39);
	eyeviewy.AddTimeValue(17.0, 39);
	eyeviewy.AddTimeValue(18.0, 39);
	eyeviewy.AddTimeValue(19.0, 39);
	eyeviewy.AddTimeValue(20.0, 39);

	eyeviewz.Init();
	eyeviewz.AddTimeValue(0.0, 20);
	eyeviewz.AddTimeValue(1.0, 20);
	eyeviewz.AddTimeValue(2.0, 20);
	eyeviewz.AddTimeValue(4.0, 0.0);
	eyeviewz.AddTimeValue(5.0, 0.0);
	eyeviewz.AddTimeValue(6.0, 0.0);
	eyeviewz.AddTimeValue(7.0, 0.0);
	eyeviewz.AddTimeValue(8.0, 0.0);
	eyeviewz.AddTimeValue(9.0, 0.0);
	eyeviewz.AddTimeValue(10.0,0.0);
	eyeviewz.AddTimeValue(11.0,0.0);
	eyeviewz.AddTimeValue(12.0,0.0);
	eyeviewz.AddTimeValue(13.0, 0.0);
	eyeviewz.AddTimeValue(14.0, 0.0);
	eyeviewz.AddTimeValue(15.0, 0.0);
	eyeviewz.AddTimeValue(16.0, 0.0);
	eyeviewz.AddTimeValue(17.0, 0.0);
	eyeviewz.AddTimeValue(18.0, 0.0);
	eyeviewz.AddTimeValue(19.0, 0.0);
	eyeviewz.AddTimeValue(20.0, 0.0);


	lookplacex.Init();
	lookplacex.AddTimeValue(0.0, 0.000);
	lookplacex.AddTimeValue(1.0, 0.0);
	lookplacex.AddTimeValue(2.0, 0.0);
	lookplacex.AddTimeValue(3.0, 0.0);
	lookplacex.AddTimeValue(4.0, -0.1);
	lookplacex.AddTimeValue(5.0, -3.0);
	lookplacex.AddTimeValue(6.0, -6.0);
	lookplacex.AddTimeValue(7.0, -9.3);
	lookplacex.AddTimeValue(8.0, -12.0);
	lookplacex.AddTimeValue(9.0, -15.0);
	lookplacex.AddTimeValue(10.0, -18.000);
	lookplacex.AddTimeValue(11.0, -21.000);
	lookplacex.AddTimeValue(12.0, -24.000);
	lookplacex.AddTimeValue(13.0, -27.000);
	lookplacex.AddTimeValue(14.0, -30.000);
	lookplacex.AddTimeValue(15.0, -33.000);
	lookplacex.AddTimeValue(16.0, -36.000);
	lookplacex.AddTimeValue(17.0, -39.000);
	lookplacex.AddTimeValue(18.0, -42.000);
	lookplacex.AddTimeValue(19.0, -50.000);
	lookplacex.AddTimeValue(20.0, -60.000);

	lookplacey.Init();
	lookplacey.AddTimeValue(0.0, 20.000);
	lookplacey.AddTimeValue(1.0, 20.);
	lookplacey.AddTimeValue(2.0, 21);
	lookplacey.AddTimeValue(3.0, 24.);
	lookplacey.AddTimeValue(4.0, 27.0);
	lookplacey.AddTimeValue(5.0, 30.0);
	lookplacey.AddTimeValue(6.0, 33.0);
	lookplacey.AddTimeValue(7.0, 36.0);
	lookplacey.AddTimeValue(8.0, 39.);
	lookplacey.AddTimeValue(9.0, 39.);
	lookplacey.AddTimeValue(10.0, 39);
	lookplacey.AddTimeValue(11.0, 39);
	lookplacey.AddTimeValue(11.01, 20);
	lookplacey.AddTimeValue(11.2, 80);
	lookplacey.AddTimeValue(11.3, 20);
	lookplacey.AddTimeValue(11.4, 80);
	lookplacey.AddTimeValue(11.5, 20);
	lookplacey.AddTimeValue(11.6, 80);
	lookplacey.AddTimeValue(11.7, 20);
	lookplacey.AddTimeValue(11.8, 80);
	lookplacey.AddTimeValue(11.9, 20);
	lookplacey.AddTimeValue(12.0, 39.000);
	lookplacey.AddTimeValue(13.0, 39.000);
	lookplacey.AddTimeValue(14.0, 39.000);
	lookplacey.AddTimeValue(15.0, 39.000);
	lookplacey.AddTimeValue(16.0, 39.000);
	lookplacey.AddTimeValue(17.0, 39.000);
	lookplacey.AddTimeValue(18.0, 39.000);
	lookplacey.AddTimeValue(19.0, 39.000);
	lookplacey.AddTimeValue(20.0, 39.000);

	lookplacez.Init();
	lookplacez.AddTimeValue(0.0, -1.000);
	lookplacez.AddTimeValue(1.0, -2.0);
	lookplacez.AddTimeValue(2.0, 0.0);
	lookplacez.AddTimeValue(4.0, 0.0);
	lookplacez.AddTimeValue(5.0, 0.0);
	lookplacez.AddTimeValue(6.0, 0.0);
	lookplacez.AddTimeValue(7.0, 0.0);
	lookplacez.AddTimeValue(8.0, 0.0);
	lookplacez.AddTimeValue(9.0, 0.0);
	lookplacez.AddTimeValue(10.0, 0.0);
	lookplacez.AddTimeValue(11.0, 0.0);
	lookplacez.AddTimeValue(12.0, 0.0);
	lookplacez.AddTimeValue(13.0, 0.0);
	lookplacez.AddTimeValue(14.0, 0.0);
	lookplacez.AddTimeValue(15.0, 0.0);
	lookplacez.AddTimeValue(16.0, 0.0);
	lookplacez.AddTimeValue(17.0, 0.0);
	lookplacez.AddTimeValue(18.0, 0.0);
	lookplacez.AddTimeValue(19.0, 0.0);
	lookplacez.AddTimeValue(20.0, 0.0);

	ast1x.Init();
	ast1x.AddTimeValue(0.0, -15);
	ast1x.AddTimeValue(1.0, -14);
	ast1x.AddTimeValue(2.0, -8);
	ast1x.AddTimeValue(3.0, -5);
	ast1x.AddTimeValue(4.0, 3.5);
	ast1x.AddTimeValue(5.0, -2.5);
	ast1x.AddTimeValue(6.0, -1.4);
	ast1x.AddTimeValue(7.0, -4.3);
	ast1x.AddTimeValue(8.0, -4.2);
	ast1x.AddTimeValue(9.0, -4.1);
	ast1x.AddTimeValue(10.0, -4);
	ast1x.AddTimeValue(11.0, -3.5);
	ast1x.AddTimeValue(12.0, -3);
	ast1x.AddTimeValue(13.0, -2.9);
	ast1x.AddTimeValue(14.0, -2.8);
	ast1x.AddTimeValue(15.0, -2.7);
	ast1x.AddTimeValue(16.0, -2.6);
	ast1x.AddTimeValue(17.0, -2.5);
	ast1x.AddTimeValue(18.0, -2.4);
	ast1x.AddTimeValue(19.0, -2.3);
	ast1x.AddTimeValue(20.0, -1);

	ast1y.Init();
	ast1y.AddTimeValue(0.0, 20);
	ast1y.AddTimeValue(1.0, -25);
	ast1y.AddTimeValue(2.0, 30);
	ast1y.AddTimeValue(3.0, 40);
	ast1y.AddTimeValue(4.0, 35);
	ast1y.AddTimeValue(5.0, 36.5);
	ast1y.AddTimeValue(6.0, 34);
	ast1y.AddTimeValue(7.0, 33);
	ast1y.AddTimeValue(8.0, 30);
	ast1y.AddTimeValue(9.0, 33);
	ast1y.AddTimeValue(10.0, 34);
	ast1y.AddTimeValue(11.0, 36);
	ast1y.AddTimeValue(12.0, 36);
	ast1y.AddTimeValue(13.0, 38);
	ast1y.AddTimeValue(14.0, 39);
	ast1y.AddTimeValue(15.0, 40);
	ast1y.AddTimeValue(16.0, 40);
	ast1y.AddTimeValue(17.0, 38);
	ast1y.AddTimeValue(18.0, 39);
	ast1y.AddTimeValue(19.0, 37);
	ast1y.AddTimeValue(20.0, 36);

	ast2x.Init();
	ast2x.AddTimeValue(0.0, -20);
	ast2x.AddTimeValue(1.0, -18);
	ast2x.AddTimeValue(2.0, -17);
	ast2x.AddTimeValue(3.0, -16);
	ast2x.AddTimeValue(4.0, -15);
	ast2x.AddTimeValue(5.0, -18);
	ast2x.AddTimeValue(6.0, -17);
	ast2x.AddTimeValue(7.0, -16);
	ast2x.AddTimeValue(8.0, -11);
	ast2x.AddTimeValue(9.0, -10);
	ast2x.AddTimeValue(10.0, -9);
	ast2x.AddTimeValue(11.0, -8);
	ast2x.AddTimeValue(12.0, -7);
	ast2x.AddTimeValue(13.0, -7);
	ast2x.AddTimeValue(14.0, -7);
	ast2x.AddTimeValue(15.0, -6);
	ast2x.AddTimeValue(16.0, -5);
	ast2x.AddTimeValue(17.0, -4);
	ast2x.AddTimeValue(18.0, -3);
	ast2x.AddTimeValue(19.0, -2);
	ast2x.AddTimeValue(20.0, -1);

	ast2y.Init();
	ast2y.AddTimeValue(0.0, 20);
	ast2y.AddTimeValue(1.0, 28);
	ast2y.AddTimeValue(2.0, 35);
	ast2y.AddTimeValue(3.0, 40);
	ast2y.AddTimeValue(4.0, 42);
	ast2y.AddTimeValue(5.0, 43);
	ast2y.AddTimeValue(6.0, 47);
	ast2y.AddTimeValue(7.0, 50);
	ast2y.AddTimeValue(8.0, 45);
	ast2y.AddTimeValue(9.0, 43);
	ast2y.AddTimeValue(10.0, 41);
	ast2y.AddTimeValue(11.0, 40);
	ast2y.AddTimeValue(12.0, 39);
	ast2y.AddTimeValue(13.0, 38);
	ast2y.AddTimeValue(14.0, 37);
	ast2y.AddTimeValue(15.0, 36);
	ast2y.AddTimeValue(16.0, 35);
	ast2y.AddTimeValue(17.0, 34);
	ast2y.AddTimeValue(18.0, 33);
	ast2y.AddTimeValue(19.0, 32);
	ast2y.AddTimeValue(20.0, 31);

    lightdir.Init();
	lightdir.AddTimeValue(0.0, 7);
	lightdir.AddTimeValue(1.0, 10);
	lightdir.AddTimeValue(2.0, 35);
	lightdir.AddTimeValue(3.0, 40);
	lightdir.AddTimeValue(4.0, 60);
   
    
    lightdirx.Init();
	lightdirx.AddTimeValue(0.0, 0);
	lightdirx.AddTimeValue(1.0, 0);
	lightdirx.AddTimeValue(2.0, 0);
	

    
	

    







	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

	glutIdleFunc(Animate);

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "glewInit Error\n");
	}
	else
		fprintf(stderr, "GLEW initialized OK\n");
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif
	int width, height;
	char* file = (char*)"earth.bmp";
	unsigned char* texture = BmpToTexture(file, &width, &height);
	if (texture == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", file);
	else
		fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", file, width, height);

	glGenTextures(1, &EarthTex);
	glBindTexture(GL_TEXTURE_2D, EarthTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

	// all other setups go here, such as GLSLProgram and KeyTime setups:
    
   

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists()
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitLists.\n");

	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow(MainWindow);

	// create the object:
	Toruslist = glGenLists(1);
	glNewList(Toruslist, GL_COMPILE);
	OsuTorus(1, 8, 50, 50);
	glEndList();

	ltsource = glGenLists(1);
	glNewList(ltsource, GL_COMPILE);
	glNormal3f(0., 1., 0.);
	OsuSphere(0.25f, 50, 50);
	glEndList();

	SpaceshuttleDL = glGenLists(1);
	glNewList(SpaceshuttleDL, GL_COMPILE);
    SetMaterial(0.f, 1.f, 1.f, 30.f);
    glPushMatrix();
	glNormal3f(0., 1., 0.);
	LoadObjFile((char*)"Spaceship.obj");
    glPopMatrix();
	glEndList();

	Spherelist = glGenLists(1);
	glNewList(Spherelist, GL_COMPILE);
	OsuSphere(7., 200, 200);
	glEndList();

	asteroidlist = glGenLists(1);
	glNewList(asteroidlist, GL_COMPILE);
    glScalef(1.00f, 1.7f,0.7f );
	OsuSphere(0.9, 5, 5);
	glEndList();
    
	EarthDL = glGenLists(1);
	glNewList(EarthDL, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, EarthTex);
	glPushMatrix();
	glScalef(1.00f, 1.00f, 1.00f);	// scale of earth sphere, from the table
	glCallList(Spherelist);		// a dl can call another dl that has been previously created
	glPopMatrix();
	glEndList();



	GridDL = glGenLists(1);
	glNewList(GridDL, GL_COMPILE);
	glNormal3f(0., 1., 0.);
	for (int i = 0; i < NZ; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < NX; j++)
		{
			glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 0));
			glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 1));
		}
		glEnd();
	}
	glEndList();



	// create the axes:

	AxesList = glGenLists(1);
	glNewList(AxesList, GL_COMPILE);
	glLineWidth(AXES_WIDTH);
	Axes(1.5);
	glLineWidth(1.);
	glEndList();
}


// the keyboard callback:

void
Keyboard(unsigned char c, int x, int y)
{
	if (DebugOn != 0)
		fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

	switch (c)
	{
	case 'o':
	case 'O':
		NowProjection = ORTHO;
		break;

	case 'l':
	case 'L':
		lightMode = (lightMode + 1) % 2;
		break;
	case 'p':
	case 'P':
		lightstate = 1;
		break;
	case 'S':
	case 's':
		lightstate = 0;
		break;

	case 't':
	case 'T':
		textureMode = (textureMode + 1) % 2;
		break;

	case 'q':
	case 'Q':
	case ESCAPE:
		DoMainMenu(QUIT);	// will not return here
		break;				// happy compiler

	default:
		fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
	}

	// force a call to Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// called when the mouse button transitions down or up:

void
MouseButton(int button, int state, int x, int y)
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if (DebugOn != 0)
		fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);


	// get the proper button bit mask:

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;

	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;

	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;

	case SCROLL_WHEEL_UP:
		Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	case SCROLL_WHEEL_DOWN:
		Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		break;

	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}

	// button down sets the bit, up clears the bit:

	if (state == GLUT_DOWN)
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion(int x, int y)
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT * dy);
		Yrot += (ANGFACT * dx);
	}

	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset()
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale = 1.0;
	ShadowsOn = 0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize(int width, int height)
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// handle a change to the window's visibility:

void
Visibility(int state)
{
	if (DebugOn != 0)
		fprintf(stderr, "Visibility: %d\n", state);

	if (state == GLUT_VISIBLE)
	{
		glutSetWindow(MainWindow);
		glutPostRedisplay();
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = { 0.f, 1.f, 0.f, 1.f };

static float xy[] = { -.5f, .5f, .5f, -.5f };

static int xorder[] = { 1, 2, -3, 4 };

static float yx[] = { 0.f, 0.f, -.5f, .5f };

static float yy[] = { 0.f, .6f, 1.f, 1.f };

static int yorder[] = { 1, 2, 3, -2, 4 };

static float zx[] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes(float length)
{
	glBegin(GL_LINE_STRIP);
	glVertex3f(length, 0., 0.);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., length, 0.);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., length);
	glEnd();

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 4; i++)
	{
		int j = xorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 5; i++)
	{
		int j = yorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 6; i++)
	{
		int j = zorder[i];
		if (j < 0)
		{

			glEnd();
			glBegin(GL_LINE_STRIP);
			j = -j;
		}
		j--;
		glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
	}
	glEnd();

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb(float hsv[3], float rgb[3])
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while (h >= 6.)	h -= 6.;
	while (h < 0.) 	h += 6.;

	float s = hsv[1];
	if (s < 0.)
		s = 0.;
	if (s > 1.)
		s = 1.;

	float v = hsv[2];
	if (v < 0.)
		v = 0.;
	if (v > 1.)
		v = 1.;

	// if sat==0, then is a gray:

	if (s == 0.0)
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:

	float i = (float)floor(h);
	float f = h - i;
	float p = v * (1.f - s);
	float q = v * (1.f - s * f);
	float t = v * (1.f - (s * (1.f - f)));

	float r = 0., g = 0., b = 0.;			// red, green, blue
	switch ((int)i)
	{
	case 0:
		r = v;	g = t;	b = p;
		break;

	case 1:
		r = q;	g = v;	b = p;
		break;

	case 2:
		r = p;	g = v;	b = t;
		break;

	case 3:
		r = p;	g = q;	b = v;
		break;

	case 4:
		r = t;	g = p;	b = v;
		break;

	case 5:
		r = v;	g = p;	b = q;
		break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit(float v[3])
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}
