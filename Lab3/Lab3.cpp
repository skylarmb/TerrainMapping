// Lab3.cpp : Terrain Mapping
//

#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <array>
#include "ppm_canvas.h"
#include "util.h"
#include "time.h"

#define PI 3.14159265
int g_argc;
char** g_argv;
float xrot, yrot = 0;
bool keyState[256] = { false };
bool use_dl_bool = true, vnormals_bool = true;
int windowWidth, windowHeight;
int angle = 2;
canvas_t terrain;
canvas_t skin;
GLuint texture;
GLuint world;

int eyex = 30,eyey=30,eyez=30,centerx=50,centerz = 50;
//int res = 2;

float verts[512][512][3];
float fnormals[512][512][2][3];
float vnormals[512][512][3];

float lightPosition[4];
float diffuseColour[3];
float ambientColour[3];

float l0x = 30, l0y = 30, l0z = 30, l1x = 30,l1y = 30, l1z = 30;

int getHeight(int x, int y){
	pixel_t pixel = PIXEL(terrain, x, y);
	int h = RED(pixel);
	//printf("%d\n",h);
	return h;
}

float* cross(float *v1, float *v2)
{
	float product[3];

	//Cross product formula 
	product[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	product[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	product[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);

	return product;
}

float* normalize(float* vect){
	float x = vect[0];
	float y = vect[1];
	float z = vect[2];

	float length = sqrt((x * x) + (y * y) + (z * z));

	float result[3];

	result[0] = x / length;
	result[1] = y / length;
	result[2] = z / length;

	//printf("x: %25f y: %25f z: %25f\n", result[0], result[1], result[2]);
	return result;
}

float* normal(float* p1, float* p2, float* p3){

	float v1[3], v2[3];

	v1[0] = p3[0] - p1[0];
	v1[1] = p3[1] - p1[1];
	v1[2] = p3[2] - p1[2];

	v2[0] = p2[0] - p1[0];
	v2[1] = p2[1] - p1[1];
	v2[2] = p2[2] - p1[2];

	float normalvect[3] = {cross(v1,v2)[0],cross(v1,v2)[1],cross(v1,v2)[2]};

	//printf("in normal x: %f y: %f z: %f\n",normalvect[0],normalvect[1],normalvect[2]);
	normalize(normalvect);

	return normalvect;
}

float* addVect(float* a, float* b){
	float result[3];
	result[0] = a[0] + b[0];
	result[1] = a[1] + b[1];
	result[2] = a[2] + b[2];
	return result;
}


void calcFaceNormals(float xzscale, float yscale, int res){
	float* p1,* p2,* p3,* p4,* t1n,* t2n;
	for(int x = 0; x < terrain.width-res; x+=res){
		for(int y = 0; y < terrain.height-res; y+=res){

			//store verts in array
			verts[x][y][0] = (x)/xzscale;
			verts[x][y][1] = getHeight(x,y)/yscale;
			verts[x][y][2] = (y)/xzscale;

			verts[x+res][y+res][0] = (x+res)/xzscale;
			verts[x+res][y+res][1] = getHeight(x+res,y+res)/yscale;
			verts[x+res][y+res][2] = (y+res)/xzscale;

			verts[x+res][y][0] = (x+res)/xzscale;
			verts[x+res][y][1] = getHeight(x+res,y)/yscale;
			verts[x+res][y][2] = (y)/xzscale;

			verts[x][y+res][0] = (x)/xzscale;
			verts[x][y+res][1] = getHeight(x,y+res)/yscale;
			verts[x][y+res][2] = (y+res)/xzscale;

			//get points from array of verts
			p1 = verts[x][y];
			p2 = verts[x+res][y];
			p3 = verts[x+res][y+res];
			p4 = verts[x][y+res];

			t1n = normal(p1,p2,p3);
			t2n = normal(p1,p2,p4);
			//store normal for triangle 0
			fnormals[x][y][0][0] = t1n[0];
			fnormals[x][y][0][1] = t1n[1];
			fnormals[x][y][0][2] = t1n[2];

			//store normal for triangle 1
			fnormals[x][y][1][0] = t2n[0];
			fnormals[x][y][1][1] = t2n[1];
			fnormals[x][y][1][2] = t2n[2];

		}
	}
}

void calcVertNormals(float xzscale, float yscale, int res){
	int xpr,ypr,xmr,ymr;
	float result[3];

	for(int x = 0; x < terrain.width-res; x+=res){
		for(int y = 0; y < terrain.height-res; y+=res){
			xpr = x+res;
			ypr = y+res;
			xmr = x-res;
			ymr = y-res;

			result[0] = 0;
			result[1] = 0;
			result[2] = 0;

			if(ypr <= terrain.height && xpr <= terrain.height){
				result[0] = result[0] + fnormals[x][y][0][0]; //normal 1
				result[1] = result[1] + fnormals[x][y][0][1];
				result[2] = result[2] + fnormals[x][y][0][2];

				result[0] = result[0] + fnormals[x][y][1][0]; //and 2
				result[1] = result[1] + fnormals[x][y][1][1];
				result[2] = result[2] + fnormals[x][y][1][2];
			}

			if(ymr >= 0 && xpr <= terrain.width){
				result[0] = result[0] + fnormals[x][ymr][1][0]; //normal 3
				result[1] = result[1] + fnormals[x][ymr][1][1];
				result[2] = result[2] + fnormals[x][ymr][1][2];
			}
			if(xmr >= 0 && ymr >= 0){
				result[0] = result[0] + fnormals[xmr][ymr][0][0]; //normal 4 
				result[1] = result[1] + fnormals[xmr][ymr][0][1];
				result[2] = result[2] + fnormals[xmr][ymr][0][2];

				result[0] = result[0] + fnormals[xmr][ymr][1][0]; // and 5
				result[1] = result[1] + fnormals[xmr][ymr][1][1];
				result[2] = result[2] + fnormals[xmr][ymr][1][2];
			}
			if(xmr >= 0 && ypr <= terrain.height){
				result[0] = result[0] + fnormals[xmr][y][0][0]; //normal 6
				result[1] = result[1] + fnormals[xmr][y][0][1];
				result[2] = result[2] + fnormals[xmr][y][0][2];
			}
			normalize(result);
			vnormals[x][y][0] = result[0];
			vnormals[x][y][1] = result[1];
			vnormals[x][y][2] = result[2];
		}
	}
}

void drawTerrain(){

	float res = 1;
	float xzscale = 5;
	float yscale = 15;
	int xpr, ypr;
	calcFaceNormals(xzscale,yscale,res);
	calcVertNormals(xzscale,yscale,res);
	
	glEnable(GL_TEXTURE_2D);   
	glBindTexture(GL_TEXTURE_2D, texture);
	
	for(int x = 0; x < terrain.width-res; x+=res){
		for(int y = 0; y < terrain.height-res; y+=res){
			xpr = x+res;
			ypr = y+res;

			

			if(vnormals_bool) 
				glNormal3f(vnormals[x][y][0],vnormals[x][y][1],vnormals[x][y][2]); //set vertex normal p1
			else 
				glNormal3f(fnormals[x][y][0][0], fnormals[x][y][0][1], fnormals[x][y][0][2]); //else set surface normal for t1

			glTexCoord2f((float)x/(float)512,(float)y/(float)512); //texture
			glVertex3f(verts[x][y][0],verts[x][y][1],verts[x][y][2]);//make p1

			if(vnormals_bool) 
				glNormal3f(vnormals[xpr][y][0],vnormals[xpr][y][1],vnormals[xpr][y][2]); //set vertex normal p2

			glTexCoord2f((float)xpr/(float)512,(float)y/(float)512); //texture
			glVertex3f(verts[xpr][y][0],verts[xpr][y][1],verts[xpr][y][2]);//make p2

			if(vnormals_bool) 
				glNormal3f(vnormals[xpr][ypr][0],vnormals[xpr][ypr][1],vnormals[xpr][ypr][2]); //set vertex normal p3

			glTexCoord2f((float)xpr/(float)512,(float)ypr/(float)512); //texture
			glVertex3f(verts[xpr][ypr][0],verts[xpr][ypr][1],verts[xpr][ypr][2]);//make p3


			//triangle 2
			if(vnormals_bool) 
				glNormal3f(vnormals[x][y][0],vnormals[x][y][1],vnormals[x][y][2]); //set vertex normal p1
			else 
				glNormal3f(fnormals[x][y][1][0], fnormals[x][y][1][1], fnormals[x][y][1][2]); //else set surface normal for t2
			glTexCoord2f((float)x/(float)512,(float)y/(float)512); //texture
			glVertex3f(verts[x][y][0],verts[x][y][1],verts[x][y][2]);//make p1

			if(vnormals_bool) 
				glNormal3f(vnormals[xpr][ypr][0],vnormals[xpr][ypr][1],vnormals[xpr][ypr][2]); //set vertex normal p3
			glTexCoord2f((float)xpr/(float)512,(float)ypr/(float)512); //texture
			glVertex3f(verts[xpr][ypr][0],verts[xpr][ypr][1],verts[xpr][ypr][2]);//make p3

			if(vnormals_bool) 
				glNormal3f(vnormals[x][ypr][0],vnormals[x][ypr][1],vnormals[x][ypr][2]); //set vertex normal p4
			glTexCoord2f((float)x/(float)512,(float)ypr/(float)512); //texture
			glVertex3f(verts[x][ypr][0],verts[x][ypr][1],verts[x][ypr][2]); //make p4

		}
	}	
	glDisable(GL_TEXTURE_2D);
}

void cb_display() {
		///*
	static clock_t previous_clock = 0;
	static clock_t ms_since_last = 0;
	clock_t now = clock()/(CLOCKS_PER_SEC / 1000);
	ms_since_last = now - previous_clock;
	if(ms_since_last < 20) return;
	previous_clock = now;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(eyex,eyey,eyez,centerx,0,centerz,0,1,0);

	//Add ambient light
	GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color (0.3, 0.3, 0.3)
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	//Add positioned light
	GLfloat lightColor0[] = {0.9f,0.9f, 0.9f, 1.0f}; //Color (0.5, 0.5, 0.5)
	GLfloat lightPos0[] = {l0x, l0y, l0z, 1.0f}; //Positioned at (4, 0, 8)
	glPushMatrix();
	glTranslatef(l0x, l0y, l0z);
	draw_axis(3.0);
	glPopMatrix();
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	glBegin(GL_TRIANGLES);
	if(use_dl_bool) glCallList(world);
	else drawTerrain();
	glEnd();
	draw_axis(4.0);
	glFlush();
	glutSwapBuffers(); // for smoother animation
}

void loadTextureMap(canvas_t tex) {
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.height, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.pixels);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
}

void cb_idle() {	
	glutPostRedisplay();
}

void cb_reshape(int w, int h) {
	windowWidth = w;
	windowHeight = h;
	if(windowWidth == 0 || windowHeight == 0) exit(0);
	int aspect = w/h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, aspect, 0.1, 200); // necessary to preserve aspect ratio
	glMatrixMode(GL_MODELVIEW);
}

