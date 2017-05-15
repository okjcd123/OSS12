/*
---------------------------------------------------------------------------
 Copied from Dan Cristia, Rotaru
 https://github.com/RotaruDan/SolarSystem
---------------------------------------------------------------------------
 OpenSource Software Project (https://github.com/okjcd123/OSS12)
 Digital Contents 김준혁 문희호 이상협 정지혜

 Date of preparation (작성일):						2017년 5월 12일
 Date of final modification (최종 수정일):			2017년 5월 15일
*/
//-------------------------------------------------------------------------------------------
// Headers
//-------------------------------------------------------------------------------------------
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <GL/freeglut.h>

#include <cmath>
#include "Mesh.h"
#include "Sun.h"
#include "Axis.h" 
#include "Sphere.h" 
#include "Earth.h" 
#include "Disk.h" 
#include "Satelite.h" 
#include "Group.h"
#include <stdio.h>

//-------------------------------------------------------------------------------------------
// Structures and global variables
//-------------------------------------------------------------------------------------------
// Viewport, Represents the size of the rectangle that displays the screen.
struct viewPort{GLsizei w; GLsizei h;} Vp = {700, 700};

// Camera parameters, decide the camera position, view direction and camera angle.
// upX, upY, upZ is up vector, determines which direction is "up".
struct viewCamera{GLdouble eyeX, eyeY, eyeZ;
        GLdouble lookX, lookY, lookZ;
        GLdouble upX, upY, upZ;} 
			initial = {350, 350, 350, 0, 0, 0, 0, 1, 0}, 
			front = {0, 0, 550, 0,0,0, 0,1,0},
			topView = {0, 450, 0, 0,0,0, 1,0,1}, 
			lateral = {550, 0, 0, 0,0,0, 0,1,0};

// Viewing frustum parameters, used for projection.
// zNear - minimum distance to draw.
// zFar  - maximum distance to draw.
// others- decide screen size.
struct viewVolume{GLdouble xRight, xLeft;
        GLdouble yTop, yBot;
        GLdouble zNear, zFar;} Pj = {350, -350, 350,-350, 1, 1000};

GLdouble scale = 1;
bool tiling, ortho = true;
viewCamera * currentView = &initial;
GLfloat light_position1[] = {0, 0, 0, 1.0f};

// Declared objects to display on screen in structure form.
Mesh plane("f-16.obj", 20);
Group root;
Sun sun;
Group earthSystem;
Earth* earthRef;
Satelite satelite;
Sphere moon(8, 20, 20);

//---------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------
static const unsigned int UP = 0;
static const unsigned int DOWN = 1;
static const unsigned int LEFT = 2;
static const unsigned int RIGHT = 3;
static const unsigned int _IN = 4;
static const unsigned int _OUT = 5;
static const unsigned int _ROTATE = 6;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------
void updateProjection();
void updateCamera();
void rotate(double &vx, double &vy, double &vz, double ax, double ay, double az, double angle);
void moveCamera(unsigned int direction);
void initScene();
void initGL();
void resize(int wW, int wH);
void display();
void keyPres(unsigned char key, int mX, int mY);
void keySp(int key, int mX, int mY);
//---------------------------------------------------------------------------

