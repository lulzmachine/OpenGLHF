
#include <GL/glut.h>
#include <math.h>

#include "helpers.h"
#define crossProduct(a,b,c) \
	(a)[0] = (b)[1] * (c)[2] - (c)[1] * (b)[2]; \
	(a)[1] = (b)[2] * (c)[0] - (c)[2] * (b)[0]; \
	(a)[2] = (b)[0] * (c)[1] - (c)[0] * (b)[1];


GLuint groundTexture;
int height, width;
unsigned char *imagedata;

Model* klingoff;

void renderGround()
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  
// Your terrain should match this flat quad
  
  glBegin(GL_POLYGON);
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 10);
  glVertex3f(-100, -0.01, 100);
  glTexCoord2f(0, 0);
  glVertex3f(100, -0.01, 100);
  glTexCoord2f(10, 0);
  glVertex3f(100, -0.01, -100);
  glTexCoord2f(10, 10);
  glVertex3f(-100, -0.01, -100);
  glEnd();
  glPopAttrib();
}

void normalize(float* a) {
	float len = sqrt(pow(a[0],2) + pow(a[1],2) + pow(a[2],2));
           a[0] = a[0]/len;
	   a[1] = a[1]/len;
	   a[2] = a[2]/len;
}


void renderTerrain()
{
int row,col;
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  int sideLength = 200;
  // set texLength to 20.ssss
  int texLength = 20;

  float heightNodes = height - 1;
  float widthNodes = width - 1;
float heightDivisorer = 3;


float haxPos[] = { 0+getElapsedTime(), 0, 50 };
int quad[] = { ((haxPos[0] + sideLength / 2) / (sideLength/widthNodes))
		, ((haxPos[2] + sideLength / 2) / (sideLength/heightNodes)) };
//printf("quad: %d, %d \n", quad[0], quad[1]);
//printf("heightNodes, Widthnodes, height, width: %f, %f\n", heightNodes, widthNodes);

    for (row = 0; row < heightNodes; row++)	{
      for (col = 0; col < widthNodes; col++) {
//printf("row: %d", row);
float xLow = row*(sideLength/widthNodes) - sideLength / 2;
float xHigh = (row+1)*(sideLength/widthNodes) - sideLength / 2;

float zLow = col*(sideLength/heightNodes) - sideLength / 2;
float zHigh = (col+1)*(sideLength/heightNodes) - sideLength / 2;

float yLowLow = (int)imagedata[(row*width + col)*3] / heightDivisorer;
float yLowHigh = (int)imagedata[(row*width + col+1)*3] / heightDivisorer;
float yHighLow = (int)imagedata[((row+1)*width + col)*3] / heightDivisorer;
float yHighHigh = (int)imagedata[((row+1)*width + col + 1)*3] / heightDivisorer;



float texnumb = sideLength/widthNodes;

glPushMatrix();
glTranslatef(0, 10, 0);
glBegin(GL_POLYGON);
//First triangle
float a[3];
float b[] = {xHigh-xLow,yHighLow-yLowLow , zLow-zLow};
float c[] = {xLow-xLow,yLowHigh-yLowLow, zHigh-zLow};
crossProduct(a, c, b);
normalize(a);
glNormal3f(a[0], a[1], a[2]);
glTexCoord2f(0, 0);
glVertex3f(xLow, yLowLow, zLow);
glTexCoord2f(0, texnumb);
glVertex3f(xHigh, yHighLow, zLow);
glTexCoord2f(texnumb, 0);
glVertex3f(xLow , yLowHigh, zHigh);
glEnd();
glBegin(GL_POLYGON);
//Second triangle
float d[] = {xHigh-xLow, yHighLow-yLowHigh, zLow-zHigh};
float e[] = {xHigh-xLow, yHighHigh-yLowHigh, zHigh-zHigh};
crossProduct(a, e, d);
normalize(a);
glNormal3f(a[0], a[1], a[2]);
glTexCoord2f(texnumb, 0);
glVertex3f(xLow , yLowHigh, zHigh);
glTexCoord2f(texnumb, texnumb);
glVertex3f(xHigh, yHighLow, zLow);
glTexCoord2f(0, texnumb);
glVertex3f(xHigh, yHighHigh, zHigh);
glEnd();
glPopMatrix();

if(quad[0] == row && quad[1] == col) {
	float botleftDistance = sqrt(pow(quad[0]-xLow,2)+pow(quad[1]-zLow,2));
	float toprightDistance = sqrt(pow(quad[0]-xHigh,2)+pow(quad[1]-zHigh,2));

	//printf("botleftdist: %f, topleftdist: %f\n", botleftDistance, toprightDistance);
	//float y = botleftDistance < toprightDistance ? yLowLow : yHighHigh;
	//float x = botleftDistance < toprightDistance ? xLow : xHigh;
	//float z = botleftDistance < toprightDistance ? zLow : zHigh; 
	float yPos;
	float cornerX;
	float cornerZ;
	float cornerY;
	if(botleftDistance < toprightDistance) {
	float a[] = { xLow, yLowLow, zLow };
	float b[] = { xLow, yLowHigh, zHigh };
	float c[] = { xHigh, yHighLow, zLow };
	cornerX = xLow;
	cornerZ = zLow;
	cornerY = yLowLow;
	}
	else {
	float a[] = { xLow, yLowHigh, zHigh };
	float b[] = { xHigh, yHighLow, zLow };
	float c[] = { xHigh, yHighHigh, zHigh};
	cornerX = xHigh;
	cornerZ = zHigh;
	cornerY = yHighHigh;
	}
	float ab[] = { b[0] - a[0], b[1] - a[1], b[2] - a[2] };
	float ac[] = { c[0] - a[0], c[1] - a[1], c[2] - a[2] };
	float normal[3];
	crossProduct(normal, ab, ac);
	float d = -(normal[0]*a[0]+normal[1]*a[1]+normal[2]*a[2]);
	yPos = -(d+normal[0]*(haxPos[0]-cornerX)+(haxPos[2]-cornerZ)*normal[2])/normal[1] + cornerY;
	

        //printf("grr: (%d, %f, %d): height: %d\n", row, yPos, col, (int)imagedata[(row*width + col)*3]);
  glPushMatrix();
  glTranslatef(haxPos[0], yPos+10, haxPos[2]);
  //glRotatef(-56, 1, 1, 0);
  glScalef(1, 1, 1);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, klingoff->vertexArray);
  glNormalPointer(GL_FLOAT, 0, klingoff->normalArray);
  glDrawElements(GL_TRIANGLES, klingoff->numIndices, GL_UNSIGNED_INT, klingoff->indexArray);
  glPopMatrix();
    }
}
}
  glPopAttrib();
	// YOU SHOULD WRITE THIS!
	//The height map image is available to you as a data array. Figure out a way to draw it in 3D.
	//Hint: The initial heightmap is extremely small. This is to help you. Use printf as needed.
	//Hint: Make a double "for" loop over the array, with appropriate scaling for height and width so your terrain matches the given quad.
	//Hint: Every quad is drawn as two triangles. Do each triangle separately, so you have full control.
}

