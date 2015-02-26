#include "Libraries\glew\glew.h"
#include "Libraries\freeglut\glut.h"
#include <iostream>
#include <Math.h>    
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Libraries\glm\glm.hpp"
#include "Libraries\glm\gtc\matrix_transform.hpp"
#include "Libraries\glm\gtc\type_ptr.hpp"

#define PI 3.14159265f

void drawButtons();

int winw = 720;
int winh = 610;
GLfloat anglePyramid = 0.0f;  
GLfloat angleCube = 0.0f;  
GLfloat anglueSphere = 0.0f;
bool rotatePyramid = false;
bool rotateSphere = false;
bool rotateCube = false;
float sX, sY, pX, pY, cX, cY;
float scaleS, scaleP, scaleC;
float move = 0.0f;


int refreshRate = 15;

//struct for keeping track of mouse input with GLUT functions
struct Mouse
{
	int x;		/*	the x coordinate of the mouse cursor	*/
	int y;		/*	the y coordinate of the mouse cursor	*/
	int lmb;	/*	is the left button pressed?		*/
	int mmb;	/*	is the middle button pressed?	*/
	int rmb;	/*	is the right button pressed?	*/
	int xpress; /*	stores the x-coord of when the first button press occurred	*/
	int ypress; /*	stores the y-coord of when the first button press occurred	*/
};
typedef struct Mouse Mouse;

//global instance of mouse information
Mouse TheMouse = { 0, 0, 0, 0, 0 };

typedef void(*ButtonCallback)();
struct Button
{
	int   x;							/* top left x coord of the button */
	int   y;							/* top left y coord of the button */
	int   w;							/* the width of the button */
	int   h;							/* the height of the button */
	int	  state;						/* the state, 1 if pressed, 0 otherwise */
	int	  highlighted;					/* is the mouse cursor over the control? */
	char* label;						/* the text label of the button */
	bool enabled;						/* is this button enabled (shape drawn or not)*/
	ButtonCallback callbackFunction;	/* A pointer to a function to call if the button is pressed */
	std::string uniqueId;								/* A unique ID to represent this Button */
	struct Button* next;				/* a pointer to the next node in the linked list */
};
typedef struct Button Button;


Button* buttonList = NULL; //button list


//adds new button to button list
void CreateButton(std::string id, char *label, ButtonCallback cb, int x, int y, int w, int h)
{
	Button* p = (Button*)malloc(sizeof(Button));
	memset(p, 0, sizeof(Button));
	p->x = x;
	p->y = y;
	p->w = w;
	p->h = h;
	p->enabled = true;
	p->callbackFunction = cb;
	p->label = label;
	p->uniqueId = id;
	p->next = buttonList;
	buttonList = p;
}

void enableButton(std::string id)
{
	Button* b = buttonList;
	while (b)
	{
		if (b->uniqueId == id)
		{
			b->enabled = true;
		}
		b = b->next;
	}
}

void disableButton(std::string id)
{
	Button* b = buttonList;
	while (b)
	{
		if (b->uniqueId == id)
		{
			b->enabled = false;
		}
		b = b->next;
	}
}

static void exitButtonCallback()
{
	std::exit(0);
}
static void rotateAllBcb()
{
	if (rotateSphere && rotatePyramid && rotateCube)
	{
		rotateSphere = false;
		rotatePyramid = false;
		rotateCube = false;
	}
	 else 
	 {
		 rotateSphere = true;
		 rotatePyramid = true;
		 rotateCube = true;
	 }
}
static void rotateSphereBcb()
{
	rotateSphere = rotateSphere ? 0 : 1;
}
static void rotatePyrmaidBcb()
{
	rotatePyramid = rotatePyramid ? 0 : 1;
}
static void rotateCubeBcb()
{
	rotateCube = rotateCube ? 0 : 1;
}
static void allLeftBcb()
{
	sX -= 0.1f;
	cX -= 0.1f;
	pX -= 0.1f;
}
static void allRightBcb()
{
	sX += 0.1f;
	cX += 0.1f;
	pX += 0.1f;
}
static void allUpBcb()
{
	sY += 0.1f;
	cY += 0.1f;
	pY += 0.1f;
}
static void allDownBcb()
{
	sY -= 0.1f;
	cY -= 0.1f;
	pY -= 0.1f;
}

