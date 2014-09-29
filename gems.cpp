/*****************************************
 *   HW assignment 0: Option 2 - Gems    *
 *   Tiffany Nguyen                      *
 *   Graphics                            *
 *   Due: September 22, 2014             *
 *****************************************/


#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// Needed on MsWindows
#include <windows.h>
#endif // Win32 platform
#include <math.h>
#include <queue>
#include <set>
#include <iostream>
 
#include <OpenGL/gl.h> //<GL/gl.h>
#include <OpenGL/glu.h>
// Download glut from: http://www.opengl.org/resources/libraries/glut/
#include <GLUT/glut.h>

// compile with
// g++ gems.cpp -framework OpenGL -framework GLUT && ./a.out

using namespace std;

// global variable for grid size
const int gridDimension = 10;

// offsets to deal with creation of shapes
double xOffset = 0;
double yOffset = 0;

// grid to store gem data
int grid[gridDimension][gridDimension];


/*-------------------------------*
 *       Location functions      *
 *-------------------------------*/

// a class used for easy location storage
class Point {
    public:
    int x, y;

    Point(int xIn, int yIn) {
        x = xIn;
        y = yIn;
    } // Point(int, int)

    Point() {}

    // gets the gem value at coordinate x, y
    int getGemVal() {
        return grid[y][x];
    } // getGemVal()
}; // class Point

// converts a point to an integer for easier set storage
int pointToInt(Point p) {
    return (p.x + (p.y * gridDimension));
} // pointToInt

// converts an integer corresponding to the grid coordinate system into a point
Point intToPoint(int i) {
    int x = i % gridDimension;
    int y = (i - x) / gridDimension;
    return Point(x,y);
} // intToPoint

/*  
 *  Breadth-first search
 *  findPath finds a path of gems that are the same as the gem at coordinate x, y
 *  it returns a null set if the set is less than 4 elements long and 
 *      a set of ints if equal to or longer than 4 elements
 *  algorithm understanding help from Preston T-W 
 */
set<int> findPath(int x, int y) { 
    queue<Point> toVisit;
    set<int> visited;

    int gem = grid[y][x]; // to keep track of gem value
    toVisit.push(Point(x, y));

    while (!toVisit.empty()) {
        Point current = toVisit.front();
        toVisit.pop();

        int currCol = current.x;
        int currRow = current.y;
        int i = 0;
        Point vals[4];

        // ensure that the coordinates do not exceed bounds of the grid
        // always 4 cases because only 4 directions around a gem
        if (currRow > 0) {
            vals[i++] = Point(currCol, currRow-1);
        } // if
        if (currRow < (gridDimension - 1)) {
            vals[i++] = Point(currCol, currRow+1);
        } // if
        if (currCol > 0) {
            vals[i++] = Point(currCol-1, currRow);
        } // if
        if (currCol < (gridDimension - 1)) {
            vals[i++] = Point(currCol+1, currRow);
        } // if

        for (int j = 0; j < i; j++) {
            // check that the point is the required gem and has not been visited before
            if ((vals[j].getGemVal() == gem) && 
                (visited.insert(pointToInt(vals[j])).second)) {
                toVisit.push(vals[j]);
            } // if
        } // for

        visited.insert(pointToInt(current));
        // cout << toVisit.size() << endl;
    } // while

    if (visited.size() > 3) {
        return visited;
    } else {
        // returns an empty set to indicate that no gems are to be removed
        return set<int>();
    } // if/else
} // findPath(int, int)

// "removes" the gems located at the positions defined in path by replacing them with a delimiter, -1
void removePath(set<int> path) {
    if (!path.empty()) {
        // replaces all locations in the set with a delimiter
        for (set<int>::iterator it=path.begin(); it != path.end(); ++it) {
            Point point = intToPoint(*it);
            grid[point.y][point.x] = -1;
        } // for
    } else {
        return;
    } // if/else
} // removePath(set<int>)

// moves the gem value located at x, y to the "top" of the grid,
//     moving all gems above x, y down a slot
void moveGemUp(int x, int y) {
    int a = y;
    while (a > 0) {
        int temp = grid[a][x];
        grid[a][x] = grid[a-1][x];
        grid[a-1][x] = temp;
        a--;
    } // while
} // parseGemUp(int, int)

// "drops down" the gems above a delimiter
void dropDown() {
    for (int i = 0; i < gridDimension; i++) {
        for (int j = 0; j < gridDimension; j++) {
            if (grid[i][j] == -1) {
                moveGemUp(j, i);
            } // if
        } // for
    } // for
} // dropDown()

/*-------------------------------*
 *       Board functions         *
 *-------------------------------*/

// initialize board to delimiter, -1 to indicate empty space
void initializeBoard() {
    for (int i = 0; i < gridDimension; i++) {
        for (int j = 0; j < gridDimension; j++) {
            grid[i][j] = -1;
        } // for
    } // for
} // initializeBoard()

// fills in all empty spaces (denoted by -1) with random gems
void fillInRandom() {
    srand (time(NULL));
    for (int i = 0; i < gridDimension; i++) {
        for (int j = 0; j < gridDimension; j++) {
            if (grid[i][j] == -1) {
                grid[i][j] = rand() % 4;
            } // if
        } // for
    } // for
} // fillInRandom()


