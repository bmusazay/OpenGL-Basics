#include "Libraries\glew\glew.h"
#include "Libraries\freeglut\glut.h"
#include <iostream>
#include <Math.h>    
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define PI 3.14159265f

void drawButtons();

int winw = 600;
int winh = 600;

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

static void squareButtonCallback()
{
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(50.0f, 500.0f);
	glVertex2f(150.0f, 500.0f);
	glVertex2f(150.0f, 400.0f);
	glVertex2f(50.0f, 400.0f);
	glEnd();
	glFlush();

	//disable button
	Button* b = buttonList;
	while (b)
	{
		if (b->uniqueId == "squareButton")
		{
			b->enabled = false;
		}
		b = b->next;
	}
}

static void traingleButtonCallback()
{
	//TRIANGLE
	glBegin(GL_TRIANGLES);

	glColor3f(1.0f, 0.0f, 0.0f); // Red
	glVertex2f(400.0f, 550.0f);
	glColor3f(0.0f, 1.0f, 0.0f); // Green
	glVertex2f(300.0f, 400.0f);
	glColor3f(0.0f, 0.0f, 1.0f); // Blue
	glVertex2f(500.0f, 400.0f);
	glEnd();
	glFlush();

	//disable button
	Button* b = buttonList;
	while (b)
	{
		if (b->uniqueId == "triangleButton")
		{
			b->enabled = false;
		}
		b = b->next;
	}
}

static void circleButtonCallback()
{
	//CIRCLE
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0f, 0.0f, 1.0f);  // Blue
	glVertex2f(260.0f, 240.0f);       // Center of circle
	int numSegments = 100;
	GLfloat angle;
	for (int i = 0; i <= numSegments; i++) { // Last vertex same as first vertex
		angle = i * 2.0f * PI / numSegments;  // 360 deg for all segments
		glVertex2f(cos(angle) * 75.0f + 260.0f, sin(angle) * 75.0f + 240.0f);
	}
	glEnd();
	glFlush();

	//disable button
	Button* b = buttonList;
	while (b)
	{
		if (b->uniqueId == "circleButton")
		{
			b->enabled = false;
		}
		b = b->next;
	}
}

static void clearButtonCallback()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	Button* b = buttonList;

	//re-enable all buttons
	while (b)
	{
		b->enabled = true;
		b = b->next;
	}
	//redraw buttons
	drawButtons();
}
static void exitButtonCallback()
{
	std::exit(0);
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
	// Set "clearing" or background color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black and opaque

	//create our five buttons
	CreateButton("squareButton", "Draw Square", squareButtonCallback, 40, 71, 140, 35);
	CreateButton("triangleButton", "Draw Triangle", traingleButtonCallback, 230, 71, 140, 35);
	CreateButton("circleButton", "Draw Circle", circleButtonCallback, 420, 71, 140, 35);
	CreateButton("clearButton", "Clear All", clearButtonCallback, 106, 18, 140, 35);
	CreateButton("exitButton", "Exit", exitButtonCallback, 352, 18, 140, 35);
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
		fontx = b->x + (b->w - glutBitmapLength(GLUT_BITMAP_HELVETICA_18,
						(const unsigned char*)b->label)) / 2;
		fonty = b->y + (b->h - 8) / 2;

		// visual effect of label
		if (b->state) {
			fontx += 2;
			fonty += 2;
		}
		if (b->highlighted)
		{
			glColor3f(0, 0, 0);
			Font(GLUT_BITMAP_HELVETICA_18, b->label, fontx, fonty);
			fontx--;
			fonty--;
		}

		glColor3f(1, 1, 1);
		Font(GLUT_BITMAP_HELVETICA_18, b->label, fontx, fonty);
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
	glVertex2i(600, 0);
	glVertex2i(600, 125);
	glVertex2i(0, 125);
	glEnd();
	glFlush();
}

void Draw()
{	
	drawPanel();
	drawButtons();
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
	y = winh - y;
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
	y = winh - y;
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
	y = winh - y;
	//change in location
	int dx = x - TheMouse.x;
	int dy = y - TheMouse.y;

	TheMouse.x = x;
	TheMouse.y = y;
	//check if over button
	ButtonPassive(x, y);
	glutPostRedisplay();
}

int main(int argc, char** argv) 
{
	glutInit(&argc, argv);          // Initialize GLUT
	glutInitWindowSize(winw, winh);   // Set the window's initial width & height
	glutInitWindowPosition(50, 50); // Position the window's initial top-left corner

	glutCreateWindow("Shapes");  // Create window with the given title
	
	glutReshapeFunc(Resize);
	glutDisplayFunc(Draw);       // Register callback handler for window re-paint event
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MousePassiveMotion);
	glutMouseFunc(MouseButton);
	initGL();                       // Our own OpenGL initialization
	glutMainLoop();                 // Enter the event-processing loop
	return 0;
}
