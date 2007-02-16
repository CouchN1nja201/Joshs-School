#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>

void init(void);
void display(void);

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;

class Point2
{
public:
	Point2() {x = y = 0.0f;} 
	Point2(float xx, float yy) {x=xx; y=yy;} 
	void set(float xx, float yy) {x=xx; y=yy;}
	float getX() {return x;}
	float getY() {return y;}
	void draw(void)
	{
		glBegin(GL_POINTS); 
		glVertex2f((GLfloat)x, (GLfloat)y);
		glEnd();
		glFlush();
	}
private:
	float x, y;
};

Point2 testPoint; //the point in question as to whether inside or outside polygon
Point2 polyPoint[100]; //the points in the polygon itself (max of 100)
int polyIndex = 0; //used for the adjustment of the polygon points
int numPoints = 0; //num of points in the polygon array (can be used to loop through populated points)
bool initializeComplete = false; //the polygon has been drawn
bool moveEnabled = false; //used for the movement of the polygon points
bool firstPointDrawn = false; //makes sure messages are not printed before the very first testPoint is drawn
bool pointInside = false; //boolean for inside or outside. Is the ONLY prameter that needs code for the point being inside or outside

//function that needs added code for the point lying inside or outside the given polygon
//variable names kept the same with the exception of the '_' character in front (for local modification if necessary)
void insideOutside(Point2 _testPoint, Point2 *_polyPointPtr, int _numPoints)
{
	//pointInside = true or pointInside = false
	glutPostRedisplay();
}

void myMovedMouse(int x, int y)
{
	if (moveEnabled == true)
	{
		polyPoint[polyIndex].set((float) x, (float) (WINDOW_HEIGHT-y));
		glutPostRedisplay();
	}
}


void myMouseState(int button, int state, int x, int y)
{
	if (initializeComplete == false)
	{
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		{
			polyPoint[numPoints].set((float) x, (float) (WINDOW_HEIGHT-y));
			++numPoints;
			glutPostRedisplay();
		}
		else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
		{
			initializeComplete = true;
			glutPostRedisplay();
		}
	}
	else
	{
		switch (button)
		{
		case (GLUT_RIGHT_BUTTON):
			if (state == GLUT_DOWN)
			{
				moveEnabled = false;
				for (int index=0; index<numPoints; index++)
				{
					if ((x < ((int)polyPoint[index].getX() + 20)) &&
						(x > ((int)polyPoint[index].getX() - 20)) &&
						(y < (WINDOW_HEIGHT - (int)polyPoint[index].getY() + 20)) &&
						(y > (WINDOW_HEIGHT - (int)polyPoint[index].getY() - 20)))
					{
						moveEnabled = true;
						polyIndex = index;
					}
				}
			}
			else if (state == GLUT_UP)
			{
				moveEnabled = false;
				if (firstPointDrawn == true)
				{
					//pointInside = true; //remove this--for test only
					Point2 *polyPointPtr;
					polyPointPtr = &polyPoint[0];
					insideOutside(testPoint, polyPointPtr, numPoints);
				}
			}
			break;

		case (GLUT_LEFT_BUTTON):
			if (state == GLUT_DOWN)
			{
				//pointInside = false; //remove this--for test only
				Point2 *polyPointPtr;
				polyPointPtr = &polyPoint[0];
				testPoint.set((float) x, (float) (WINDOW_HEIGHT-y));
				firstPointDrawn = true;
				insideOutside(testPoint, polyPointPtr, numPoints);
			}
			break;

		default:
			break;
		}
	}
}

int main (int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Team C Project 3");
    init();
	glPointSize(8);
	glLineWidth(3);
	glutDisplayFunc(display);
	glutMouseFunc(myMouseState);
	glutMotionFunc(myMovedMouse);
    glutMainLoop();
    return 0;
}
 
void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
 
void display(void)
{
	char inside[39] = "The point drawn is INSIDE the polygon.";
	char outside[40] = "The point drawn is OUTSIDE the polygon.";
	glClear(GL_COLOR_BUFFER_BIT);
    for (int x=0; x<numPoints; x++)
	{
        glColor3f(1.0, 0.0, 0.0);  
		polyPoint[x].draw();
	}
	if (initializeComplete == true)
	{
		glBegin(GL_LINE_LOOP);
		for (int x=0; x<numPoints; x++)
		{
			glColor3f(0.0, 0.0, 1.0);  
			glVertex2f(polyPoint[x].getX(), polyPoint[x].getY());
		}
		glEnd();
	}
	if (firstPointDrawn == true)
	{
		glColor3f(0.0, 1.0, 0.0);  
		testPoint.draw();
		//
		glRasterPos2i(100,100);
		if (pointInside == true)
		{
			for (int x=0; x<38; x++)
				glutBitmapCharacter(GLUT_BITMAP_8_BY_13, inside[x]);
		}
		else
		{
			for (int x=0; x<39; x++)
				glutBitmapCharacter(GLUT_BITMAP_8_BY_13, outside[x]);
		}
	}
	glutSwapBuffers();
}
