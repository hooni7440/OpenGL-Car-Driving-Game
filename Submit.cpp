// Ass2.cpp : Defines the entry point for the console application.
//

#include <stdlib.h>
#include <stdio.h>
#include <gl\glut.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include <vector>
#include "bitmap.h"
using namespace std;

const float PI = 3.14159265;

int winWidth = 1000;
int winHeight = 800;

float keyboardCameraMoveSpeed(10.0);
float mouseCameraMoveSpeed(0.5);
float mouseCameraMoveDirection[2] = {0, 0};

float light_X(-500), light_Y(300), light_Z(-1000);

float cam_X(0), cam_Y(350), cam_Z(800);
float cam_ViewX(0), cam_ViewY(0), cam_ViewZ(0);

GLfloat mat_diffuse[] = {0.8,0.2,0.5,1.0};
GLfloat mat_specular[] = {1.0,1.0,1.0,1.0};
GLfloat high_shininess[] = {100.0};

float groundWidth(1600.0), groundLong(1800.0);


//Load the BMP file
GLubyte* TextureLoadBitmap(char *filename, int *w, int *h)		/* I - Bitmap file to load */
{
	BITMAPINFO	*info;				/* Bitmap information */
	void		*bits;				/* Bitmap pixel bits */
	GLubyte	*rgb;				/* Bitmap RGB pixels */
	GLubyte   err = '0';

	/*
	* Try loading the bitmap and converting it to RGB...
	*/

	bits = LoadDIBitmap(filename, &info);
	if(bits==NULL) 
		return(NULL);
	rgb = ConvertRGB(info, bits);
	if (rgb == NULL)
	{
		free(info);
		free(bits);
	};

	printf("%s: %d %d\n", filename, info->bmiHeader.biWidth, info->bmiHeader.biHeight);
	printf("read %s successfully\n", filename);
	*w = info->bmiHeader.biWidth;
	*h = info->bmiHeader.biHeight;

	/*
	* Free the bitmap and RGB images, then return 0 (no errors).
	*/

	free(info);
	free(bits);
	return (rgb);

}

void init(void) // All Setup For OpenGL Goes Here
{
	// Light 0 Settings
	static GLfloat light0pos[] = {200.f, 100.f, 400.f, 0.f};
	static GLfloat light0_mat1[] = {0.8, 0.8, 0.8, 1.f};
	static GLfloat light0_diff1[] = {0.9, 0.9, 0.9, 1.f};
	glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_mat1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diff1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);		// Cull all the back face
	glFrontFace(GL_CCW);		// Define Counter-Clockwise as front face
	
	glEnable(GL_COLOR_MATERIAL);

	// Texture mapping setting for Microsoft's OpenGL implementation
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

	// Texture mapping parameters for filter and repeatance
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/* Add code here to initialize lighting, read bitmap images, initialize different GLU geometry
	* use glLight, TextureLoadBitmap, gluNewQuadric...etc
	*/
}

// Move camera to specified position without changing view angle
void moveCameraTo(float newCamX, float newCamY, float newCamZ)
{
	cam_ViewX = (cam_ViewX - cam_X) + newCamX;
	cam_ViewY = (cam_ViewY - cam_Y) + newCamY;
	cam_ViewZ = (cam_ViewZ - cam_Z) + newCamZ;
	cam_X = newCamX;
	cam_Y = newCamY;
	cam_Z = newCamZ;
	printf("moveCameraTo: %f, %f, %f; %f, %f, %f\n", cam_X,cam_Y,cam_Z, cam_ViewX, cam_ViewY, cam_ViewZ);
}

// Move camera by magnitude of movement
void moveCameraBy(float deltaX, float deltaY, float deltaZ)
{
	moveCameraTo(cam_X + deltaX, cam_Y + deltaY, cam_Z + deltaZ);
}

void updateCamera() 
{
	gluLookAt(cam_X,cam_Y,cam_Z, cam_ViewX, cam_ViewY, cam_ViewZ, 0, 1, 0);
	moveCameraBy(mouseCameraMoveSpeed * mouseCameraMoveDirection[0], 0, mouseCameraMoveSpeed * mouseCameraMoveDirection[1]);
}