static void sLeftBcb()
{
	sX -= 0.1f;
}
static void sRightBcb()
{
	sX += 0.1f;
}
static void sUpBcb()
{
	sY += 0.1f;
}
static void sDownBcb()
{
	sY -= 0.1f;
}

static void pLeftBcb()
{
	pX -= 0.1f;
}
static void pRightBcb()
{
	pX += 0.1f;
}
static void pUpBcb()
{
	pY += 0.1f;
}
static void pDownBcb()
{
	pY -= 0.1f;
}

static void cLeftBcb()
{
	cX -= 0.1f;
}
static void cRightBcb()
{
	cX += 0.1f;
}
static void cUpBcb()
{
	cY += 0.1f;
}
static void cDownBcb()
{
	cY -= 0.1f;
}

static void scaleAllBcb()
{
	scaleS += 0.1f;
	scaleP += 0.1f;
	scaleC += 0.1f;
}
static void scaleSphereBcb()
{
	scaleS += 0.1f;
}
static void scalePyramidBcb()
{
	scaleP += 0.1f;
}
static void scaleCubeAllBcb()
{
	scaleC += 0.1f;
}
static void moveinCB()
{
	move += 0.1f;
}
static void moveoutCB()
{
	move -= 0.1f;
}
//Font function for drawing text to buttons
void Font(void *font, char *text, int x, int y)
{
	glRasterPos2i(x, y);
	while (*text != '\0')
	{
		glutBitmapCharacter(font, *text);
		++text;
	}
}


int ButtonClickTest(Button* b, int x, int y)
{
	if (b)
	{
		//if clicked within button area
		if (x > b->x      &&
			x < b->x + b->w &&
			y > b->y      &&
			y < b->y + b->h) {
			return 1;
		}
	}
	return 0;
}

void ButtonRelease(int x, int y)
{
	Button* b = buttonList;
	while (b)
	{
		//check if click area was within current button
		if (ButtonClickTest(b, TheMouse.xpress, TheMouse.ypress) &&
			ButtonClickTest(b, x, y))
		{
			//if yes, call its function
			if (b->callbackFunction) {
				b->callbackFunction();
			}
		}
		b->state = 0;
		b = b->next;
	}
}

void ButtonPress(int x, int y)
{
	Button* b = buttonList;
	while (b)
	{
		//if clicked within button set true
		if (ButtonClickTest(b, x, y))
		{
			b->state = 1;
		}
		b = b->next;
	}
}

void ButtonPassive(int x, int y)
{
	int needRedraw = 0;
	Button* b = buttonList;
	while (b)
	{
		//if mouse over button
		if (ButtonClickTest(b, x, y))
		{
			//set highlight
			if (b->highlighted == 0) {
				b->highlighted = 1;
				needRedraw = 1;
			}
		}
		else if (b->highlighted == 1)
		{
			b->highlighted = 0;
			needRedraw = 1;
		}
		b = b->next;
	}
	if (needRedraw) {
		glutPostRedisplay();
	}
}