// programa principal
int main(int argc, char* argv[]){ 
	// Initialization 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(Vp.w, Vp.h); 
	glutInitWindowPosition(100,100);
	glutInit(&argc,argv);
	// Window construction
	int win=glutCreateWindow("Solar System OSS12");

	//창에 관련된 함수  
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyPres);
	glutSpecialFunc(keySp);
	// OpenGL basic setting
	initGL();

	// X Y Z 축 생성하기
	Axis axis;
	root.addChildren(&axis);
	
	//Solar System Group 생성
	Group solarSystem;
	root.addChildren(&solarSystem);
	solarSystem.setAngleVector(0, 1, 0);			//y축을 기준으로 회전

	sun.setAngleVector(0, 1, 0);
	sun.setColor(1.0, 1.0, 0.0, 1.0);
	solarSystem.addChildren(&sun);

	//지구 궤도 생성
	Disk earthOrbit;
	earthOrbit.setColor(1.0, 1.0, 0.0, 1.0);
	earthOrbit.setAngle(90);					
	earthOrbit.setAngleVector(1, 0, 0);
	solarSystem.addChildren(&earthOrbit);

	// Earth system
	solarSystem.addChildren(&earthSystem);
	earthSystem.setX(130);
	earthSystem.setAngleVector(0, 1, 0);

	//지구 생성
	Earth earth;
	earthRef = &earth;
	earth.setColor(1, 1, 1, 1);
	earth.setAngleVector(0, 0, 1);

	// 지구 자전을 위한 earthContainer Group 생성
	Group earthContainer;
	earthContainer.addChildren(&earth);
	earthContainer.setAngle(-90);
	earthContainer.setAngleVector(1, 0, 0);
	earthSystem.addChildren(&earthContainer);

	//달궤도 생성
	Disk moonOrbit(52, 54, 60, 1);
	moonOrbit.setColor(1, 1, 1, 1);
	moonOrbit.setAngle(90);					
	moonOrbit.setAngleVector(1, 0, 0);
	earthSystem.addChildren(&moonOrbit);

	//달 생성
	moon.setColor(1, 1, 1, 1);
	moon.setAngleVector(0, 1, 0);
	moon.setX(53);
	earthSystem.addChildren(&moon);

	//인공위성 궤도 생성
	Disk sateliteOrbit(52, 54, 60, 1);
	sateliteOrbit.setColor(0, 1, 0, 1);
	earthSystem.addChildren(&sateliteOrbit);

	//인공위성 생성
	satelite.setColor(0, 1, 0, 1);
	satelite.setAngleVector(0, 0, 1);
	satelite.setY(53);
	earthSystem.addChildren(&satelite);

	//비행기 궤도 생성
	Disk planeOrbit(52, 54, 60, 1);
	planeOrbit.setColor(0, 1, 1, 1);
	planeOrbit.setAngle(90);
	planeOrbit.setAngleVector(0, 1, 0);
	earthSystem.addChildren(&planeOrbit);

	//비행기 생성
	plane.setColor(0, 1, 1, 1);
	plane.setAngleVector(1, 0, 0);
	plane.setY(53);
	earthSystem.addChildren(&plane);
  
	initScene();				//카메라 시점에 관한 부분 초기화
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION) ;
	glutMainLoop();  //glutMainLoopEvent(); 내장함수에 따라서 진행
	glutDestroyWindow(win);
	return 0;
}
//---------------------------------------------------------------------------

void initGL(){

	//glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);		// z buffer enable
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);   
	glEnable(GL_TEXTURE_2D); 

	GLfloat mat_diffuse[] = {1.0f, 0.0f, 0.0f, 1.0f};
	GLfloat mat_specular[] = {0.0f, 0.0f, 1.0f, 1.0f};
	GLfloat mat_ambient[] = {0.0f, 1.0f, 0.0f, 1.0f};
	GLfloat mat_shininess[] = {100.0f};

	GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat light_position[] = {0, 0, 0, 1.0f};
	
	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);


	// 빛 퐁셰이딩 초기화
	glEnable(GL_LIGHT1);	
	GLfloat light_specular1[] = {0.4f, 0.4f, 0.4f, 1.0f};
	GLfloat light_diffuse1[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	

	// 물질 퐁셰이딩 초기화
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

	glShadeModel(GL_SMOOTH);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// 기본적인 카메라 세팅
	updateCamera();
	updateProjection();
}

void initScene(){

	sun.setAngle(0);
	earthSystem.setAngle(0);
	satelite.setAngle(0);
	moon.setAngle(0);
	tiling = false;				//태양계를 타일 모양으로 여러개 출력하는 변수
	currentView = &initial;

	initial.eyeX = 350;
	initial.eyeY = 350;
	initial.eyeZ = 350;
	initial.lookX = 0;
	initial.lookY = 0;
	initial.lookZ = 0;
	initial.upX = 0;
	initial.upY = 1;
	initial.upZ = 0;

	front.eyeX = 0;
	front.eyeY = 0;
	front.eyeZ = 550;
	front.lookX = 0;
	front.lookY = 0;
	front.lookZ = 0;
	front.upX = 0;
	front.upY = 1;
	front.upZ = 0;

	topView.eyeX = 0;
	topView.eyeY = 450;
	topView.eyeZ = 0;
	topView.lookX = 0;
	topView.lookY = 0;
	topView.lookZ = 0;
	topView.upX = 1;
	topView.upY = 0;
	topView.upZ = 1;

	lateral.eyeX = 550;
	lateral.eyeY = 0;
	lateral.eyeZ = 0;
	lateral.lookX = 0;
	lateral.lookY = 0;
	lateral.lookZ = 0;
	lateral.upX = 0;
	lateral.upY = 1;
	lateral.upZ = 0;

	updateCamera();
	ortho = true;
	updateProjection();
}

