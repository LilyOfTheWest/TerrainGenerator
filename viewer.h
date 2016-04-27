#ifndef VIEWER_H
#define VIEWER_H

// GLEW lib: needs to be included first!!
#include <GL/glew.h> 

// OpenGL library 
#include <GL/gl.h>

// OpenGL Utility library
#include <GL/glu.h>

// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QGLFormat>
#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <stack>

#include "camera.h"
#include "shader.h"
#include "grid.h"

class Viewer : public QGLWidget {
 public:
  Viewer(
	 const QGLFormat &format=QGLFormat::defaultFormat());
  ~Viewer();
  
 protected :
  virtual void paintGL();
  virtual void initializeGL();
  virtual void resizeGL(int width,int height);
  virtual void keyPressEvent(QKeyEvent *ke);
  virtual void mousePressEvent(QMouseEvent *me);
  virtual void mouseMoveEvent(QMouseEvent *me);

 private:
  // OpenGL objects creation
  void createVAO();
  void deleteVAO();

  void createTextures();
  void deleteTextures();

  void createFBO();
  void deleteFBO();

  // drawing functions 
  void drawHeight(GLuint id);
  void drawTerrain(GLuint id);
  void drawShadow(GLuint id);
  void drawShadowMap(GLuint id);
  void drawRendu(GLuint id);

  void loadTexture(GLuint id,const char *filename);
  void createShaders();
  void deleteShaders();

  QTimer        *_timer;    // timer that controls the animation
  unsigned int   _currentshader; // current shader index

  Grid *_grid;
  Camera *_cam;    // the camera

  glm::vec3 _light; // light direction
  glm::vec3 _move;
  bool      _mode;  // camera motion or light motion
  bool      _showShadowMap; // Permet d'afficher la shadow map
  bool      _showHeightMap; // Permet d'afficher le champ de hauteur

  Shader *_shadowMapShader;
  Shader *_renderingShader;
  Shader *_noiseShader;
  Shader *_normalShader;
  //Shader *_postProcessShader;
  Shader *_showShadowMapShader;

  GLuint _vaoTerrain;
  GLuint _vaoQuad;
  GLuint _terrain[2];
  GLuint _quad;
  
  // fbo id and associated depth texture 
  GLuint  _fbo[3];
  GLuint _texHeight;
  GLuint _texNormal;
  GLuint _texRendu;
  GLuint _texDepth;
  GLuint _texColor;
  unsigned int _depthResol;
};

#endif // VIEWER_H
