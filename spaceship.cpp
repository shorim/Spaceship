#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <math.h>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <Windows.h>
#include <mmsystem.h>

using namespace std;

struct Object {
	int textureId, type, bezierId;
	double bezierT;
	double bezierOffset, realOffset;
	double initialX, initialY, initialZ;
	double bezierTInc;
	double currX, currY, currZ, x_angle, y_angle, z_angle;
	bool rotateLeft;
	Object(int textureId1, int type1, int bezierId1,
		double bezierT1, double bezierOffset1, double realOffset1,
		double initialX1, double initialY1, double initialZ1, double bezierTInc1, double currX1, double currY1, double currZ1,
		double x_angle1, double y_angle1, double z_angle1, bool rotateLeft1) :
		textureId(textureId1), type(type1), bezierId(bezierId1), bezierT(bezierT1), bezierOffset(bezierOffset1),
		realOffset(realOffset1), initialX(initialX1), initialY(initialY1), initialZ(initialZ1), bezierTInc(bezierTInc1),
		currX(currX1), currY(currY1), currZ(currZ1), x_angle(x_angle1), y_angle(y_angle1), z_angle(z_angle1), rotateLeft(rotateLeft1) {}
};

int health = 3, score = 0;
double earthZ = 0;
int METEROID = 0, COIN = 1, POWER_UP = 2, SPACESHIP = 3, SATELLITE = 4, ASTRONAUT = 5;

char title[] = "Spaceship";
vector<Object> objects;
double bezierCurves[4][4][2];
double animateObjectRet[2];
double curveTInc = 0.005, generalTInc = 0.02;

// 3D Projection Options
int WIDTH = 1280;
int HEIGHT = 720;
GLdouble fovy = 70;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 150;

// Model Variables
Model_3DS model_spaceship, model_coin, model_powerUp, model_satellite, model_astronaut, model_spaceship2;

// Meteroids Textures
GLuint textures[5];
GLuint earthTexture;

// Space Textures  
GLuint space1_Tex;
GLuint space2_Tex;
GLuint space3_Tex;
int currFrontScene = 2;

double space1_Z = 0;
double space2_Z = -150;
double space3_Z = -300;

double space_translate_x = 0;
double space_translate_y = 0;

double space_rotate_z = 0;
double space_rotate_x = 0;


double earth_angle = 0;
double sun_angle = 0;

int gameState = 0; // 0--> user is playing, 1--> lost, 2--> won

bool firstPerson = false;

double sun_x = 90;
double sun_y = 10;
double sun_z = 0;

void print(float x, float y, float z, float r, float g, float b, char *string)
{
	glPushMatrix();
	glColor3f(r, g, b);
	int len, i;

	//set the position of the text in the window using the x and y coordinates
	glRasterPos3f(x, y, z);

	//get the length of the string to display
	len = (int)strlen(string);
	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
	glPopMatrix();
	glColor3f(1, 1, 1);

}

void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// light source inside the sun
	glEnable(GL_LIGHT0);
	GLfloat sun_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat sun_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat sun_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat sunLight_position[] = { sun_x, sun_y, sun_z, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, sun_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, sun_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, sunLight_position);

	// params for light source from spaceship
	GLfloat spaceship_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat spacehip_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat spaceship_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat spaceship_light_direction[] = { 0.0, 0.0, -1.0 };

	// light source on right wing of spaceship
	glEnable(GL_LIGHT1);
	GLfloat spaceship_light1_position[] = { 55 + space_translate_x, 48 + space_translate_y, 98, 1.0f };
	glLightfv(GL_LIGHT1, GL_AMBIENT, spaceship_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, spacehip_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spaceship_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, spaceship_light1_position);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 5.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 128.0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spaceship_light_direction);

	// light source on left wing of spaceship
	glEnable(GL_LIGHT2);
	GLfloat spaceship_light2_position[] = { 45 + space_translate_x, 48 + space_translate_y, 98, 1.0f };
	glLightfv(GL_LIGHT2, GL_AMBIENT, spaceship_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, spacehip_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, spaceship_specular);
	glLightfv(GL_LIGHT2, GL_POSITION, spaceship_light2_position);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 5.0);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 128.0);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spaceship_light_direction);

}