void updateCamera(){
	glMatrixMode(GL_MODELVIEW);   // VIEW
	glLoadIdentity();
	gluLookAt((*currentView).eyeX, (*currentView).eyeY, (*currentView).eyeZ,
		(*currentView).lookX,	(*currentView).lookY,	(*currentView).lookZ, 
		(*currentView).upX, (*currentView).upY, (*currentView).upZ);

}
//---------------------------------------------------------------------------

void resize(int wW, int wH){
	// Viewport set up     
	Vp.w = wW; Vp.h = wH; 

	// Frustum set up
	updateProjection();

	glutPostRedisplay();
}

void updateProjection(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	Pj.xRight = Vp.w/ 2.0 / scale; Pj.xLeft = -Pj.xRight; 
	Pj.yTop = Vp.h/2.0 / scale;  Pj.yBot = -Pj.yTop; 
	if(ortho){
		glOrtho(Pj.xLeft,Pj.xRight, Pj.yBot,Pj.yTop, Pj.zNear, Pj.zFar);
	} else {
		glFrustum(Pj.xLeft,Pj.xRight, Pj.yBot,Pj.yTop, Pj.zNear*300, Pj.zFar);
		//gluPerspective(80, Vp.w / Vp.h, Pj.zNear, Pj.zFar );
	}
}
//---------------------------------------------------------------------------

