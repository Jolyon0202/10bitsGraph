#include <GLUT/glut.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <time.h>
#include <vector>
using namespace std;

GLdouble width = 512, height = 512;
unsigned int userRed = 0;
unsigned int userGreen = 0;
unsigned int userBlue = 255;


// Auxiliary union used to set the values of the 10 bit texture
union format2_10_10_10 {
  struct {
    unsigned int red : 10;
    unsigned int green : 10;
    unsigned int blue : 10;
    unsigned int alpha : 2;
  } bits;
  unsigned int u32All;
};

/* Handler for window-repaint event. Call back when the window first appears and
 whenever the window needs to be re-painted. */
void display() {
  // Tell OpenGL to use the whole window for drawing
  glViewport(0, 0, width, height); // DO NOT CHANGE THIS LINE

  // Do an orthographic parallel projection with the coordinate
  // system set to first quadrant, limited by screen/window size
  glMatrixMode(GL_PROJECTION);                 // DO NOT CHANGE THIS LINE
  glLoadIdentity();                            // DO NOT CHANGE THIS LINE
  glOrtho(0.0, width, height, 0.0, -1.f, 1.f); // DO NOT CHANGE THIS LINE

  // Clear the color buffer with current clearing color
  glClear(GL_COLOR_BUFFER_BIT); // DO NOT CHANGE THIS LINE

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // DO NOT CHANGE THIS LINE

  // cout << flappyHighScore << endl << spaceRunnerHighScore << endl;
  glFlush(); // Render now

  // Map the texture on a screen size quad for display
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0);
  glVertex2f(0.0, 0.0);
  glTexCoord2f(0.0, 1.0);
  glVertex2f(0.0, height);
  glTexCoord2f(1.0, 1.0);
  glVertex2f(width, height);
  glTexCoord2f(1.0, 0.0);
  glVertex2f(width, 0.0);
  glEnd();
  glFlush();
  glutSwapBuffers();
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv) {

  glutInit(&argc, argv); // Initialize GLUT

  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

  glutInitWindowSize((int)width, (int)height);
  glutInitWindowPosition(0, 0); // Position the window's initial top-left corner

  // request 10 bit colors, a depth buffer, and double buffering
  glutInitDisplayString("red=10 green=10 blue=10 depth double");

  /* create the window and store the handle to it */
  int wd = glutCreateWindow("Jolyon Machine" /* title */);

  // verify that each color channel has 10 bits
  int red_bits, green_bits, blue_bits;
  glGetIntegerv(GL_RED_BITS, &red_bits);
  glGetIntegerv(GL_GREEN_BITS, &green_bits);
  glGetIntegerv(GL_BLUE_BITS, &blue_bits);
  printf("red %u bits, green %u bits, blue %u bits\n", red_bits, green_bits,
         blue_bits);

  // Allocate a buffer which will be used as the 10 bit texture
  format2_10_10_10 *drawData = new format2_10_10_10[width * height];

  // Fill the buffer with values from the user. userRed, userGreen,
  // and userBlue are unsigned integers in the range [0,1024).
  format2_10_10_10 aux;
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x) {
      aux.bits.red = userRed;     // user input
      aux.bits.green = userGreen; // user input
      aux.bits.blue = userBlue;   // user input
      drawData[(int)(x + y * width)].u32All = aux.u32All;
    }

  // Create and bind a new texture
  GLuint texID;
  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);
  // Fill the texture image from the 10 bit buffer data
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB10_A2, width, height, 0, GL_RGBA,
               GL_UNSIGNED_INT_2_10_10_10_REV, drawData);
  // Set desired texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  // Register callback handler for window re-paint event
  glutDisplayFunc(display);

  // Enter the event-processing loop
  glutMainLoop();
  return 0;
}
