#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <string>
#include <fstream>
#include "Triangle.h"
#include "vector3.h"
#include <vector>
#include <iostream>
#include <cfloat>
#include <cmath>


#define PI 3.14159265
#define T_SIZE 256
//derlerken g++ hw2.cpp -lGLU -lGL -lglut
using namespace std;

vector<Triangle> TriVec;
GLuint shirt, text;
bool fst = true, snd = false;
float minX   = FLT_MAX, minY  = FLT_MAX, minZ  = FLT_MAX, maxX = FLT_MIN, maxY = FLT_MIN, maxZ = FLT_MIN;
Vector3 center;
float start;
float posx = 0, posy = 0, posz, ptx = 0, pty = 0, ptz = 0, ux = 0, uy = 1, uz = 0;
float deg = 0;
float upDeg = 0, downDeg = 0, rightDeg = 0, leftDeg = 0;
Vector3 rght;

bool isH = true;

bool movingUp = false; // Whether or not we are moving up or down 
bool movingRight = false;
float moveY = 0, moveX = 0;

bool growing = false;
float scale = 1;

GLuint ttop, tbottom, tleft, tright, tfront, tback;
float skyboxsize;


/*er
 * 
 * rightDeg += 0.0002;
	Vector3 up(ux, uy, uz);
	Vector3 viewing(ptx-posx, pty-posy, ptz-posz);
	Vector3 newUp = Rot(viewing, up, -rightDeg);
	ux = newUp._x;
	uy = newUp._y;
	uz = newUp._z;
	Vector3 nR = Rot(viewing, rght, -rightDeg);
	rght = nR;*/

//float yLocation = 0.0f; // Keep track of our position on the y axis.
float yRotationAngle = 0.0f; // The angle of rotation for our object


bool* keyStates = new bool[256];// Create an array of boolean values of length 256 (0-255)  
bool* keySpecialStates = new bool[256];



 
//u nun etrafında tor u deg kadar dondurmek
Vector3 Rot(Vector3 u, Vector3 tor, float deg){
	Vector3 ret;
	float cs = cos(deg * PI / 180);
	float sn = sin(deg * PI / 180);
	u.normalize();
	ret._x = tor._x*(cs + (u._x * u._x)*(1 - cs)) + tor._y*((u._x * u._y)*(1 - cs) - (u._z*sn)) + tor._z*((u._x * u._z*(1-cs)) + u._y * sn);
	ret._y = tor._x*((u._x * u._y)*(1 - cs)+(u._z*sn)) + tor._y*(cs + (u._y * u._y)*(1 - cs)) + tor._z*((u._y * u._z*(1-cs)) - u._x * sn);
	ret._z = tor._x*((u._x * u._z*(1-cs)) - u._y * sn) + tor._y*((u._y * u._z*(1-cs)) + u._x * sn) + tor._z*(cs + (u._z * u._z)*(1 - cs));
	return ret;
}

void harmonicVertical(){
	float sy = maxY - minY;
	float mov = sy * 0.75 / 100;
	if(movingUp)
	{
		if(moveY > sy * 0.75)
		{
			movingUp = false;
			moveY -= mov;
		}
		moveY += mov;
	}
	else
	{
		if (moveY < -sy * 0.75)
		{
			movingUp = true;
			moveY += mov;
		}
		moveY -= mov;		
	}	
}
void harmonicHorizontal(){
	float sx = maxX - minX;
	float mov = sx * 0.75 / 100;
	if(movingRight)
	{
		if(moveX > sx * 0.75)
		{
			movingRight = false;
			moveX -= mov;
		}
		moveX += mov;
	}
	else
	{
		if (moveX < -sx * 0.75)
		{
			movingRight = true;
			moveX += mov;
		}
		moveX -= mov;		
	}	
}
void objRot(){
	yRotationAngle += 0.3;
	
	if (yRotationAngle >= 360)
		yRotationAngle -= 360;	
}
void objScale(){
	if(growing)
	{
		if (scale > 1.5)
		{
			growing = false;
			scale -= 0.001;
		}
		scale += 0.001;
	}
	else
	{
		if (scale < 0.6)
		{
			growing = true;
			scale += 0.001;
		}
		scale -= 0.001;
	}
	
}
void camRot(){
	deg += 0.2;
	if(deg > 360)
		deg -= 360;
		
	posx = abs(start) * sin(deg * PI / 180);
	posz = abs(start) * cos(deg * PI / 180);
}