void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (firstPerson){
		gluLookAt(50 + space_translate_x, 50 + space_translate_y, 95,
			50 + space_translate_x, 50 + space_translate_y, 5, 0.0, 1.0, 0.0);
	}
	else {
		//gluLookAt(80, 50, 100, 50, 50, 100, 0.0, 1.0, 0.0); //side view
		gluLookAt(50, 60, 140, 50, 50, 5, 0.0, 1.0, 0.0);
		//gluLookAt(50, 90, 105, 50, 50, 100, 0.0, 1.0, 0.0);
	}
	InitLightSource();
	InitMaterial();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
}

double* bezier(double t, double* p0, double* p1, double* p2, double* p3)
{
	double res[2];
	res[0] = pow((1 - t), 3)*p0[0] + 3 * t*pow((1 - t), 2)*p1[0] + 3 * pow(t, 2)*(1 - t)*p2[0] + pow(t, 3)*p3[0];
	res[1] = pow((1 - t), 3)*p0[1] + 3 * t*pow((1 - t), 2)*p1[1] + 3 * pow(t, 2)*(1 - t)*p2[1] + pow(t, 3)*p3[1];
	return res;
}

void animateObject(int objIdx, double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3)
{
	objects[objIdx].bezierT += objects[objIdx].bezierTInc;
	if (objects[objIdx].bezierT >= 1)
	{
		objects[objIdx].bezierT = 0;
		objects[objIdx].realOffset += objects[objIdx].bezierOffset;
	}
	// bezier points
	double objectP0[2];
	double objectP1[2];
	double objectP2[2];
	double objectP3[2];

	objectP0[0] = x0;
	objectP0[1] = y0;

	objectP1[0] = x1;
	objectP1[1] = y1;

	objectP2[0] = x2;
	objectP2[1] = y2;

	objectP3[0] = x3;
	objectP3[1] = y3;

	double *p = bezier(objects[objIdx].bezierT, objectP0, objectP1, objectP2, objectP3);
	animateObjectRet[0] = p[0];
	animateObjectRet[1] = p[1];
	animateObjectRet[0] += objects[objIdx].realOffset;

}

void drawFrontBoundary() {

	// front space disk
	glPushMatrix();
	glNormal3f(0, 0, 1);
	glTranslated(50, 50, 0);
	GLUquadric *frontSpace = gluNewQuadric();
	if (currFrontScene == 1)
		glBindTexture(GL_TEXTURE_2D, space1_Tex);
	else if (currFrontScene == 2)
		glBindTexture(GL_TEXTURE_2D, space2_Tex);
	else if (currFrontScene == 3)
		glBindTexture(GL_TEXTURE_2D, space3_Tex);

	gluQuadricTexture(frontSpace, true);
	gluQuadricNormals(frontSpace, GL_SMOOTH);
	gluDisk(frontSpace, 0, 150, 25, 25);
	glPopMatrix();

}

void drawBoundary(GLuint spaceText, double space_z) {

	// main space cylinder
	glPushMatrix();
	glTranslated(50, 50, space_z);
	GLUquadricObj * space = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, spaceText);
	gluQuadricTexture(space, true);
	gluQuadricNormals(space, GL_SMOOTH);
	gluQuadricDrawStyle(space, GL_TRIANGLE_STRIP);
	gluCylinder(space, 75, 75, 150, 100, 100);
	glPopMatrix();


}

void drawSphere(double x, double y, double z, int textureId, double r, double angle_x, double angle_y) {


	if (textureId == 4) // sun
		glDisable(GL_LIGHTING);

	glPushMatrix();
	glTranslated(x, y, z);
	glRotated(angle_y, 0, 1, 0);
	glRotated(angle_x, 1, 0, 0);
	GLUquadricObj * sphere = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, textures[textureId]);
	gluQuadricTexture(sphere, true);
	gluQuadricNormals(sphere, GL_SMOOTH);
	gluSphere(sphere, r, 100, 100);
	glPopMatrix();

	if (textureId == 4) // sun
		glEnable(GL_LIGHTING);



}

void drawSpaceShip() {
	// Draw Spaceship
	glPushMatrix();
	glNormal3f(0, 1, 0);
	glTranslated(50 + space_translate_x, 50 + space_translate_y, 100);
	glRotated(space_rotate_z, 0, 0, 1);
	glRotated(space_rotate_x, 1, 0, 0);
	glScaled(0.04, 0.04, 0.04);
	model_spaceship.Draw();
	glPopMatrix();

}

