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

//terrain variables
#define PI 3.14159265
int g_argc;
char** g_argv;
float xrot, yrot = 0;
bool keyState[256] = { false };
bool use_dl_bool = true, vnormals_bool = true;
int windowWidth, windowHeight;
int angle = 2;
const int size = 256;
canvas_t terrain;
canvas_t skin;
GLuint texture;
GLuint world;
float eyex = 15,eyey=15,eyez=0,centerx=50,centerz = 50;
float verts[size][size][3];
float fnormals[size][size][2][3];
float vnormals[size][size][3];
float lightPosition[4];
float diffuseColour[3];
float ambientColour[3];

//animal variables
bool is_spinning = false;
bool g_debug = false;
bool anim = false;
float neck = -45;
float shoulder = 35;
double fronthip = -30;
double backhip = -30;
double frontknee = -60;
double backknee = -60;
double frontankle = -60;
double backankle = -60;
int neg = 2;
int neg2 = 2;
int reverseAnim1 = 1;
int reverseAnim2 = 1;
int reverseAnim3 = 1;
int numAnimals = 1;
bool lookAtAnimal = true;

float animal1x=45;
float animal1y=18;

float animal2x=8;
float animal2y=32;

float animal3x=12;
float animal3y=26;

int whichAnimal = 0;
float l0x = 30, l0y = 30, l0z = 30, l1x = 30,l1y = 30, l1z = 30;

//animal code

void spin() {
	if (is_spinning) {
		is_spinning = false;
		angle = 0.0;
	} else {
		is_spinning = true;
	}
}

void drawDA(){
	if(g_debug){
		glBegin(GL_LINES);
		glColor3f(255,255,255);
		glVertex3f(0,0,0);
		glVertex3f(1,0,0);
		glVertex3f(0,0,0);
		glVertex3f(0,1,0);
		glVertex3f(0,0,0);
		glVertex3f(0,0,1);
		glEnd();
	}
}

void resetJoints(){
	neck = -45;
	shoulder = 35;
	fronthip = -30;
	backhip = -30;
	frontknee = -60;
	backknee = -60;
	frontankle = -60;
	backankle = -60;
}

void resetAnimJoints(){
	fronthip = -30;
	backhip = 30;
	frontknee = -60;
	backknee = -10;
}

void drawBody(){
	drawDA();
	glColor3f(0,255,0);
	glPushMatrix();
	glScalef(4.0,2,2);
	glutSolidCube(1);
	glPopMatrix();
}

void drawHead(){
	//drawDA();
	glColor3f(0,255,255);
	glPushMatrix();
	glTranslatef (2.0, 1.0, 0.0);
	glRotatef ((GLfloat) shoulder, 0.0, 0.0, 1.0);
	glTranslatef (.5, 0.0, 0.0);
	glPushMatrix();
	glScalef (2.0, 1, 1.0);
	glutSolidCube (1.0);
	glPopMatrix();


	glTranslatef (1.0, 0.25, 0.0);
	glRotatef ((GLfloat) neck, 0.0, 0.0, 1.0);
	glTranslatef (0.25, -.125, 0.0);
	glPushMatrix();
	glScalef (1.5, .5, 1.0);
	glutSolidCube (1.0);
	glPopMatrix();
	drawDA();
	glPushMatrix();
	glTranslatef(-.5,.25,-.25);
	glRotatef(-90,1,0,0);

	glutSolidCone(.25,.5,5,5);
	glPushMatrix();
	glTranslatef(.5,.125,0);
	glutSolidSphere(.125,5,5);
	glPopMatrix();

	glTranslatef(0,-.5,0);
	glutSolidCone(.25,.5,5,5);

	glTranslatef(.5,-.125,0);
	glutSolidSphere(.125,5,5);
	glPopMatrix();

	glPopMatrix();
}

void drawThighs(double x,double y,double z, float ihip){

	double radius = .5;
	//double cubesize;
	drawDA();
	glPushMatrix();
	//sphere
	glColor3f(255,0,0);
	glTranslatef(x,y,z);
	glRotatef(-ihip,0,0,1);
	glutSolidSphere(radius,10,10);


	glTranslatef(0,-1,0);
	glPushMatrix();
	glScalef(1,3,1);
	glutSolidCube(.5);
	glPopMatrix();


}

void drawFeet(double iankle){
	drawDA();
	glColor3f(255,255,0);
	glTranslatef(0,-.75,0);
	glRotatef(iankle,0,0,1);
	glTranslatef(0,.5,0);
	drawDA();
	glScalef(.5,1.5,1);
	glutSolidCube(.5);
	glPopMatrix();
}