void camMoveFwd(){	
	Vector3 pos(posx, posy, posz);
	Vector3 pt(ptx, pty, ptz);
	Vector3 viewing = pt - pos;
	
	posx += viewing._x * 0.001;
	posy += viewing._y * 0.001;
	posz += viewing._z * 0.001;
	
	ptx = posx + viewing._x;
	pty = posy + viewing._y;
	ptz = posz + viewing._z;
}
void camMoveBck(){	
	Vector3 pos(posx, posy, posz);
	Vector3 pt(ptx, pty, ptz);
	Vector3 viewing = pt - pos;
	
	posx -= viewing._x * 0.001;
	posy -= viewing._y * 0.001;
	posz -= viewing._z * 0.001;
	
	ptx = posx + viewing._x;
	pty = posy + viewing._y;
	ptz = posz + viewing._z;
}
void camLeft(){
	Vector3 pos(posx, posy, posz);
	Vector3 pt(ptx, pty, ptz);
	Vector3 viewing = pt - pos;
	
	posx -= rght._x * 0.001;
	posy -= rght._y * 0.001;
	posz -= rght._z * 0.001;
	
	ptx = posx + viewing._x;
	pty = posy + viewing._y;
	ptz = posz + viewing._z;
	
}
void camRight(){
	Vector3 pos(posx, posy, posz);
	Vector3 pt(ptx, pty, ptz);
	Vector3 viewing = pt - pos;
	
	posx += rght._x * 0.001;
	posy += rght._y * 0.001;
	posz += rght._z * 0.001;
	
	ptx = posx + viewing._x;
	pty = posy + viewing._y;
	ptz = posz + viewing._z;
}
void camUp(){	
//	if(upDeg < 70)
		upDeg = 0.05;
	Vector3 up(ux, uy, uz);
	Vector3 newUp = Rot(rght, up, upDeg);
	ux = newUp._x;
	uy = newUp._y;
	uz = newUp._z;
	//Vector3 viewing = newUp.cross(rght);
	Vector3 viewing(ptx-posx, pty-posy, ptz-posz);
	Vector3 nV = Rot(rght, viewing, upDeg);
	//ptx = viewing._x + posx;
	//pty = viewing._y + posy;
	//ptz = viewing._z + posz;
	ptx = nV._x + posx;
	pty = nV._y + posy;
	ptz = nV._z + posz;
}
void camDown(){
	downDeg = -0.05;
	Vector3 up(ux, uy, uz);
	Vector3 viewing(ptx-posx, pty-posy, ptz-posz);
	Vector3 newUp = Rot(rght, up, downDeg);	
	ux = newUp._x;
	uy = newUp._y;
	uz = newUp._z;
	Vector3 nV = Rot(rght, viewing, downDeg);
	ptx = nV._x + posx;
	pty = nV._y + posy;
	ptz = nV._z + posz;
}
void camR(){
	rightDeg = 0.05;	
	Vector3 up(ux, uy, uz);
	Vector3 viewing(ptx-posx, pty-posy, ptz-posz);
	Vector3 nR = Rot(up, rght, -rightDeg);
	rght = nR;
	Vector3 nV = Rot(up, viewing, -rightDeg);
	
	ptx = nV._x + posx;
	pty = nV._y + posy;
	ptz = nV._z + posz;	
}
void camL(){
	leftDeg = 0.05;
	Vector3 up(ux, uy, uz);
	Vector3 viewing(ptx-posx, pty-posy, ptz-posz);
	Vector3 nR = Rot(up, rght, leftDeg);
	rght = nR;
	Vector3 nV = Rot(up, viewing, leftDeg);
	
	ptx = nV._x + posx;
	pty = nV._y + posy;
	ptz = nV._z + posz;	
	
}

void eCam(){
	Vector3 up(ux, uy, uz);
	Vector3 viewing(ptx-posx, pty-posy, ptz-posz);
	Vector3 newUp = Rot(viewing, up, -0.1);
	ux = newUp._x;
	uy = newUp._y;
	uz = newUp._z;
	Vector3 nR = Rot(viewing, rght, -0.1);
	rght = nR;
}
void rCam(){
	Vector3 up(ux, uy, uz);
	Vector3 viewing(ptx-posx, pty-posy, ptz-posz);
	Vector3 newUp = Rot(viewing, up, 0.1);
	ux = newUp._x;
	uy = newUp._y;
	uz = newUp._z;
	Vector3 nR = Rot(viewing, rght, 0.1);
	rght = nR;
	
}