void drawBoundaries() {
	drawFrontBoundary();
	drawBoundary(space1_Tex, space1_Z);
	drawBoundary(space2_Tex, space2_Z);
	drawBoundary(space3_Tex, space3_Z);
}

void drawCoin(double x, double y, double z, double angle_y) {
	glPushMatrix();
	glTranslated(x, y, z);
	glRotated(angle_y, 0, 1, 0);
	glRotated(-45, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	glScaled(0.5, 0.5, 0.5);
	model_coin.Draw();
	glPopMatrix();
}

void drawPowerUp(double x, double y, double z, double angle_y) {
	glPushMatrix();
	glTranslated(x, y, z);
	glRotated(angle_y, 0, 1, 0);
	glRotated(-45, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	glScaled(0.5, 0.5, 0.5);
	model_powerUp.Draw();
	glPopMatrix();

}

void drawSpaceship2(double x, double y, double z) {

	glPushMatrix();
	glTranslated(x, y, z);
	glRotated(90, 0, 1, 0);
	model_spaceship2.Draw();
	glPopMatrix();

}

void drawSatellite(double x, double y, double z, double y_angle) {

	glPushMatrix();
	glTranslated(x, y, z);
	glRotated(y_angle, 0, 1, 0);
	glScaled(0.05, 0.05, 0.05);
	model_satellite.Draw();
	glPopMatrix();
}

void drawAstronaut(double x, double y, double z, double z_angle) {

	glPushMatrix();
	glTranslated(x, y, z);
	glRotated(z_angle, 0, 0, 1);
	glScaled(0.01, 0.01, 0.01);
	model_astronaut.Draw();
	glPopMatrix();
}

void drawObjects() {
	for (int i = 0; i < (int)objects.size(); i++) {

		double z;
		double y;
		double x;

		if (objects[i].type == SPACESHIP) {
			objects[i].currZ += 0.5;
			z = objects[i].currZ;
			drawSpaceship2(objects[i].currX, objects[i].currY, objects[i].currZ);
		}
		else if (objects[i].type == SATELLITE) {
			objects[i].currZ += 0.5;
			z = objects[i].currZ;
			objects[i].y_angle += 0.5;
			drawSatellite(objects[i].currX, objects[i].currY, objects[i].currZ, objects[i].y_angle);
		}
		else if (objects[i].type == ASTRONAUT) {
			objects[i].currZ += 0.2;
			z = objects[i].currZ;

			if (objects[i].rotateLeft) {
				objects[i].z_angle += 0.5;
				if (objects[i].z_angle >= 45)
					objects[i].rotateLeft = false;

			}
			else {
				objects[i].z_angle -= 0.5;
				if (objects[i].z_angle <= -45)
					objects[i].rotateLeft = true;

			}

			drawAstronaut(objects[i].currX, objects[i].currY, objects[i].currZ, objects[i].z_angle);

		}
		else {

			animateObject(i,
				bezierCurves[objects[i].bezierId][0][0], bezierCurves[objects[i].bezierId][0][1],
				bezierCurves[objects[i].bezierId][1][0], bezierCurves[objects[i].bezierId][1][1],
				bezierCurves[objects[i].bezierId][2][0], bezierCurves[objects[i].bezierId][2][1],
				bezierCurves[objects[i].bezierId][3][0], bezierCurves[objects[i].bezierId][3][1]);

			z = objects[i].currZ = animateObjectRet[0] + objects[i].initialZ;

			if (objects[i].type == COIN) {
				x = objects[i].currX = animateObjectRet[1] + objects[i].initialX;
				y = objects[i].currY = objects[i].initialY;
			}
			else {
				y = objects[i].currY = animateObjectRet[1] + objects[i].initialY;
				x = objects[i].currX = objects[i].initialX;
			}

		}

		if (objects[i].type == METEROID) {
			objects[i].x_angle++;
			drawSphere(x, y, z, objects[i].textureId, 1, objects[i].x_angle, 0);
		}
		else if (objects[i].type == COIN) {
			objects[i].y_angle++;
			drawCoin(x, y, z, objects[i].y_angle);
		}
		else if (objects[i].type == POWER_UP) {
			objects[i].y_angle++;
			drawPowerUp(x, y, z, objects[i].y_angle++);
		}


		if (z >= 150)
			objects.erase(objects.begin() + i);

	}
}

void printHealth(float x, float y, float z, float r, float g, float b) {
	std::ostringstream ss;
	ss << health;

	string h = "Health : " + ss.str();
	char hChar[1024];
	strncpy(hChar, h.c_str(), sizeof(hChar));
	hChar[sizeof(hChar)-1] = 0;
	print(x, y, z, r, g, b, hChar);

}

void printScore(float x, float y, float z, float r, float g, float b) {
	std::ostringstream ss;
	ss << score;

	string s = "Score : " + ss.str();
	char sChar[1024];
	strncpy(sChar, s.c_str(), sizeof(sChar));
	sChar[sizeof(sChar)-1] = 0;
	print(x, y, z, r, g, b, sChar);
}

void gameOver() {

	glClearColor(1, 0, 0, 0);
	print(44, 70, 100, 1, 1, 1, "Game Over!");
	printScore(45, 65, 100, 1, 1, 1);
	print(40, 60, 100, 1, 1, 1, "Press Enter to Play Again");
}
void gameWin(){

	glClearColor(0, 1, 0, 0);
	print(40, 70, 100, 1, 1, 1, "Congratulations You won !");
	printScore(46, 65, 100, 1, 1, 1);
	print(40, 60, 100, 1, 1, 1, "Press Enter to Play Again");
}

void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	myInit();
	if (gameState == 1)
		gameOver();
	else if (gameState == 2){
		gameWin();
	}
	else {

		drawBoundaries();
		drawSpaceShip();
		drawObjects();
		// draw Earth
		drawSphere(50, 10, earthZ, 3, 10, -90, earth_angle);
		// draw Sun
		drawSphere(sun_x, sun_y, sun_z, 4, 20, 90, sun_angle);

		glBindTexture(GL_TEXTURE_2D, 0); // prevents the color of the text from being changed
		if (!firstPerson){
			printHealth(5, 80, 100, 1, 0, 0);
			printScore(5, 76, 100, 0, 1, 0);
		}
		else{
			printHealth(44.2 + space_translate_x, 53 + space_translate_y, 90, 1, 0, 0);
			printScore(44.2 + space_translate_x, 52.5 + space_translate_y, 90, 0, 1, 0);
		}
	}

	glutSwapBuffers();
}

void myKeyboard(unsigned char button, int x, int y)
{
	if (button == 'f'){
		firstPerson = !firstPerson;
	}

	if (button == 13 && gameState != 0) {
		mciSendString(TEXT("play space repeat"), 0, 0, 0);
		earthZ = 0;
		health = 3;
		score = 0;
		objects.clear();
		gameState = 0;
		currFrontScene = 2;

		space1_Z = 0;
		space2_Z = -150;
		space3_Z = -300;

		space_translate_x = 0;
		space_translate_y = 0;
	}

	glutPostRedisplay();
}

void LoadAssets()
{
	// Loading Model files
	model_spaceship.Load("Models/spaceship/Fighter.3ds");
	model_coin.Load("Models/coin/Coin 2.3ds");
	model_powerUp.Load("Models/powerUp/Coin 2.3ds");
	model_satellite.Load("Models/satellite/satellite.3ds");
	model_astronaut.Load("Models/astronaut/astronaut.3ds");
	model_spaceship2.Load("Models/spaceship2/spaceship2.3ds");

	// Loading Textures
	loadBMP(&space1_Tex, "Textures/space1.bmp", true);
	loadBMP(&space2_Tex, "Textures/space2.bmp", true);
	loadBMP(&space3_Tex, "Textures/space3.bmp", true);
	loadBMP(&textures[0], "Textures/meteroid1.bmp", true);
	loadBMP(&textures[1], "Textures/meteroid2.bmp", true);
	loadBMP(&textures[2], "Textures/meteroid3.bmp", true);
	loadBMP(&textures[3], "Textures/earth.bmp", true);
	loadBMP(&textures[4], "Textures/sun.bmp", true);

}

void Anim() {

	if (gameState == 0) { // user is playing

		earth_angle += 0.5;
		sun_angle += 0.1;

		space1_Z += 0.05;
		space2_Z += 0.05;
		space3_Z += 0.05;

		earthZ += 0.01;

		if (space1_Z >= 300) {
			space1_Z = -150;
		}
		if (space2_Z >= 300) {
			space2_Z = -150;
		}
		if (space3_Z >= 300) {
			space3_Z = -150;
		}

		if (space1_Z >= 150) {
			currFrontScene = 3;
		}
		if (space2_Z >= 150) {
			currFrontScene = 1;
		}
		if (space3_Z >= 150) {
			currFrontScene = 2;
		}

		for (int i = 0; i < (int)objects.size(); i++) {

			double x = objects[i].currX;
			double y = objects[i].currY;
			double z = objects[i].currZ;

			if (
				(abs(x - (50 + space_translate_x)) <= 5 && abs(y - (50 + space_translate_y)) <= 3 && abs(z - 100) <= 10)
				||
				(abs(x - (50 + space_translate_x)) <= 15 && abs(y - (50 + space_translate_y)) <= 2 && abs(z - 103) <= 5)
				) {
				if (objects[i].type == COIN) {
					objects.erase(objects.begin() + i);
					PlaySound(TEXT("coin.wav"), NULL, SND_ASYNC);
					score++;
				}
				else if (objects[i].type == METEROID){
					objects.erase(objects.begin() + i);
					PlaySound(TEXT("meteroid.wav"), NULL, SND_ASYNC);
					health--;
					if (health == 0) {
						mciSendString(TEXT("stop space"), 0, 0, 0);
						PlaySound(TEXT("lose.wav"), NULL, SND_ASYNC);
						gameState = 1;
						firstPerson = false;
					}
				}
				else if (objects[i].type == POWER_UP){
					objects.erase(objects.begin() + i);
					PlaySound(TEXT("health.wav"), NULL, SND_ASYNC);
					health = min(3, health + 1);
				}

			}

		}

		if (earthZ >= 90){
			mciSendString(TEXT("stop space"), 0, 0, 0);
			PlaySound(TEXT("win.wav"), NULL, SND_ASYNC);
			gameState = 2;
			firstPerson = false;
		}
	}

	glutPostRedisplay();
}

void spe(int k, int x, int y)// keyboard special key function takes 3 parameters
// int k: is the special key pressed such as the keyboard arrows the f1,2,3 and so on
{
	if (k == GLUT_KEY_UP)
	{
		space_translate_y = min(space_translate_y + 1, 25);
		if (!firstPerson)
			space_rotate_x = 10;
	}
	if (k == GLUT_KEY_DOWN)
	{
		space_translate_y = max(space_translate_y - 1, -13);
		if (!firstPerson)
			space_rotate_x = -10;
	}
	if (k == GLUT_KEY_RIGHT)
	{
		space_translate_x = min(space_translate_x + 1, 32);
		space_rotate_z = -5;
	}
	if (k == GLUT_KEY_LEFT)
	{
		space_translate_x = max(space_translate_x - 1, -32);
		space_rotate_z = 5;
	}

	glutPostRedisplay();//redisplay to update the screen with the changes
}

void speUp(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
	{
		space_rotate_x = 0;
	}

	if (key == GLUT_KEY_DOWN) {

		space_rotate_x = 0;
	}

	if (key == GLUT_KEY_RIGHT)
	{
		space_rotate_z = 0;
	}

	if (key == GLUT_KEY_LEFT)
	{
		space_rotate_z = 0;
	}

	glutPostRedisplay();
}

void setBezierPoints() {

	// Repeated Parabola
	bezierCurves[0][0][0] = 0;
	bezierCurves[0][0][1] = 0;

	bezierCurves[0][1][0] = 0;
	bezierCurves[0][1][1] = 10;

	bezierCurves[0][2][0] = 10;
	bezierCurves[0][2][1] = 10;

	bezierCurves[0][3][0] = 10;
	bezierCurves[0][3][1] = 0;


	// Single Parabola
	bezierCurves[1][0][0] = 0;
	bezierCurves[1][0][1] = 0;

	bezierCurves[1][1][0] = 140 / 2;
	bezierCurves[1][1][1] = 20;

	bezierCurves[1][2][0] = 140 / 2;
	bezierCurves[1][2][1] = 20;

	bezierCurves[1][3][0] = 140;
	bezierCurves[1][3][1] = 0;


	// Sin
	bezierCurves[2][0][0] = 0;
	bezierCurves[2][0][1] = 0;

	bezierCurves[2][1][0] = 10;
	bezierCurves[2][1][1] = -10;

	bezierCurves[2][2][0] = 10;
	bezierCurves[2][2][1] = 10;

	bezierCurves[2][3][0] = 20;
	bezierCurves[2][3][1] = 0;

	// Spiral
	bezierCurves[3][0][0] = 0;
	bezierCurves[3][0][1] = 0;

	bezierCurves[3][1][0] = -10;
	bezierCurves[3][1][1] = 5;

	bezierCurves[3][2][0] = 20;
	bezierCurves[3][2][1] = 5;

	bezierCurves[3][3][0] = 10;
	bezierCurves[3][3][1] = 0;

}


void generateObject() {
	int r = rand() % 10;
	int type = r <= 3 ? METEROID : r <= 7 ? COIN : POWER_UP;
	double intitalX = (rand() % 60) + 20;
	double initialY = (rand() % 25) + 45;
	double initialZ = -5;
	int bezierId = type == COIN ? 2 : type == POWER_UP ? 0 : (rand() % 3) + 1;
	double tInc = bezierId == 1 ? curveTInc : generalTInc;

	Object o = Object(rand() % 3, type, bezierId,
		0, bezierCurves[bezierId][3][0] - bezierCurves[bezierId][0][0], 0,
		intitalX, initialY, initialZ, tInc, intitalX, initialY, initialZ, 0, 0, 0, false);

	objects.push_back(o);

	// forcing an extra meteroid
	type = METEROID;
	intitalX = (rand() % 60) + 20;
	initialY = (rand() % 25) + 45;
	initialZ = -5;
	bezierId = (rand() % 3) + 1;
	tInc = bezierId == 1 ? curveTInc : generalTInc;

	Object meteroid = Object(rand() % 3, type, bezierId,
		0, bezierCurves[bezierId][3][0] - bezierCurves[bezierId][0][0], 0,
		intitalX, initialY, initialZ, tInc, intitalX, initialY, initialZ, 0, 0, 0, false);

	objects.push_back(meteroid);

	// forcing an extra coin
	type = COIN;
	intitalX = (rand() % 60) + 20;
	initialY = (rand() % 25) + 45;
	initialZ = -5;
	bezierId = 2;
	tInc = generalTInc;

	Object coin = Object(rand() % 3, type, bezierId,
		0, bezierCurves[bezierId][3][0] - bezierCurves[bezierId][0][0], 0,
		intitalX, initialY, initialZ, tInc, intitalX, initialY, initialZ, 0, 0, 0, false);

	objects.push_back(coin);


	// handling scene models
	intitalX = (rand() % 60) + 20;
	initialY;
	initialZ = -5;

	if (currFrontScene == 2) {
		initialY = (rand() % 8) + 22;
		Object spaceship = Object(0, SPACESHIP, 0, 0, 0, 0, intitalX, initialY, initialZ, 0, intitalX, initialY, initialZ, 0, 0, 0, false);
		objects.push_back(spaceship);
	}
	else if (currFrontScene == 3) {
		initialY = (rand() % 7) + 20;
		Object spaceship = Object(0, SATELLITE, 0, 0, 0, 0, intitalX, initialY, initialZ, 0, intitalX, initialY, initialZ, 0, 0, 0, false);
		objects.push_back(spaceship);
	}
	else if (currFrontScene == 1) {
		initialY = (rand() % 8) + 20;
		intitalX = (rand() % 30) + 20;  // to avoid making the astronaut too close to sun
		Object spaceship = Object(0, ASTRONAUT, 0, 0, 0, 0, intitalX, initialY, initialZ, 0, intitalX, initialY, initialZ, 0, 0, 0, true);
		objects.push_back(spaceship);
	}


}

void time(int val)
{
	if (gameState == 0) //user is playing
		generateObject();

	glutPostRedisplay();
	glutTimerFunc(3000, time, 0);
}

void main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(50, 50);
	glutCreateWindow(title);

	if (gameState == 0) { //user is playing

		mciSendString("open space.mp3 alias space", 0, 0, 0);
		mciSendString(TEXT("play space repeat"), 0, 0, 0);
	}

	InitLightSource();
	InitMaterial();

	glutDisplayFunc(myDisplay);
	glutIdleFunc(Anim);
	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(spe);
	glutSpecialUpFunc(speUp);
	glutTimerFunc(0, time, 0);

	setBezierPoints();
	LoadAssets();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}