// Light and materials
  GLfloat light_position[] = { 0.0, 300.0, 0.0, 1.0 }; // Directional from above
  GLfloat mat_shininess[] = { 50.0 };
  GLfloat mat_diffuseColor[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat mat_specularColor[] = { 1.0, 1.0, 1.0, 1.0 };

void display()
{
  // This function is called whenever it is time to render
  // a new frame; due to the idle()-function below, this
  // function will get called several times per second
  
  // Clear framebuffer & zbuffer
  glClearColor(0.3, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set current material  
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuseColor);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specularColor);
  

  // Place camera and light
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(100,50,50, 0, 0, 0, 0, 1, 0); // No manual control but better place from the start
  glLoadMatrixd(getCameraMatrix());
  glLightfv(GL_LIGHT0, GL_POSITION, light_position); // After camera matrix - camera independent!

  
  // Render the scene!
  renderGround();
  renderTerrain();
  
  // Swap front- and backbuffers
  glutSwapBuffers();
}

void init()
{
  int row, col;
  // An ordinary texture
  groundTexture = loadTexture("../textures/TropicalFoliage0025_1_S.jpg");
    klingoff = loadModel("../models/various/klingon.obj");
  // A heightmap image
  //imagedata = readppm("heightmaps/44-terrain.ppm", &height, &width);
//  imagedata = readppm("../../../heightmaps/fft-terrain.ppm", &height, &width);
  imagedata = readppm("heightmaps/fft-terrain-larger.ppm", &height, &width);

  // Print out image contents.
  // Warning! Only do this for small images or you will get incredible amounts of data!

  if (width < 16)
    for (row = 0; row < height; row++)
      for (col = 0; col < width; col++)
      {
        //printf("(%d, %d): R = %d, G = %d, B = %d\n", row, col, (int)imagedata[(row*width + col)*3], (int)imagedata[(row*width + col)*3+1], (int)imagedata[(row*width + col)*3+2]);
      }
  // Use the imagedata array for heights in the terrain!

// GL inits
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH); // Enable Gouraud shading
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, groundTexture);
}

void timer(int i)
{
	glutTimerFunc(20, timer, i);
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);

  // Configure GLUT:
  //  - framebuffer with RGB + Alpha values per pixel
  //  - Z-buffer
  //  - two sets of above mentioned buffers, so that
  //    doublebuffering is possible
  //
  // Initial window size 800x800
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Lab 4");

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90, 1, 0.01, 1000);
  glMatrixMode(GL_MODELVIEW);
  
  initHelperLibrary();
  init();

  // Register our display- and idle-functions with GLUT
  glutDisplayFunc(display);
  glutTimerFunc(20, timer, 0);

  // Enter GLUT's main loop; this function will never return
  glutMainLoop();

  return 0;
}