void drawCalves(double iknee, double iankle){
	drawDA();
	glColor3f(0,0,255);
	glTranslatef(0,-.75,0);

	glRotatef(iknee,0,0,1);
	glTranslatef(0,-.75,0);

	glPushMatrix();
	glScalef(1,3,1);
	glutSolidCube(.5);
	glPopMatrix();
	//glPopMatrix();
	drawFeet(iankle);
}

void animateAnimal(){
	animal1x += 0.05 * (float)reverseAnim1;
	animal2y += 0.05 * (float)reverseAnim2;
	animal3x += 0.05 * (float)reverseAnim3;
	animal3y += 0.05 * (float)reverseAnim3;
	//if(whichAnimal == 0) eyex += 0.05 * (float)reverseAnim1;
	//if(whichAnimal == 1) eyez += 0.05 * (float)reverseAnim2;
	//if(whichAnimal == 2){
	//	eyex += 0.05 * (float)reverseAnim3;
	//	eyez += 0.05 * (float)reverseAnim3;
	//}


	//printf("%f",(float)reverseAnim);
	if(animal1x > 50 || animal1x < 30) reverseAnim1 = reverseAnim1*(-1);
	if(animal2y > 50 || animal2y < 30) reverseAnim2 = reverseAnim2*(-1);
	if(animal3x > 22.5 || animal3x < 11) reverseAnim3 = reverseAnim3*(-1);

	fronthip += neg2;
	frontknee += neg2;
	backhip -= neg2;
	backknee -= neg2;
	shoulder -= (.25)*neg2;
	if (fronthip > 50 || fronthip < -50) neg2 = (-1)*neg2;
	//printf("%f %f\n",fronthip,neg2);
	//backhip > 40 || backhip < -40) neg = -neg
}

void drawAnimal(int animalNumber){
	//glutSolidTeapot(2.0);
	glPushMatrix();
	if(animalNumber == 1){
		glRotatef(0,0,1,0);
		if(reverseAnim1 < 0) glRotatef(180,0,1,0);
	}
	if(animalNumber == 2){ 
		glRotatef(-90,0,1,0);
		if(reverseAnim2 < 0 ) glRotatef(180,0,1,0);
	}
	if(animalNumber == 3){
		glRotatef(-45,0,1,0);
		if(reverseAnim3 < 0) glRotatef(180,0,1,0);
	}
	drawBody();
	drawHead();
	drawThighs(-1.5,-1,-.5,backhip);
	drawCalves(backknee,backankle);

	drawThighs(-1.5,-1,.5,backhip);
	drawCalves(backknee,backankle);

	drawThighs(1.5,-1,-.5,fronthip);
	drawCalves(frontknee,frontankle);

	drawThighs(1.5,-1,.5,fronthip);
	drawCalves(frontknee,frontankle);

	glPopMatrix();

}