void drawOrigin()
{
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	{
		glVertex4f(0.0f, 0.0f, 0.0f, 1.0f);
		glVertex4f(0.0f, 1.0f, 0.0f, 0.0f);
	}
	glEnd();
	glBegin(GL_LINES);
	{
		glVertex4f(0.0f, 0.0f, 0.0f, 1.0f);
		glVertex4f(1.0f, 0.0f, 0.0f, 0.0f);
	}
	glEnd();
	glBegin(GL_LINES);
	{
		glVertex4f(0.0f, 0.0f, 0.0f, 1.0f);
		glVertex4f(0.0f, 0.0f, 1.0f, 0.0f);
	}
	glEnd();
	glPopMatrix();
}

void drawGround()
{
	glPushMatrix();
	glScalef(groundWidth, 0.1f, groundLong);
	glColor3f(0.55f, 0.65f, 0.8f);
	glutSolidCube(1.0f);
	glPopMatrix();
}

void display(void) // Here's Where We Do All The Drawing
{
	/* clear the window color before drawing is performed */
	glClearColor(0.0, 0.0, 0.0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	/* Add code here to transform the car and the ground & draw them
	* use glTranslate, glRotate, glLoadIdentity, glPushMatrix, glPopMatrix, glMaterial, 
	* glBegin, gluSphere...etc
	*
	* Add code for Texture Mapping for the car and the ground
	* use glTexImage2D..
	*/

	// Default material
	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,high_shininess);

	glLoadIdentity();
	updateCamera();

	// TODO:
	// Draw grounds and objects here
	drawOrigin();
	drawGround();

	glFlush();
	glutSwapBuffers();
}


void reshape(int w, int h) // Resize the GL Window. w=width, h=height
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45, (float)w/(float)h, 100, 2000); 

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

/* keyboard CALLBACK for handling special keyboard event */
void keyspecial(int key, int x, int y)
{	
	/* Add code here to control the animation interactively */

	switch (key) 
	{
		// car control
		case GLUT_KEY_UP: // move front
			moveCameraBy(0, 0, -keyboardCameraMoveSpeed);
			break;
		case GLUT_KEY_DOWN: // move back
			moveCameraBy(0, 0, keyboardCameraMoveSpeed);
			break;
		case GLUT_KEY_LEFT: // move left
			moveCameraBy(-keyboardCameraMoveSpeed, 0, 0);
			break;
		case GLUT_KEY_RIGHT: // move right
			moveCameraBy(keyboardCameraMoveSpeed, 0, 0);
			break;
	}
}

/* keyboard up CALLBACK for handling special keyboard event */
void keyspecialup(int key, int x, int y)
{	
	/* Add code here to control the animation interactively */

	switch (key) 
	{
		// car control
		case GLUT_KEY_UP: // move front
			break;
		case GLUT_KEY_DOWN: // move back
			break;
		case GLUT_KEY_LEFT: // move left
			break;
		case GLUT_KEY_RIGHT: // move right
			break;
	}
}

/* keyboard CALLBACK for handling keyboard event */
void keyboard(unsigned char key, int x, int y)
{
	/* Add code here to control the animation interactively */

	switch (key) 
	{
		case 'r': // Reset all parameters
			break;
	}
}

void mouseclick(int button, int state, int x, int y) // Handle the mouse click events here
{
	printf("Button %d, State %d, Position %d, %d \n", button, state, x, y);	
}

void mousemove(int x, int y) // Handle the mouse movement events here
{
	printf("Mouse moved to position %d %d \n", x, y);
}

void timer(int t)
{
	/* Add code here to update the velocity, acceleration, position and rotation of car and wheels */

	// display after update and reset timer
	glutPostRedisplay();
    glutTimerFunc(10, timer, 1);
}

void drag(int ix, int iy)
{

}

void main(int argc, char** argv)
{
	/*Initialization of GLUT Library */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);

	/*Create a window with title specified */
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Assignment 2");

	/*Register different CALLBACK function for GLUT to response 
	with different events, e.g. window sizing, mouse click or
	keyboard stroke */
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyspecial);
	glutSpecialUpFunc(keyspecialup);
	// call timer function every 10 ms
	glutTimerFunc(10, timer, 1);
	glutMouseFunc(mouseclick);
	glutMotionFunc(drag);
	init();	/*not GLUT call, initialize several parameters */

	/*Enter the GLUT event processing loop which never returns.
	it will call different registered CALLBACK according
	to different events. */
	glutMainLoop();
}