/* Initialize OpenGL Graphics and Buttons */
void initGL()
{
	glEnable(GL_LIGHT0);
	sX = sY = pX = pY = cX = cY = 0.0f;
	scaleS = scaleP = scaleC = 1.0f;
	
	//create our five buttons
	//CreateButton("squareButton", "Draw Square", squareButtonCallback, 40, winh - 71, 140, 35);
	//CreateButton("triangleButton", "Draw Triangle", traingleButtonCallback, 230, 71, 140, 35);
	//CreateButton("circleButton", "Draw Circle", circleButtonCallback, 420, 71, 140, 35);
	//CreateButton("clearButton", "Clear All", clearButtonCallback, 106, 18, 140, 35);
	CreateButton("exitButton", "Exit", exitButtonCallback, 10, 10, 50, 25);

	CreateButton("rotateAllButton", "Rotate All", rotateAllBcb, 10, winh - 140 + 40, 95, 25);
	CreateButton("leftAllButton", "<-", allLeftBcb, 10, winh - 105 + 40, 20, 25);
	CreateButton("upAllButton", "^", allUpBcb, 35, winh - 105 + 40, 20, 25);
	CreateButton("downAllButton", "V", allDownBcb, 60, winh - 105 + 40, 20, 25);
	CreateButton("rightAllButton", "->", allRightBcb, 85, winh - 105 + 40, 20, 25);
	CreateButton("scaleAllButton", "Scale All", scaleAllBcb, 10, winh - 70 + 40, 95, 25);
	//createbutton

	CreateButton("rotateSphereButton", "Rotate Sphere", rotateSphereBcb, 160, winh - 140 + 40, 95, 25);
	CreateButton("leftSphereButton", "<-", sLeftBcb, 160, winh - 105 + 40, 20, 25);
	CreateButton("upSphereButton", "^", sUpBcb, 150 + 35, winh - 105 + 40, 20, 25);
	CreateButton("downSphereButton", "V", sDownBcb, 150 + 60, winh - 105 + 40, 20, 25);
	CreateButton("rightSphereButton", "->", sRightBcb, 150 + 85, winh - 105 + 40, 20, 25);
	CreateButton("scaleSphereButton", "Scale Sphere", scaleSphereBcb, 160, winh - 70 + 40, 95, 25);
	//shear

	CreateButton("rotatePyramidButton", "Rotate Pyramid", rotatePyrmaidBcb, 310, winh - 140 + 40, 95, 25);
	CreateButton("leftPyramidButton", "<-", pLeftBcb, 310, winh - 105 + 40, 20, 25);
	CreateButton("upPyramidButton", "^", pUpBcb, 310 + 25, winh - 105 + 40, 20, 25);
	CreateButton("downPyramidButton", "V", pDownBcb, 310 + 50, winh - 105 + 40, 20, 25);
	CreateButton("rightPyramidButton", "->", pRightBcb, 310 + 75, winh - 105 + 40, 20, 25);
	CreateButton("scalePyramidButton", "Scale Pyramid", scalePyramidBcb, 310, winh - 70 + 40, 95, 25);

	CreateButton("rotateCubeButton", "Rotate Cube", rotateCubeBcb, 460, winh - 140 + 40, 95, 25);
	CreateButton("leftCubeButton", "<-", cLeftBcb, 460, winh - 105 + 40, 20, 25);
	CreateButton("upCubeButton", "^", cUpBcb, 460 + 25, winh - 105 + 40, 20, 25);
	CreateButton("downCubeButton", "V", cDownBcb, 460 + 50, winh - 105 + 40, 20, 25);
	CreateButton("rightCubeButton", "->", cRightBcb, 460 + 75, winh - 105 + 40, 20, 25);
	CreateButton("scaleCubeButton", "Scale Cube", scaleCubeAllBcb, 460, winh - 70 + 40, 95, 25);

	CreateButton("moveinButton", "Move In", moveinCB, 610, winh - 140 + 40, 95, 25);
	CreateButton("moveoutButton", "Move Out", moveoutCB, 610, winh - 105 + 40, 95, 25);

}


void drawButtons()
{
	Button* b = buttonList;
	int fontx;
	int fonty;
	while (b)
	{
		//button has already been clicked


		//change color if highlighted or disabled
		if (!b->enabled) {
			glColor3f(0.555f, 0.555f, 0.6f);
		}
		else if (b->highlighted)
			glColor3f(0.7f, 0.7f, 0.8f);
		else
			glColor3f(0.6f, 0.6f, 0.6f);

		//main button background
		glBegin(GL_QUADS);
		glVertex2i(b->x, b->y);
		glVertex2i(b->x, b->y + b->h);
		glVertex2i(b->x + b->w, b->y + b->h);
		glVertex2i(b->x + b->w, b->y);
		glEnd();
		glLineWidth(2);

		if (!b->enabled) {
			b = b->next;
			continue;
		}
		//outline
		if (b->state)
			glColor3f(0.4f, 0.4f, 0.4f);
		else
			glColor3f(0.8f, 0.8f, 0.8f);

		glBegin(GL_LINE_STRIP);
		glVertex2i(b->x + b->w, b->y);
		glVertex2i(b->x, b->y);
		glVertex2i(b->x, b->y + b->h);
		glEnd();

		if (b->state)
			glColor3f(0.8f, 0.8f, 0.8f);
		else
			glColor3f(0.4f, 0.4f, 0.4f);

		glBegin(GL_LINE_STRIP);
		glVertex2i(b->x, b->y + b->h);
		glVertex2i(b->x + b->w, b->y + b->h);
		glVertex2i(b->x + b->w, b->y);
		glEnd();

		glLineWidth(1);
		//Calculate the x and y coords for the text string in order to center it.
		fontx = b->x + (b->w - glutBitmapLength(GLUT_BITMAP_HELVETICA_10,
			(const unsigned char*)b->label)) / 2;
		fonty = b->y + (b->h + 5) / 2;

		// visual effect of label
		if (b->state) {
			fontx += 2;
			fonty += 2;
		}
		if (b->highlighted)
		{
			glColor3f(0, 0, 0);
			Font(GLUT_BITMAP_HELVETICA_10, b->label, fontx, fonty);
			fontx--;
			fonty--;
		}

		glColor3f(1, 1, 1);
		Font(GLUT_BITMAP_HELVETICA_10, b->label, fontx, fonty);
		b = b->next;
	}

}