//terrain code
int getHeight(int x, int y){
	pixel_t pixel = PIXEL(terrain, x, y);
	int h = RED(pixel);
	//printf("%d\n",h);
	return h;
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

float* cross(float *v1, float *v2)
{
	float product[3];

	//Cross product formula 
	product[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	product[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	product[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);

	return product;
}

float getDot(float *a, float *b) {
	float vecta[3] = {normalize(a)[0], normalize(a)[1],normalize(a)[2]};
	float vectb[3] = {normalize(b)[0], normalize(b)[1],normalize(b)[2]};
	return vecta[0]*vectb[0] + vecta[1]*vectb[1] + vecta[2]*vectb[2];
}

float getAngle(float *a, float *b) {
	return acosf(getDot(a, b));
}

void placeAnimal(float x, float y, int animalNumber){

	float animalUp[3] = {0,1,0};
	float* terrainNormal = vnormals[(int)(x*5)][(int)(y*5)];
	//printf("terrainNormal: %f %f %f\n", terrainNormal[0],terrainNormal[1],terrainNormal[2]);
	float axis[3] = {cross(animalUp,terrainNormal)[0],cross(animalUp,terrainNormal)[1],cross(animalUp,terrainNormal)[2]};
	//printf("axis: %f %f %f\n", axis[0],axis[1],axis[2]);
	float angle = getAngle(animalUp,terrainNormal);
	angle = angle * (180/PI);
	//printf("angle: %f\n", angle);

	glPushMatrix();
	glTranslatef(x,((float)getHeight(x*5,y*5)/15.0)+1,y);
	if(axis[0] != 0 || axis[1] != 0 || axis[2] != 0) glRotatef(angle,axis[0],axis[1],axis[2]);
	glBindTexture(GL_TEXTURE_2D, 0);
	glScalef(.25,.25,.25);
	drawAnimal(animalNumber);
	glBindTexture(GL_TEXTURE_2D, texture);
	glPopMatrix();
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

			glTexCoord2f((float)x/(float)size,(float)y/(float)size); //texture
			glVertex3f(verts[x][y][0],verts[x][y][1],verts[x][y][2]);//make p1

			if(vnormals_bool) 
				glNormal3f(vnormals[xpr][y][0],vnormals[xpr][y][1],vnormals[xpr][y][2]); //set vertex normal p2

			glTexCoord2f((float)xpr/(float)size,(float)y/(float)size); //texture
			glVertex3f(verts[xpr][y][0],verts[xpr][y][1],verts[xpr][y][2]);//make p2

			if(vnormals_bool) 
				glNormal3f(vnormals[xpr][ypr][0],vnormals[xpr][ypr][1],vnormals[xpr][ypr][2]); //set vertex normal p3

			glTexCoord2f((float)xpr/(float)size,(float)ypr/(float)size); //texture
			glVertex3f(verts[xpr][ypr][0],verts[xpr][ypr][1],verts[xpr][ypr][2]);//make p3


			//triangle 2
			if(vnormals_bool) 
				glNormal3f(vnormals[x][y][0],vnormals[x][y][1],vnormals[x][y][2]); //set vertex normal p1
			else 
				glNormal3f(fnormals[x][y][1][0], fnormals[x][y][1][1], fnormals[x][y][1][2]); //else set surface normal for t2
			glTexCoord2f((float)x/(float)size,(float)y/(float)size); //texture
			glVertex3f(verts[x][y][0],verts[x][y][1],verts[x][y][2]);//make p1

			if(vnormals_bool) 
				glNormal3f(vnormals[xpr][ypr][0],vnormals[xpr][ypr][1],vnormals[xpr][ypr][2]); //set vertex normal p3
			glTexCoord2f((float)xpr/(float)size,(float)ypr/(float)size); //texture
			glVertex3f(verts[xpr][ypr][0],verts[xpr][ypr][1],verts[xpr][ypr][2]);//make p3

			if(vnormals_bool) 
				glNormal3f(vnormals[x][ypr][0],vnormals[x][ypr][1],vnormals[x][ypr][2]); //set vertex normal p4
			glTexCoord2f((float)x/(float)size,(float)ypr/(float)size); //texture
			glVertex3f(verts[x][ypr][0],verts[x][ypr][1],verts[x][ypr][2]); //make p4

		}
	}	
	glDisable(GL_TEXTURE_2D);
}

//GL code
void cb_display() {
	///*
	static clock_t previous_clock = 0;
	static clock_t ms_since_last = 0;
	clock_t now = clock()/(CLOCKS_PER_SEC / 1000);
	ms_since_last = now - previous_clock;
	if(ms_since_last < 20) return;
	previous_clock = now;
	float animalx = 0, animaly = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	if(whichAnimal == 0){
		eyex = animal1x;
		eyez = animal1y - 10;
		animalx = animal1x;
		animaly = animal1y;
	}
	if(whichAnimal == 1){
		eyex = animal2x - 10;
		eyez = animal2y;
		animalx = animal2x;
		animaly = animal2y;
	}
	if(whichAnimal == 2){
		eyex = animal3x - 10;
		eyez = animal3y + 10;
		animalx = animal3x;
		animaly = animal3y;
	}

	gluLookAt(eyex,eyey,eyez,animalx,0,animaly,0,1,0);

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
	placeAnimal(animal1x,animal1y,1);
	placeAnimal(animal2x,animal2y,2);
	placeAnimal(animal3x,animal3y,3);
	if(anim) animateAnimal();
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
	case 'x':
		animal1x=45;
		animal1y=18;

		animal2x=8;
		animal2y=32;

		animal3x=12;
		animal3y=26;
		break;
	case 'z':
		if(!anim) resetAnimJoints();
		anim = !anim;
		break;
	case '1':
		whichAnimal = 0;
		break;
	case '2':
		whichAnimal = 1;
		break;
	case '3':
		whichAnimal = 2;
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

	printf("\n\n1,2,3: Look at different animals\nZ: Enable/Disable animation of animals\nX: Reset animals\nQ,W,E,A,S,D: Light controls\nG,T: Camera Height Control\nV: Enable/disable vertex normals\nB: Enable/disable display lists\n");

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