void display(void) {
	
	light_position1[0] = static_cast<GLfloat>((*currentView).eyeX);
	light_position1[1] = static_cast<GLfloat>((*currentView).eyeY);
	light_position1[2] = static_cast<GLfloat>((*currentView).eyeZ);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
	glMatrixMode(GL_MODELVIEW);
	
	//타일 모양이 아닐경우 일반적인 화면 출력
	if(!tiling){
		glViewport(0,0, Vp.w, Vp.h); 
		root.render();
	} else {			//타일 모양 출력을 위해 4개의 행렬을 선언하여 viewport 세팅
		int columns = 4;
		int rows = 4;
		GLsizei vpW = Vp.w / columns;
		GLsizei vpH = Vp.h / rows;
		for(int i = 0; i < rows; ++i){
			for(int j = 0; j < columns; ++j){
				glViewport(i * vpW, j * vpH, vpW, vpH); 
				root.render();
			}
		}
	}

	glutSwapBuffers(); // Hay dos buffers que se van intercambiando para ir pinando en ellos
}
//-------------------------------------------------------------------------------------------
// moveCamera 
// description : Moves the camera relative to the camera's viewpoint
// parameter   : direction - direction to move, include rotation
// return      : none
//-------------------------------------------------------------------------------------------
void moveCamera(unsigned int direction){
	GLdouble eX, eY, eZ;
	GLdouble lX, lY, lZ;
	GLdouble uX, uY, uZ;
	GLdouble vX, vY, vZ;
	GLdouble wX, wY, wZ;
	GLdouble length;
	switch (direction)
	{
	case UP:
		// e is "camera position"
		eX = currentView->eyeX;
		eY = currentView->eyeY;
		eZ = currentView->eyeZ;

		// l is "camera look at"
		lX = currentView->lookX;
		lY = currentView->lookY;
		lZ = currentView->lookZ;

		// u is the vector between the two points
		uX = eX - lX;
		uY = eY - lY;
		uZ = eZ - lZ;		

		// v is up vector.
		vX = -currentView->upX;
		vY = -currentView->upY;
		vZ = -currentView->upZ;

		// w = u x v
		wX = uY * vZ - uZ * vY;
		wY = uZ * vX - uX * vZ;
		wZ = uX * vY - uY * vX;

		// Compute the perpendicular of the current obtained vector
		vX = wX;
		vY = wY;
		vZ = wZ;
		
		// w = u x v. The newly obtained vector points upwards.
		wX = uY * vZ - uZ * vY;
		wY = uZ * vX - uX * vZ;
		wZ = uX * vY - uY * vX;

		// Normalize w
		length = sqrt((wX * wX)+(wY * wY)+(wZ * wZ));
		wX /= length;
		wY /= length;
		wZ /= length;

		// Add it to "camera position"
		currentView->eyeX += wX;
		currentView->eyeY += wY;
		currentView->eyeZ += wZ;
		
		// The ""camera position"" and the ""camera look at"" move in parallel
		currentView->lookX += wX;
		currentView->lookY += wY;
		currentView->lookZ += wZ;
		break;
	case DOWN:
		// e is "camera position"
		eX = currentView->eyeX;
		eY = currentView->eyeY;
		eZ = currentView->eyeZ;

		// l is "camera look at"
		lX = currentView->lookX;
		lY = currentView->lookY;
		lZ = currentView->lookZ;

		// u is the vector between the two points
		uX = eX - lX;
		uY = eY - lY;
		uZ = eZ - lZ;		

		// v is up vector.
		vX = currentView->upX;
		vY = currentView->upY;
		vZ = currentView->upZ;

		// w = u x v
		wX = uY * vZ - uZ * vY;
		wY = uZ * vX - uX * vZ;
		wZ = uX * vY - uY * vX;

		// Compute the perpendicular of the current obtained vector
		vX = wX;
		vY = wY;
		vZ = wZ;
		
		// w = u x v. The newly obtained vector points downwards
		wX = uY * vZ - uZ * vY;
		wY = uZ * vX - uX * vZ;
		wZ = uX * vY - uY * vX;

		// Normalize w
		length = sqrt((wX * wX)+(wY * wY)+(wZ * wZ));
		wX /= length;
		wY /= length;
		wZ /= length;

		// Add it to "camera position"
		currentView->eyeX += wX;
		currentView->eyeY += wY;
		currentView->eyeZ += wZ;
		
		// The "camera position" and the "camera look at" move in parallel
		currentView->lookX += wX;
		currentView->lookY += wY;
		currentView->lookZ += wZ;
		break;
	case LEFT:
		// e is "camera position"
		eX = currentView->eyeX;
		eY = currentView->eyeY;
		eZ = currentView->eyeZ;

		// l is "camera look at"
		lX = currentView->lookX;
		lY = currentView->lookY;
		lZ = currentView->lookZ;

		// u is the vector between two points
		uX = eX - lX;
		uY = eY - lY;
		uZ = eZ - lZ;		

		// v is up vector.
		vX = currentView->upX;
		vY = currentView->upY;
		vZ = currentView->upZ;

		// w = u x v
		wX = uY * vZ - uZ * vY;
		wY = uZ * vX - uX * vZ;
		wZ = uX * vY - uY * vX;

		// Normalize w
		length = sqrt((wX * wX)+(wY * wY)+(wZ * wZ));
		wX /= length;
		wY /= length;
		wZ /= length;

		// Add it to "camera position"
		currentView->eyeX += wX;
		currentView->eyeY += wY;
		currentView->eyeZ += wZ;
		
		// The "camera position" and the "camera look at" move in parallel
		currentView->lookX += wX;
		currentView->lookY += wY;
		currentView->lookZ += wZ;
		break;
	case RIGHT:
		// e is "camera position"
		eX = currentView->eyeX;
		eY = currentView->eyeY;
		eZ = currentView->eyeZ;

		// l is "camera look at"
		lX = currentView->lookX;
		lY = currentView->lookY;
		lZ = currentView->lookZ;

		// u is the vector between two points
		uX = eX - lX;
		uY = eY - lY;
		uZ = eZ - lZ;		

		// v is up vector.
		vX = -currentView->upX;
		vY = -currentView->upY;
		vZ = -currentView->upZ;

		// w = u x v
		wX = uY * vZ - uZ * vY;
		wY = uZ * vX - uX * vZ;
		wZ = uX * vY - uY * vX;

		// Normalize w
		length = sqrt((wX * wX)+(wY * wY)+(wZ * wZ));
		wX /= length;
		wY /= length;
		wZ /= length;

		// Add it to "camera position"
		currentView->eyeX += wX;
		currentView->eyeY += wY;
		currentView->eyeZ += wZ;
		
		// The "camera position" and the "camera look at" move in parallel
		currentView->lookX += wX;
		currentView->lookY += wY;
		currentView->lookZ += wZ;
		break;
	case _IN:
		// e is "camera position"
		eX = currentView->eyeX;
		eY = currentView->eyeY;
		eZ = currentView->eyeZ;

		// l is "camera look at"
		lX = currentView->lookX;
		lY = currentView->lookY;
		lZ = currentView->lookZ;

		// u is the vector between two points
		uX = eX - lX;
		uY = eY - lY;
		uZ = eZ - lZ;	

		// Normalize u
		length = sqrt((uX * uX)+(uY * uY)+(uZ * uZ)) * .5;
		uX /= length;
		uY /= length;
		uZ /= length;

		// Add it to "camera position"
		currentView->eyeX += uX;
		currentView->eyeY += uY;
		currentView->eyeZ += uZ;
		break;
	case _OUT:
		// e is "camera position"
		eX = currentView->eyeX;
		eY = currentView->eyeY;
		eZ = currentView->eyeZ;

		// l is "camera look at"
		lX = currentView->lookX;
		lY = currentView->lookY;
		lZ = currentView->lookZ;

		// u is the vector between two points
		uX = eX - lX;
		uY = eY - lY;
		uZ = eZ - lZ;	

		// Normalize u
		length = sqrt((uX * uX)+(uY * uY)+(uZ * uZ)) * .5;
		uX /= length;
		uY /= length;
		uZ /= length;

		// Subtract it from "camera position"
		currentView->eyeX -= uX;
		currentView->eyeY -= uY;
		currentView->eyeZ -= uZ;
		break;
	case _ROTATE: {
		// e is "camera position"
		eX = currentView->eyeX;
		eY = currentView->eyeY;
		eZ = currentView->eyeZ;

		// l is "camera look at"
		lX = currentView->lookX;
		lY = currentView->lookY;
		lZ = currentView->lookZ;

		// n is the vector between two points
		GLdouble nX = eX - lX;
		GLdouble nY = eY - lY;
		GLdouble nZ = eZ - lZ;

		// up vector
		GLdouble upX = currentView->upX;
		GLdouble upY = currentView->upY;
		GLdouble upZ = currentView->upZ;
		
		// u = up x n
		GLdouble uX = upY * nZ - upZ * nY;
		GLdouble uY = upZ * nX - upX * nZ;
		GLdouble uZ = upX * nY - upY * nX;

		// v = n x u
		vX = nY * uZ - nZ * uY;
		vY = nZ * uX - nX * uZ;
		vZ = nX * uY - nY * uX;

		// Normalize v
		length = sqrt((vX * vX)+(vY * vY)+(vZ * vZ));
		vX /= length;
		vY /= length;
		vZ /= length;

		// Rotate "camera look at" based on v vector.
		rotate(currentView->lookX, currentView->lookY, currentView->lookZ, vX, vY, vZ, 0.1);
		break;
	} default:
		break;
	}
}
//-------------------------------------------------------------------------------------------
// rotate
// description : Rotate v vector based on a point.
// parameter   : vector v - vector to rotate
// parameter   : point a  - rotation axis
// parameter   : angle    - rotation angle
// return      : none
//-------------------------------------------------------------------------------------------
void rotate(double &vx, double &vy, double &vz, double ax, double ay, double az, double angle) {
	double ca = cos(angle);
	double sa = sin(angle);
	double crossx = -vy*az + vz*ay;
	double crossy = -vz*ax + vx*az;
	double crossz = -vx*ay + vy*ax;
	double dot = ax*vx + ay*vy + az*vz;
	double rx = vx*ca + crossx*sa + dot*ax*(1-ca);
	double ry = vy*ca + crossy*sa + dot*ay*(1-ca);
	double rz = vz*ca + crossz*sa + dot*az*(1-ca);
	vx = rx; 
	vy = ry; 
	vz = rz;
}