void drawPanel()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, winw, 0, winh, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(0.7f, 0.7f, 0.8f);
	glColor3f(0.758824f, 0.758824f, 0.758824f);
	glBegin(GL_QUADS);
	glVertex2i(0, 0);
	glVertex2i(winw, 0);
	glVertex2i(winw, 125);
	glVertex2i(0, 125);
	glEnd();
	glFlush();
}

void Draw2D()
{
	drawButtons();
}

void Draw3D()
{	
	glTranslatef(0.0f, 0.0f, -1.0f + move);
	// DRAW CUBE
	glPushMatrix();
	glTranslatef(1.5f + cX, 0.0f + cY, -7.0f);
	glRotatef(angleCube, 1.0f, 0.0f, 0.0f);
	glScalef(scaleC, scaleC, scaleC);
	glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
	// Top face (y = 0.54f)
	glColor3f(0.0f, 1.0f, 0.0f);    
	glVertex3f(0.54f, 0.54f, -0.54f);
	glVertex3f(-0.54f, 0.54f, -0.54f);
	glVertex3f(-0.54f, 0.54f, 0.54f);
	glVertex3f(0.54f, 0.54f, 0.54f);

	// Bottom face (y = -0.54f)
	glVertex3f(0.54f, -0.54f, 0.54f);
	glVertex3f(-0.54f, -0.54f, 0.54f);
	glVertex3f(-0.54f, -0.54f, -0.54f);
	glVertex3f(0.54f, -0.54f, -0.54f);


	// Front face  (z = 0.54f)
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.54f, 0.54f, 0.54f);
	glVertex3f(-0.54f, 0.54f, 0.54f);
	glVertex3f(-0.54f, -0.54f, 0.54f);
	glVertex3f(0.54f, -0.54f, 0.54f);

	// Back face (z = -0.54f)
	glVertex3f(0.54f, -0.54f, -0.54f);
	glVertex3f(-0.54f, -0.54f, -0.54f);
	glVertex3f(-0.54f, 0.54f, -0.54f);
	glVertex3f(0.54f, 0.54f, -0.54f);

	// Left face (x = -0.54f)
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(-0.54f, 0.54f, 0.54f);
	glVertex3f(-0.54f, 0.54f, -0.54f);
	glVertex3f(-0.54f, -0.54f, -0.54f);
	glVertex3f(-0.54f, -0.54f, 0.54f);

	// Right face (x = 0.54f)
	glVertex3f(0.54f, 0.54f, -0.54f);
	glVertex3f(0.54f, 0.54f, 0.54f);
	glVertex3f(0.54f, -0.54f, 0.54f);
	glVertex3f(0.54f, -0.54f, -0.54f);
	glEnd();  // End of drawing color-cube
	glPopMatrix();

	
	//DRAW PYRAMID
	glPushMatrix();
	glTranslatef(-1.5f + pX, 0.0f + pY, -6.0f);
	  // Move left and into the screen

	glRotatef(anglePyramid, 1.0f, 1.0f, 1.0f);
	glScalef(scaleP, scaleP, scaleP);
	glBegin(GL_TRIANGLES);          
	// Front
	glColor3f((208.0f / 255.0f), (17.0f / 255.0f), (232.0f / 255.0f));
	glVertex3f(0.0f, 0.7f, 0.0f);
	glVertex3f(-0.7f, -0.7f, 0.7f);
	glVertex3f(0.7f, -0.7f, 0.7f);

	// Right
	glColor3f((147.0f / 255.0f), (17.0f / 255.0f), (232.0f / 255.0f));
	glVertex3f(0.0f, 0.7f, 0.0f);
	glVertex3f(0.7f, -0.7f, 0.7f);
	glVertex3f(0.7f, -0.7f, -0.7f);

	// Back
	glColor3f((205.0f / 255.0f), (63.0f / 255.0f), (204.0f / 255.0f));
	glVertex3f(0.0f, 0.7f, 0.0f);
	glVertex3f(0.7f, -0.7f, -0.7f);
	glVertex3f(-0.7f, -0.7f, -0.7f);

	// Left
	glColor3f((219.0f / 255.0f), (147.0f / 255.0f), (255.0f / 255.0f));
	glVertex3f(0.0f, 0.7f, 0.0f);
	glVertex3f(-0.7f, -0.7f, -0.7f);
	glVertex3f(-0.7f, -0.7f, 0.7f);
	glEnd();   // Done drawing the pyramid
	glPopMatrix();

	//DRAW SPHERE
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glPushMatrix();
	glColor3f((232.0f / 255.0f), (46.0f / 255.0f), (47.0f / 255.0f));
	glTranslated(0.0f + sX, 2.0f + sY, -8.0f);
	glRotatef(anglueSphere, 0.0f, 1.0f, 0.0f);
	glScalef(scaleS, scaleS, scaleS);
	glutWireSphere(1, 35, 35);
	glPopMatrix();
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	if (rotatePyramid) anglePyramid += 0.2f;
	if(rotateCube) angleCube -= 0.15f;
	if (rotateSphere) anglueSphere += 0.4f;
}