void setInit(){
	movingUp = false; // Whether or not we are moving up or down 
	movingRight = false;
	moveY = 0;
	moveX = 0;
	yRotationAngle = 0;
	deg = 0;
	scale = 1;
	posx = 0;
	posy = 0;
	posz = start;
	ptx = 0;
	pty = 0;
	ptz = 0;
	ux = 0;
	uy = 1;
	uz = 0;
	Vector3 pos(posx, posy, posz);
	Vector3 pt(ptx, pty, ptz);
	Vector3 up(ux, uy, uz);
	rght = (pt - pos).cross(up);
}
void printNormal(){
	for (int i = 0; i < TriVec.size(); i++)
	{
		cout << TriVec[i].normal._x << " " << TriVec[i].normal._y << " " << TriVec[i].normal._z << endl;
	}
	cout << TriVec.size() << endl;
}
void setCamera () {        
       GLfloat position[] = {0, 0.1, 0.4, 0.0};
       glLightfv(GL_LIGHT0, GL_POSITION , position);
       glMatrixMode(GL_MODELVIEW);
       glLoadIdentity();
       gluLookAt(posx, posy, posz, ptx, pty, ptz, ux, uy, uz);
}
void Parse(string filepath){
	fstream read;
	read.open(filepath.c_str());
	Vector3 temp1, temp2, temp3;
	while (read >> temp1._x >> temp1._y >> temp1._z)
	{		
		read >> temp2._x >> temp2._y >> temp2._z;
		read >> temp3._x >> temp3._y >> temp3._z;
		Triangle triTemp(temp1, temp2, temp3);
		TriVec.push_back(triTemp);
		if (temp1._x < minX)
			minX = temp1._x;
		if (temp1._y < minY)
			minY = temp1._y;
		if (temp1._z < minZ)
			minZ = temp1._z;
			
		if (temp2._x < minX)
			minX = temp2._x;
		if (temp2._y < minY)
			minY = temp2._y;
		if (temp2._z < minZ)
			minZ = temp2._z;
			
		if (temp3._x < minX)
			minX = temp3._x;
		if (temp3._y < minY)
			minY = temp3._y;
		if (temp3._z < minZ)
			minZ = temp3._z;
			//max
		if(temp1._x > maxX)
			maxX = temp1._x;
		if(temp1._y > maxY)
			maxY = temp1._y;
		if(temp1._z > maxZ)
			maxZ = temp1._z;
			
		if(temp2._x > maxX)
			maxX = temp2._x;
		if(temp2._y > maxY)
			maxY = temp2._y;
		if(temp2._z > maxZ)
			maxZ = temp2._z;
			
		if(temp3._x > maxX)
			maxX = temp3._x;
		if(temp3._y > maxY)
			maxY = temp3._y;
		if(temp3._z > maxZ)
			maxZ = temp3._z;		
	}
	center._x = minX + ((maxX - minX) / 2);
	center._y = minY + ((maxY - minY) / 2);
	center._z = minZ + ((maxZ - minZ) / 2);
	
	float sx, sy, sz;
	sx = maxX - minX;
	sy = maxY - minY;
	sz = maxZ - minZ;
	
	if (sx >= sy) {
		if (sx >= sz) {
			start = maxZ + 2.5 * sx;
			skyboxsize = 7 * sx;
		}
		
		else {
			start = maxZ + 2.5 * sz;
			skyboxsize = 7 * sz;
		}
	}
	
	if (sy >= sx){
		if (sy >= sz) {
			start = maxZ + 2.5 * sy;
			skyboxsize = 7 * sy;
		}
		
		else {
			start = maxZ + 2.5 * sz;
			skyboxsize = 7 * sz;
		}
	}

	posz = start;
	
	Vector3 pos(posx, posy, posz);
	Vector3 pt(ptx, pty, ptz);
	Vector3 up(ux, uy, uz);
	rght = (pt - pos).cross(up);
}
void TriDrawer(){
	
		glPushMatrix();
		glColor3f(0.7f, 0.1f, 0.5f);
		glBegin(GL_TRIANGLES);	
	for (int i = 0; i < TriVec.size(); i++)
	{		
		Triangle t = TriVec[i];	
		
		//Vector3 norma(0,0,0), normb(0,0,0), normc(0,0,0);
		Vector3 norma = TriVec[i].normal, normb = TriVec[i].normal, normc = TriVec[i].normal;
		for (int j = 0; j < TriVec.size(); j++)
		{
			if(t.Vertex_a == TriVec[j].Vertex_a)
				norma += TriVec[j].normal;
			else if(t.Vertex_a == TriVec[j].Vertex_b)
				norma += TriVec[j].normal;
			else if(t.Vertex_a == TriVec[j].Vertex_c)
				norma += TriVec[j].normal;
				
			if(t.Vertex_b == TriVec[j].Vertex_a)
				normb += TriVec[j].normal;
			else if(t.Vertex_b == TriVec[j].Vertex_b)
				normb += TriVec[j].normal;
			else if(t.Vertex_b == TriVec[j].Vertex_c)
				normb += TriVec[j].normal;
				
			if(t.Vertex_c == TriVec[j].Vertex_a)
				normc += TriVec[j].normal;
			else if(t.Vertex_c == TriVec[j].Vertex_b)
				normc += TriVec[j].normal;
			else if(t.Vertex_c == TriVec[j].Vertex_c)
				normc += TriVec[j].normal;
		}
					
		
		glNormal3f(norma._x, norma._y, norma._z);
		glVertex3f(t.Vertex_a._x, t.Vertex_a._y, t.Vertex_a._z);	
			
		glNormal3f(normb._x, normb._y, normb._z);
		glVertex3f(t.Vertex_b._x, t.Vertex_b._y, t.Vertex_b._z);	
		
		glNormal3f(normc._x, normc._y, normc._z);
		glVertex3f(t.Vertex_c._x, t.Vertex_c._y, t.Vertex_c._z);
		
	}
		glEnd();
		glPopMatrix();
}
void keyOperations(){
	if(keyStates['q'])
		exit(0);
	if(keyStates[1])
		setInit();
	if (fst) //1st camera mode
	{
		if(keyStates['z']) //Space is pressed. Put object and camera in initial position.
			harmonicVertical();
		if(keyStates['x'])
			harmonicHorizontal();
		if(keyStates['c'])
			objRot();
		if(keyStates['v'])
			objScale();
		if(keyStates[32])
			setInit();
		if(isH)
			camRot();			
	}
	if (snd) //2nd camera mode
	{
		if(keyStates[32])
			setInit();
		if(keyStates['w'])
			camMoveFwd();
		if(keyStates['s'])
			camMoveBck();
		if(keyStates['a'])
			camLeft();
		if(keyStates['d'])
			camRight();
		if(keySpecialStates[GLUT_KEY_LEFT])
			camL();
		if(keySpecialStates[GLUT_KEY_RIGHT])
			camR();
		if(keySpecialStates[GLUT_KEY_UP])
			camUp();
		if(keySpecialStates[GLUT_KEY_DOWN])
			camDown();
		if(keyStates['e'])
			eCam();
		if(keyStates['r'])
			rCam();
			
	}
	
		
	
	//if (keyStates['a'])	{  	}
}
void renderPrimitive(){
	glColor3f(0.5f, 0.0f, 0.5f);
	glBegin(GL_TRIANGLES);
	glVertex3f(-1.0f, -1.0f, 0.0f); // The bottom left corner
	glColor3f(0.5f, 0.0f, 0.5f);
	glVertex3f(-1.0f, 1.0f, 0.0f);// The top left corner
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glEnd();
}