//-------------------------------------------------------------------------------------------
// keyPres
// description : Use the keys to determine the action.
// parameter   : key - the key input
// parameter   : mX  - no use
// parameter   : mY  - no use
// return      : none
//-------------------------------------------------------------------------------------------
void keyPres(unsigned char key, int mX, int mY){
	bool need_redisplay = true;
	if(key == 27) {  /* Escape key */  //continue_in_main_loop = false; (**)
		glutLeaveMainLoop (); //Freeglut's sentence for stopping glut's main loop 
	} else if(key == '+') { 
		if(scale < 10.) {
			scale +=0.25; resize(Vp.w, Vp.h);
		} 
	} else if(key == '-') { 
		if(scale > 0.3) {
			scale -=0.25; resize(Vp.w, Vp.h);
		} 
	} else if(key == 's') { 
		sun.setAngle(sun.getAngle() + 5); 
		earthSystem.setAngle(earthSystem.getAngle() + 4); 
		(*earthRef).setAngle((*earthRef).getAngle() + 6); 
		satelite.setAngle(satelite.getAngle() + 2); 
		plane.setAngle(plane.getAngle() + 1); 
		moon.setAngle(moon.getAngle() + 3); 
	} else if(key == 't') { 
		tiling = !tiling; 
	} else if(key == 'i') {
		initScene(); 
	} else if(key == 'x') {
		currentView = &lateral;
		updateCamera();
	} else if(key == 'y') {
		currentView = &front;
		updateCamera();
	} else if(key == 'z') {
		currentView = &topView;
		updateCamera();
	} else if(key == 'p') {
		ortho = !ortho;
		updateProjection();
	} else if(key == 'm') {
		scale += 0.05;
		updateProjection();
	} else if(key == 'M') {
		scale -= 0.05;
		updateProjection();
	} else if(key == 'n') {
		moveCamera(_IN);
		updateCamera();
	} else if(key == 'N') {
		moveCamera(_OUT);
		updateCamera();
	} else if(key == 'u') {
		moveCamera(RIGHT);
		updateCamera();
	} else if(key == 'U') {
		moveCamera(LEFT);
		updateCamera();
	} else if(key == 'v') {
		moveCamera(UP);
		updateCamera();
	} else if(key == 'V') {
		moveCamera(DOWN);
		updateCamera();
	} else if(key == 'g') {
		moveCamera(_ROTATE);
		updateCamera();
	} else if(key == 'r') {
		GLdouble uX = currentView->upX;
		GLdouble uY = currentView->upY; 
		GLdouble uZ = currentView->upZ;

		GLdouble length = sqrt((uX*uX)+(uY*uY)+(uZ*uZ));
		uX /= length;
		uY /= length;
		uZ /= length;

		// Rotate "camera position" based on v vector.
		rotate(currentView->eyeX, currentView->eyeY, currentView->eyeZ, uX, uY, uZ, 0.1);
		updateCamera();
	} else if(key == 'R') {
		GLdouble uX = currentView->upX;
		GLdouble uY = currentView->upY; 
		GLdouble uZ = currentView->upZ;

		GLdouble length = sqrt((uX*uX)+(uY*uY)+(uZ*uZ));
		uX /= length;
		uY /= length;
		uZ /= length;

		rotate(currentView->eyeX, currentView->eyeY, currentView->eyeZ, uX, uY, uZ, -0.1);
		updateCamera();
	} else {		
		need_redisplay = false;
	}

	// If current window needs to be redisplayed, redisplay it.
	if (need_redisplay) { 
		glutPostRedisplay(); // Esto vuelve a invocar a "display"
	}
}
//-------------------------------------------------------------------------------------------
// keySp
// description : Use the arrow keys to determine the action.
// parameter   : key - the key input
// parameter   : mX  - no use
// parameter   : mY  - no use
// return      : none
//-------------------------------------------------------------------------------------------
void keySp(int key, int mX, int mY){
  bool need_redisplay = true;
  if(key == GLUT_KEY_UP) glRotatef(5.0, 1.0,0.0,0.0);   
  else if (key == GLUT_KEY_DOWN) glRotatef(-5.0, 1.0,0.0,0.0);  
  else if (key == GLUT_KEY_RIGHT) glTranslatef(5.0, 0.0,0.0); 
  else if(key == GLUT_KEY_LEFT) glTranslatef(-5.0, 0.0,0.0); 
  else need_redisplay = false;

  // If current window needs to be redisplayed, redisplay it.
  if (need_redisplay) glutPostRedisplay();
}
//---------------------------------------------------------------------------