void cb_mouseclick(int button, int state, int x, int y) {
}

void cb_mousemove(int x, int y) {
}

static void init(void){
	world = glGenLists (1);
	glNewList(world, GL_COMPILE);
	drawTerrain();
	glEndList();
}

void callback_keyboard(unsigned char key, int x, int y) {
	switch(key) {
	case 'a':
		l0z -= 5;
		break;
	case 'd':
		l0z += 5;
		break;
	case 'w':
		l0x += 5;
		break;
	case 's':
		l0x -= 5;
		break;
	case 'q':
		l0y -= 5;
		break;
	case 'e':
		l0y += 5;
		break;
	case 'r':
		eyex -= 5;
		break;
	case 'y':
		eyex += 5;
		break;
	case 'f':
		eyez -= 5;
		break;
	case 'h':
		eyez += 5;
		break;
	case 't':
		eyey += 5;
		break;
	case 'g':
		eyey -= 5;
		break;
	case 'v':
		vnormals_bool= !vnormals_bool;
		init();
		break;

	case 'b':
		use_dl_bool = !use_dl_bool;
		break;
	}
}

int main(int argc, char** argv) {
	g_argc = argc;
	g_argv = argv;
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Lab 3: Terrain Mapping");
	glutIgnoreKeyRepeat(true);

	glEnable(GL_NORMALIZE); //Automatically normalize normals
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (argc == 3) {
		canvas_t height;
		printf("Loading file '%s'... ", argv[1]);
		ppmLoadCanvas(argv[1], &height);
		printf("Done.\n");
		canvas_t texture;
		printf("Loading file '%s'... ", argv[2]);
		ppmLoadCanvas(argv[2], &texture);
		loadTextureMap(texture);
		terrain = height;
		skin = texture;

	} else {
		printf("Usage: %s terrain.ppm texture.ppm\n", argv[0]);
		return 1;
	}

	printf("\n\nQWEASD: Light controls\nRTYFGH: Camera Controls\n V: Enable/disable vertex normals\nB: Enable/disable display lists\n");

	//drawTerrain();
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable (GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//glEnable(GL_LIGHT1); //Enable light #1
	glShadeModel(GL_SMOOTH); //Enable smooth shading
	glEnable(GL_COLOR_MATERIAL);

	//lightPosition[0] = 0;
	//lightPosition[1] = 0;
	//lightPosition[2] = 50;
	init();
	glutDisplayFunc(cb_display);
	glutReshapeFunc(cb_reshape);
	glutIdleFunc(cb_idle);
	glutKeyboardFunc(callback_keyboard);

	glClearColor(0,0,0,0); // set background to black
	glutMainLoop();

	return 0;
}