void skybox () {
	glDisable(GL_LIGHTING);
	glColor3f(1,1,1);
	glBindTexture( GL_TEXTURE_2D, tfront ); 
    glBegin (GL_QUADS);
    glTexCoord2d(0,1); glVertex3f(-skyboxsize,-skyboxsize, -skyboxsize); 
    glTexCoord2d(1,1); glVertex3f(+skyboxsize,-skyboxsize, -skyboxsize); 
    glTexCoord2d(1,0); glVertex3f(+skyboxsize,+skyboxsize, -skyboxsize);
    glTexCoord2d(0,0); glVertex3f(-skyboxsize,+skyboxsize, -skyboxsize);
    glEnd();
    
    glBindTexture( GL_TEXTURE_2D, tback ); 
    glBegin (GL_QUADS);
    glTexCoord2d(1,1); glVertex3f(-skyboxsize,-skyboxsize, skyboxsize); 
    glTexCoord2d(0,1); glVertex3f(+skyboxsize,-skyboxsize, skyboxsize);
    glTexCoord2d(0,0); glVertex3f(+skyboxsize,+skyboxsize, skyboxsize);
    glTexCoord2d(1,0); glVertex3f(-skyboxsize,+skyboxsize, skyboxsize);
    glEnd();
    
	glBindTexture( GL_TEXTURE_2D, tleft );
    glBegin (GL_QUADS);
    glTexCoord2d(0,1); glVertex3f(-skyboxsize,-skyboxsize, skyboxsize); 
    glTexCoord2d(1,1); glVertex3f(-skyboxsize,-skyboxsize, -skyboxsize);
    glTexCoord2d(1,0); glVertex3f(-skyboxsize,+skyboxsize, -skyboxsize);
    glTexCoord2d(0,0); glVertex3f(-skyboxsize,+skyboxsize, skyboxsize);
    glEnd();
    
    glBindTexture( GL_TEXTURE_2D, tright );
    glBegin (GL_QUADS);
    glTexCoord2d(1,1); glVertex3f(skyboxsize,-skyboxsize, skyboxsize); 
    glTexCoord2d(0,1); glVertex3f(skyboxsize,-skyboxsize, -skyboxsize); 
    glTexCoord2d(0,0); glVertex3f(skyboxsize,+skyboxsize, -skyboxsize);
    glTexCoord2d(1,0); glVertex3f(skyboxsize,+skyboxsize, skyboxsize);
    glEnd();
    
    glBindTexture( GL_TEXTURE_2D, ttop );
    glBegin (GL_QUADS);
    glTexCoord2d(0,0); glVertex3f(-skyboxsize,+skyboxsize, +skyboxsize); 
    glTexCoord2d(1,0); glVertex3f(+skyboxsize,+skyboxsize, +skyboxsize); 
    glTexCoord2d(1,1); glVertex3f(+skyboxsize,+skyboxsize, -skyboxsize);
    glTexCoord2d(0,1); glVertex3f(-skyboxsize,+skyboxsize, -skyboxsize);
    glEnd();
    
    glBindTexture( GL_TEXTURE_2D, tbottom );
    glBegin (GL_QUADS);
    glTexCoord2d(0,1); glVertex3f(-skyboxsize,-skyboxsize, +skyboxsize); 
    glTexCoord2d(1,1); glVertex3f(+skyboxsize,-skyboxsize, +skyboxsize); 
    glTexCoord2d(1,0); glVertex3f(+skyboxsize,-skyboxsize, -skyboxsize);
    glTexCoord2d(0,0); glVertex3f(-skyboxsize,-skyboxsize, -skyboxsize);
    glEnd();
    
    glEnable(GL_LIGHTING);	
}