/*-------------------------------*
 *       Drawing functions       *
 *-------------------------------*/

// draws a square 
int drawSquare() {
    glBegin(GL_QUADS);
        glColor3d(0.0, 0.0, 1.0); 
        glVertex2d(0.0 - xOffset, 0.0 - yOffset);
        glVertex2d((1.0/gridDimension) - xOffset, 0.0 - yOffset);
        glVertex2d((1.0/gridDimension) - xOffset, (1.0/gridDimension) - yOffset);
        glVertex2d(0.0 - xOffset, (1.0/gridDimension) - yOffset);
    glEnd();
    return 0;
} // drawSquare()

// draws a triangle
int drawTriangle() {
    glBegin(GL_TRIANGLE_STRIP);
        glColor3d(1.0, 0.0, 0.0); 
        glVertex2d(0.0 - xOffset, 0.0 - yOffset);
        glVertex2d((0.5/gridDimension) - xOffset, (1.0/gridDimension) - yOffset);
        glVertex2d((1.0/gridDimension) - xOffset, 0.0 - yOffset);
    glEnd();
    return 1;
} // drawTriangle()

// draws a hexagon
int drawHexagon() {
    glBegin(GL_POLYGON);
        glColor3d(0.0, 1.0, 0.0); 
        glVertex2d(0.0 - xOffset, (0.5/gridDimension) - yOffset);
        glVertex2d((0.25/gridDimension) - xOffset, (1.0/gridDimension) - yOffset);
        glVertex2d((0.75/gridDimension) - xOffset, (1.0/gridDimension) - yOffset);
        glVertex2d((1.0/gridDimension) - xOffset, (0.5/gridDimension) - yOffset);
        glVertex2d((0.75/gridDimension) - xOffset, 0.0 - yOffset);
        glVertex2d((0.25/gridDimension) - xOffset, 0.0 - yOffset);
    glEnd();
    return 2;
} // drawHexagon()

// draws a diamond
int drawDiamond() {
    glBegin(GL_QUADS);
        glColor3d(0.0, 1.0, 1.0);
        glVertex2d((0.25/gridDimension) - xOffset, (0.5/gridDimension) - yOffset);
        glVertex2d((0.5/gridDimension) - xOffset, (1.0/gridDimension) - yOffset);
        glVertex2d((0.75/gridDimension) - xOffset, (0.5/gridDimension) - yOffset);
        glVertex2d((0.5/gridDimension) - xOffset, 0.0 - yOffset);
    glEnd();
    return 3;
} // drawDiamond

// draws the board
void drawBoard() {
    int shape = 0;
    double increment = 2.0/gridDimension;
    double cellOffset = .5/gridDimension;
    yOffset = -1 + (increment - cellOffset);
    for (int i = 0; i < gridDimension; i++) {
        xOffset = 1 - cellOffset;
        for (int j = 0; j < gridDimension; j++) {
            switch(grid[i][j]) {
                case 0:
                    drawSquare();
                    break;
                case 1:
                    drawTriangle();
                    break;
                case 2:
                    drawHexagon();
                    break;
                case 3:
                    drawDiamond();
                    break;
                default:
                    printf("Invalid identifier\n");
            } // switch
            xOffset -= increment;
        } // for
        yOffset += increment;
    } // for
    glutPostRedisplay();
} // drawBoard()


/*-------------------------------*
 *       Display functions       *
 *-------------------------------*/

// Display callback that is responsible for updating the screen
void onDisplay() {
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT); //or GL_DEPTH_BUFFER_BIT

    drawBoard();    

    glutSwapBuffers();

} // onDisplay

// Will deal with window size and scaling
void onReshape(int winWidth0, int winHeight0) {
    // Will prevent the screen from reshaping when the window size changes if nothing is here
    glViewport(0, 0, winWidth0, winHeight0);

}

// actions on mouse usage
void onMouse(int button, int state, int x, int y) {
    int xcoord;
    int ycoord;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        xcoord = x/(glutGet(GLUT_WINDOW_WIDTH)/gridDimension);
        ycoord = y/(glutGet(GLUT_WINDOW_HEIGHT)/gridDimension);
        removePath(findPath(xcoord, ycoord));
        dropDown();
        fillInRandom();
	} // if
} // onMouse


/*------------------*
 *       Main       *
 *------------------*/
int main(int argc, char *argv[]) {
    glutInit(&argc, argv);                 		// GLUT initialization
    glutInitWindowSize(640, 640);				// Initial resolution of the MsWindows Window is 600x600 pixels 
    glutInitWindowPosition(100, 100);            // Initial location of the MsWindows window
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);    // Image = 8 bit R,G,B + double buffer + depth buffer
 
    glutCreateWindow("Gems_Game");        	// Window is born
  
    initializeBoard();
    fillInRandom();

    glutDisplayFunc(onDisplay);                	// Register event handlers
    glutReshapeFunc(onReshape);                 // See onReshape function above
    glutMouseFunc(onMouse);
 
    glutMainLoop();                    			// Event loop  
    return 0;
} // main