void Draw()
{
	//prepare for 3d drawing
	glClear(GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
	glShadeModel(GL_SMOOTH);   // Enable smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (winh == 0) ? (1) : ((float)winw / winh), 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Draw3D();
	
	//prepare for 2d overlay
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, winw, winh, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Draw2D();
	glutSwapBuffers();
}

void Resize(int w, int h)
{
	winw = w;
	winh = h;
	glViewport(0, 0, w, h);
}

//Keep track of mouse hover position
void MousePassiveMotion(int x, int y)
{
	//y = winh - y;
	//change in mouse position
	int dx = x - TheMouse.x;
	int dy = y - TheMouse.y;
	//update values
	TheMouse.x = x;
	TheMouse.y = y;
	ButtonPassive(x, y);
}

//Function for mouse clicks
void MouseButton(int button, int state, int x, int y)
{
	//y = winh - y;
	//update the mouse position	
	TheMouse.x = x;
	TheMouse.y = y;

	//button pressed
	if (state == GLUT_DOWN)
	{
		TheMouse.xpress = x;
		TheMouse.ypress = y;
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			TheMouse.lmb = 1;
			ButtonPress(x, y);
		case GLUT_MIDDLE_BUTTON:
			TheMouse.mmb = 1;
			break;
		case GLUT_RIGHT_BUTTON:
			TheMouse.rmb = 1;
			break;
		}
	}
	else //button released
	{
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			TheMouse.lmb = 0;
			ButtonRelease(x, y);
			break;
		case GLUT_MIDDLE_BUTTON:
			TheMouse.mmb = 0;
			break;
		case GLUT_RIGHT_BUTTON:
			TheMouse.rmb = 0;
			break;
		}
	}
	glutPostRedisplay();
}

void MouseMotion(int x, int y)
{
	//y = winh - y;
	//change in location
	int dx = x - TheMouse.x;
	int dy = y - TheMouse.y;

	TheMouse.x = x;
	TheMouse.y = y;
	//check if over button
	ButtonPassive(x, y);

	glutPostRedisplay();
}

void timer(int value) {
	glutPostRedisplay();      // Post re-paint request to activate display()
	glutTimerFunc(refreshRate, timer, 0); // next timer call milliseconds later
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);          // Initialize GLUT
	glutInitWindowSize(winw, winh);   // Set the window's initial width & height
	glutInitWindowPosition(50, 50); // Position the window's initial top-left corner

	glutCreateWindow("3D Shapes and Translations");  // Create window with the given title
	
	glutReshapeFunc(Resize);
	glutDisplayFunc(Draw);       // Register callback handler for window re-paint event
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MousePassiveMotion);
	glutMouseFunc(MouseButton);
	glutTimerFunc(0, timer, 0);
	initGL();                       // Our own OpenGL initialization
	glutMainLoop();                 // Enter the event-processing loop
	return 0;
}