void display(void){
	keyOperations();
	glClearColor(0.f, 0.f, 0.0f, 1.f);//clear the background window to black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	//glTranslatef(0.0f, 0.0f, -2.0f);
	setCamera();
	
	//glScalef(2, 0.5, 1);
	
	//glTranslatef(0.0f, yLocation, 0.0f); // Translate our object along the y axis
	//glRotatef(yRotationAngle, 0.0f, 1.0f, 0.0f);  // Rotate our object around the y axis 	
	
	//glCallList(text);
	glEnable( GL_TEXTURE_2D );
	skybox();
	glDisable(GL_TEXTURE_2D);
	
	glScalef(scale, scale, scale);
	glRotatef(yRotationAngle, 0, 0, 1);
	glTranslatef(moveX, moveY, 0);
	glCallList(shirt);
	
	
	//renderPrimitive();
	//glutSolidCube(2.0f);
	//glColor3f(1, 0, 0);
	
	/*
	if(movingUp)
		yLocation -= 0.05f; // Move up along our yLocation
	else
		yLocation += 0.05f;
		
	if(yLocation < -3.0f)//If we have gone up too far 
		movingUp = false;
	else if(yLocation > 3.0f) //If we have gone down too far 
		movingUp = true;
	yRotationAngle += 0.005f;
	if(yRotationAngle > 360.0f) // If we have rotated beyond 360 degrees (a full rotation) 
		yRotationAngle -= 360.0f;  // Subtract 360 degrees off of our rotation
		*/
	
	glutSwapBuffers();	
}
void reshape(int width, int height){
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);	 // Switch to the projection matrix so that we can manipulate how our scene is viewed  
	glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)  
	gluPerspective(60, (GLfloat)width / (GLfloat)height, 0.00001, 1000.0);       // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes 
	glMatrixMode(GL_MODELVIEW);
	
}
void keyPressed(unsigned char key, int x, int y){
	if(key == '1')
	{
		fst = true;
		snd = false;
	}
	else if(key == '2')
	{
		fst = false;
		snd = true;
		Vector3 pos(posx, posy, posz);
		Vector3 pt(ptx, pty, ptz);
		Vector3 up(ux, uy, uz);
		rght = (pt - pos).cross(up);
	}
	else if (key == 'h')
	 {
		isH = !isH;
	 }
	 
	else
		keyStates[key] = true;
	//keyStates[key] = true; // Set the state of the current key to pressed
	//if (key == 'a') { // If they ‘a’ key was pressed  }
// Perform action associated with the ‘a’ key  
}


