

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>

#ifndef GL_VERSION_2_0
#error This program needs OpenGL 2.0 libraries to compile
#endif

#include "../helpers.h"
#include "../shaderutils.h"
#include "water.h"


Model* screen_model = 0;

GLuint screen_shaderProgram = 0;
GLuint shaderTimeLocation;
GLuint resLocation;
GLuint screen_noop = 0;
int shaderTextureLocation, shaderTextureLocation2;
GLuint res[2] = { 0, 0 };
int textureId, textureId2;

// make_buffer courtesy of http://duriansoftware.com/joe/An-intro-to-modern-OpenGL.-Chapter-2.1:-Buffers-and-Textures.html
static struct {
    GLuint vertex_buffer, element_buffer;
    GLuint textures[2];
    GLuint vertex_shader, fragment_shader, program;
    
    struct {
        GLint timer;
        GLint textures[2];
    } uniforms;

    struct {
        GLint position;
    } attributes;

    GLfloat timer;
} g_resources;

void screen_opts(int width, int height) {
  res[0] = width;
  res[1] = height;
}

static GLuint make_buffer(
  GLenum target,
  const void *buffer_data,
  GLsizei buffer_size) {

  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(target, buffer);
  glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);
  return buffer;
}

static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f,
    -1.0f, 1.0f, 0.0f, 1.0f,
     1.0f, 1.0f, 0.0f, 1.0f
};
static const GLushort g_element_buffer_data[] = { 0, 1, 2, 3 };

void screen_init()
{
  // Place one-time initialization code here
  initHelperLibrary();
  // load model
  screen_model = loadModel("../models/various/teapot.obj"); 
  setModelCenter(screen_model, 0.f, 0.f, 0.f);
  setModelRadius(screen_model, 0.3f);
  textureId = loadTexture("textures/texture_3.jpg"); 
  textureId2 = loadTexture("textures/stars_ss_dx_org.jpg"); 

  // Create vertex shader
  GLuint vertexShader = createShaderFromFile(GL_VERTEX_SHADER, "scenes/screen.vs");
  if (!vertexShader)
    printf("couldn't create shader program!");

  // Create fragment shader
  GLuint fragmentShader = createShaderFromFile(GL_FRAGMENT_SHADER, "scenes/screen.fs");
  if (!fragmentShader)
    printf("couldn't create shader program!");

  // Create shader program
  screen_shaderProgram = createShaderProgram(vertexShader, fragmentShader);
  if (!screen_shaderProgram)
    printf("couldn't create shader program!");

  screen_noop = createShaderProgram(createShaderFromFile(GL_VERTEX_SHADER, "scenes/noop.vs"), 
					createShaderFromFile(GL_FRAGMENT_SHADER, "scenes/noop.fs"));

    g_resources.vertex_buffer = make_buffer(
        GL_ARRAY_BUFFER,
        g_vertex_buffer_data,
        sizeof(g_vertex_buffer_data)
    );

    g_resources.element_buffer = make_buffer(
        GL_ELEMENT_ARRAY_BUFFER,
        g_element_buffer_data,
        sizeof(g_element_buffer_data)
    );

  //buffer = glCreateBuffer();
  //glBindBuffer(GL_ARRAY_BUFFER, buffer);
  //glBufferData(GL_ARRAY_BUFFER, { - 1.0, - 1.0, 1.0, - 1.0, - 1.0, 1.0, 1.0, - 1.0, 1.0, 1.0, - 1.0, 1.0 }, GL_STATIC_DRAW);

  shaderTimeLocation = getUniformLocation(screen_shaderProgram, "time");
  resLocation = getUniformLocation(screen_shaderProgram, "resolution");
  shaderTextureLocation = getUniformLocation(screen_shaderProgram, "texture");
  shaderTextureLocation2 = getUniformLocation(screen_shaderProgram, "texture2");
}

void screen_load() {
  printf("whaaaa\n");
}


void screen_disp()
{
  // This function is called whenever it is time to render
  //  a new frame; due to the idle()-function below, this
  //  function will get called several times per second

  // Clear framebuffer & zbuffer
  glClearColor(0.3, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Setup projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90, 1, 0.01, 100);

  // Setup object matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //glMultMatrixd(getObjectMatrix());
  gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -3.0f, 0.0f, 1.0f, 0.0f);

  // Enable Z-buffering
  glEnable(GL_DEPTH_TEST);

  // Enable backface culling
  glDisable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

  // Activate shader program
  glUseProgram(screen_shaderProgram);
  glUniform1f(shaderTimeLocation, getElapsedTime());
  glUniform2f(resLocation, res[0], res[1]);
  //RIGHT PLACE? O_o
  glUniform1i(shaderTextureLocation, 0);
  glUniform1i(shaderTextureLocation2, 1);

  //fix textures typ
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureId);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textureId2);
  
  // Draw mesh using array-based API



/*  glBegin(GL_POLYGON);
  glColor3f(1, 1, 1);
  glVertex3f(-0.5, 0.5, 0.0);
  glColor3f(0, 1, 1);
  glVertex3f(-0.5,-0.5, 0.0);
  glColor3f(1, 1, 0.3);
  glVertex3f( 0.5,-0.5, 0.0);
  glColor3f(1, 0, 1);
  glVertex3f( 1, 0, 0.0);
  glColor3f(0, 0.5, 1);
  glVertex3f( 0.5, 0.5, 0.0);
  glEnd();
*/
  // Deactivate shader program
  glUseProgram(screen_noop);

  glPushMatrix();
  glTranslatef(0.0, -20.0, 0.0);
  water_renderGround();
  glPopMatrix();

  glUseProgram(0);

  glPopClientAttrib();
  glPopAttrib();

  // Swap front- and backbuffers
  glutSwapBuffers();
}


void b() {
    glRotatef(getElapsedTime(), 1.0, 0.0, 0.0);
    glBindBuffer(GL_ARRAY_BUFFER, g_resources.vertex_buffer);
    glVertexAttribPointer(
        g_resources.attributes.position, /* attribute */
        4, /* size */
        GL_FLOAT, /* type */
        GL_FALSE, /* normalized? */
        sizeof(GLfloat)*4, /* stride */
        (void*)0 /* array buffer offset */
    );
    glEnableVertexAttribArray(g_resources.attributes.position);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_resources.element_buffer);
    glDrawElements(
        GL_TRIANGLE_STRIP, /* mode */
        4, /* count */
        GL_UNSIGNED_SHORT, /* type */
        (void*)0 /* element array buffer offset */
    );
    glDisableVertexAttribArray(g_resources.attributes.position);
}