void keyUp(unsigned char key, int x, int y){
	
	
	keyStates[key] = false;
	
	//keyStates[key] = false;// Set the state of the current key to not pressed
	//if (key == 'a') { // If they ‘a’ key was released  }
// Perform action associated with the ‘a’ key  

}
void keySpecial(int key, int x, int y){
	keySpecialStates[key] = true;	
}
void keySpecialUp(int key, int x, int y){
	keySpecialStates[key] = false;
}

GLuint LoadTexture( const char * filename, int width, int height ) {
    GLuint texture;
    unsigned char * data;
    FILE * file;

    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;
    data = (unsigned char *)malloc( width * height * 3 );
    fread( data, width * height * 3, 1, file );
    fclose( file );

    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    free(data);
    return texture;
}

void FreeTexture( GLuint texture ) {
  glDeleteTextures( 1, &texture ); 
}

int main(int argc, char **argv){
	
	Parse(argv[1]);
	
	
	glutInit (&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);

	
	glutInitWindowSize(900, 700);
	glutInitWindowPosition (100, 100);
	glutCreateWindow("CENG477_HW2");
	
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
		
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat position[] = {1000.0, 1000.0 , 1000.0, 0.0};
	GLfloat blackColor[] = {0.1 , 0.1 , 0.1 , 1.0};
	GLfloat whiteColor[] = {1.0 , 1.0 , 1.0 , 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION , position);
	glLightfv(GL_LIGHT0 , GL_AMBIENT , blackColor);
	glLightfv(GL_LIGHT0 , GL_DIFFUSE , whiteColor);
	glLightfv(GL_LIGHT0 , GL_SPECULAR , whiteColor);

	shirt = glGenLists(1);
	glNewList(shirt, GL_COMPILE);
	glTranslatef(- center._x, -center._y, -center._z);
	TriDrawer();
	glEndList();
	
	ttop = LoadTexture("top.raw", T_SIZE, T_SIZE );
	tbottom = LoadTexture("bottom.raw", T_SIZE, T_SIZE );
	tleft = LoadTexture("left.raw", T_SIZE, T_SIZE );
	tright = LoadTexture("right.raw", T_SIZE, T_SIZE );
	tfront = LoadTexture("front.raw", T_SIZE, T_SIZE );
	tback = LoadTexture("back.raw", T_SIZE, T_SIZE );
	
	text = glGenLists(1);
	glNewList(text, GL_COMPILE);
	skybox();
	glEndList();
	
	glutDisplayFunc (display);
	glutIdleFunc (display);
	glutReshapeFunc (reshape);
	
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(keySpecial);
	glutSpecialUpFunc(keySpecialUp);

	glutMainLoop ();
	
	FreeTexture(ttop);
	FreeTexture(tbottom);
	FreeTexture(tleft);
	FreeTexture(tright);
	FreeTexture(tfront);
	FreeTexture(tback);
	return 0;
}
