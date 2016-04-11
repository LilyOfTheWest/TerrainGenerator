#include "viewer.h"

#include <math.h>
#include <iostream>
#include <QTime>

using namespace std;


Viewer::Viewer(const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _currentshader(0),
    _light(glm::vec3(0,0,1)),
    _mode(false),
    _showShadowMap(false),
    _depthResol(512) {

  setlocale(LC_ALL,"C");

  _grid = new Grid();
  _cam  = new Camera(sqrt(2*pow(_grid->size(),2))/2.0,glm::vec3(_grid->size()/2.0, _grid->size()/2.0, 0.0));

  _timer->setInterval(10);
  connect(_timer,SIGNAL(timeout()),this,SLOT(updateGL()));
}

Viewer::~Viewer() {
  delete _timer;
  delete _cam;
  delete _grid;

  // delete all GPU objects  
  deleteShaders();
  deleteTextures();
  deleteVAO(); 
  deleteFBO();
}

void Viewer::deleteTextures() {
  // delete loaded textures 
  //glDeleteTextures(2,_texColor);
  //glDeleteTextures(2,_texNormal);
}

void Viewer::deleteVAO() {
    // delete all VAO ids
  glDeleteBuffers(2,_terrain);
  glDeleteBuffers(1,&_quad);
  glDeleteVertexArrays(1,&_vaoTerrain);
  glDeleteVertexArrays(1,&_vaoQuad);
}


void Viewer::deleteFBO() {
  // delete all FBO Ids
  //glDeleteFramebuffers(1,&_fbo);
  //glDeleteTextures(1,&_texDepth);
}

void Viewer::createFBO() {
  // generate fbo and associated textures
    glGenFramebuffers(1, &_fbo[0]);
    glGenTextures(1,&_texHeight);
    glGenTextures(1, &_texNormal);

    glBindTexture(GL_TEXTURE_2D, _texHeight);
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,_grid->size(),_grid->size(),0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

  // attach textures to framebuffer object 
    glBindFramebuffer(GL_FRAMEBUFFER,_fbo[0]);
    glBindTexture(GL_TEXTURE_2D,_texHeight);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,_texHeight,0);

  // test if everything is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "Warning: FBO not complete!" << endl;

  // disable FBO
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}


void Viewer::loadTexture(GLuint id,const char *filename) {
  // load image 
  QImage image = QGLWidget::convertToGLFormat(QImage(filename));

  // activate texture 
  glBindTexture(GL_TEXTURE_2D,id);
  
  // set texture parameters 
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);
  
  // store texture in the GPU
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image.width(),image.height(),0,
  	       GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)image.bits());
  
  // generate mipmaps 
  glGenerateMipmap(GL_TEXTURE_2D);
}

void Viewer::createTextures() {

  // generate 3 texture ids 
//  glGenTextures(2,_texColor);
//  glGenTextures(2,_texNormal);

  // load all needed textures 
//  loadTexture(_texColor[0],"textures/color01.jpg");
//  loadTexture(_texNormal[0],"textures/normal01.jpg");
//  loadTexture(_texColor[1],"textures/color02.jpg");
//  loadTexture(_texNormal[1],"textures/normal02.jpg");
}

void Viewer::createVAO() {
  //the variable _grid should be an instance of Grid
  //the .h file should contain the following VAO/buffer ids
  //GLuint _vaoTerrain;
  //GLuint _vaoQuad;
  //GLuint _terrain[2];
  //GLuint _quad;

  const GLfloat quadData[] = {
    -1.0f,-1.0f,0.0f, 1.0f,-1.0f,0.0f, -1.0f,1.0f,0.0f, -1.0f,1.0f,0.0f, 1.0f,-1.0f,0.0f, 1.0f,1.0f,0.0f };

  glGenBuffers(2,_terrain);
  glGenBuffers(1,&_quad);
  glGenVertexArrays(1,&_vaoTerrain);
  glGenVertexArrays(1,&_vaoQuad);

  // create the VBO associated with the grid (the terrain)
  glBindVertexArray(_vaoTerrain);
  glBindBuffer(GL_ARRAY_BUFFER,_terrain[0]); // vertices
  glBufferData(GL_ARRAY_BUFFER,_grid->nbVertices()*3*sizeof(float),_grid->vertices(),GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_terrain[1]); // indices
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,_grid->nbFaces()*3*sizeof(int),_grid->faces(),GL_STATIC_DRAW);

  // create the VBO associated with the screen quad
  glBindVertexArray(_vaoQuad);
  glBindBuffer(GL_ARRAY_BUFFER,_quad); // vertices
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadData),quadData,GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);
}

void Viewer::createShaders() {
  _noiseShader = new Shader();
//  _normalShader = new Shader();
//  _postProcessShader = new Shader();
//  _shadowMapShader = new Shader(); // will create the shadow map
//  _renderingShader = new Shader(); // render the scene, use shadow map

  _noiseShader->load("shaders/noise.vert", "shaders/noise.frag");
//  _normalShader->load("shaders/normal.vert", "shaders/normal.frag");
//  _postProcessShader->load("shaders/post-process.vert", "shaders/post-process.frag");
//  _shadowMapShader->load("shaders/shadow-map.vert","shaders/shadow-map.frag");
//  _renderingShader->load("shaders/rendering.vert","shaders/rendering.frag");
}


void Viewer::deleteShaders() {
  delete _noiseShader;  _noiseShader = NULL;
//  delete _normalShader; _normalShader = NULL;
//  delete _postProcessShader;    _postProcessShader = NULL;
//  delete _shadowMapShader; _shadowMapShader = NULL;
//  delete _renderingShader; _renderingShader = NULL;
}

void Viewer::drawSceneFromCamera(GLuint id) {
  // mdv matrix from the light point of view 
  const float size = sqrt(2*pow(_grid->size(),2))/2.0;
  glm::vec3 l   = glm::transpose(_cam->normalMatrix())*_light;
  glm::mat4 p   = glm::ortho<float>(-size,size,-size,size,-size,2*size);
  glm::mat4 v   = glm::lookAt(l, glm::vec3(0,0,0), glm::vec3(0,1,0));
  glm::mat4 m   = glm::mat4(1.0);
  glm::mat4 mv  = v*m;

  const glm::mat4 mvpDepth = p*mv;
  glUniformMatrix4fv(glGetUniformLocation(id,"mvpDepthMat"),1,GL_FALSE,&mvpDepth[0][0]);

  // send uniform variables 
  glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(_cam->projMatrix()[0][0]));
  glUniformMatrix3fv(glGetUniformLocation(id,"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));
  glUniform3fv(glGetUniformLocation(id,"light"),1,&(_light[0]));

  const glm::mat4 mdv = _cam->mdvMatrix();
  glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(mdv[0][0]));

  // disable VAO
  glBindVertexArray(0);
}

void Viewer::drawSceneFromLight(GLuint id) {
  // mdv matrix from the light point of view
  const float size = sqrt(2*pow(_grid->size(),2))/2.0;
  glm::vec3 l   = glm::transpose(_cam->normalMatrix())*_light;
  glm::mat4 p   = glm::ortho<float>(-size,size,-size,size,-size,2*size);
  glm::mat4 v   = glm::lookAt(l, glm::vec3(0,0,0), glm::vec3(0,1,0));
  glm::mat4 m   = glm::mat4(1.0);
  glm::mat4 mv  = v*m;

  const glm::mat4 mvpDepth = p*mv;
  glUniformMatrix4fv(glGetUniformLocation(id,"mvpDepthMat"),1,GL_FALSE,&mvpDepth[0][0]);
  // disable VAO
  glBindVertexArray(0);
}

void Viewer::drawShadowMap(GLuint id) {
  // send depth texture 
//  glActiveTexture(GL_TEXTURE0);
//  glBindTexture(GL_TEXTURE_2D,_texDepth);
//  glUniform1i(glGetUniformLocation(id,"shadowmap"),0);

  // draw the quad 
//  glBindVertexArray(_vaoQuad);
//  glDrawArrays(GL_TRIANGLES,0,6);

  // disable VAO
  glBindVertexArray(0);
}

void Viewer::paintGL() {
  // *** TODO: compute the shadow map here ***
  //glBindFramebuffer(GL_FRAMEBUFFER,_fbo[0]);

  //glDrawBuffer(GL_NONE);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(_noiseShader->id());

  glViewport(0, 0, _grid->size(),_grid->size());
  glBindVertexArray(_vaoQuad);
    glDrawArrays(GL_TRIANGLES,0,6);

    glBindVertexArray(0);
  //drawSceneFromLight(_noiseShader->id());

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // back to the screen viewport
//  glViewport(0,0,width(),height());

  // activate the rendering shader 
//  glUseProgram(_renderingShader->id());

  // clear buffers 
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw the scene and apply the shadows 
//  drawSceneFromCamera(_renderingShader->id());

  // disable shader 
  glUseProgram(0);
}

void Viewer::resizeGL(int width,int height) {
  _cam->initialize(width,height,false);
  glViewport(0,0,width,height);
  updateGL();
}

void Viewer::mousePressEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(me->button()==Qt::LeftButton) {
    _cam->initRotation(p);
    _mode = false;
  } else if(me->button()==Qt::MidButton) {
    _cam->initMoveZ(p);
    _mode = false;
  } else if(me->button()==Qt::RightButton) {
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
    _mode = true;
  } 

  updateGL();
}

void Viewer::mouseMoveEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));
 
  if(_mode) {
    // light mode
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
  } else {
    // camera mode
    _cam->move(p);
  }

  updateGL();
}

void Viewer::keyPressEvent(QKeyEvent *ke) {
  
  // key a: play/stop animation
  if(ke->key()==Qt::Key_A) {
    if(_timer->isActive()) 
      _timer->stop();
    else 
      _timer->start();
  }

  // key i: init camera
  if(ke->key()==Qt::Key_I) {
    _cam->initialize(width(),height(),true);
  }
  
  // key f: compute FPS
  if(ke->key()==Qt::Key_F) {
    int elapsed;
    QTime timer;
    timer.start();
    unsigned int nb = 500;
    for(unsigned int i=0;i<nb;++i) {
      paintGL();
    }
    elapsed = timer.elapsed();
    double t = (double)nb/((double)elapsed);
    cout << "FPS : " << t*1000.0 << endl;
  }

  // key r: reload shaders 
  if(ke->key()==Qt::Key_R) {
    _noiseShader->reload("shaders/noise.vert","shaders/noise.frag");
    _normalShader->reload("shaders/normal.vert","shaders/normal.frag");
    _postProcessShader->reload("shaders/post-process.vert","shaders/post-process.frag");
    _shadowMapShader->reload("shaders/shadow-map.vert","shaders/shadow-map.frag");
    _renderingShader->reload("shaders/rendering.vert","shaders/rendering.vert");
  }

  // key S: show the shadow map 
  if(ke->key()==Qt::Key_S) {
    _showShadowMap = !_showShadowMap;
  }

  updateGL();
}

void Viewer::initializeGL() {
  // make this window the current one
  makeCurrent();

  // init and chack glew
  if(glewInit()!=GLEW_OK) {
    cerr << "Warning: glewInit failed!" << endl;
  }

  if(!GLEW_ARB_vertex_program   ||
     !GLEW_ARB_fragment_program ||
     !GLEW_ARB_texture_float    ||
     !GLEW_ARB_draw_buffers     ||
     !GLEW_ARB_framebuffer_object) {
    cerr << "Warning: Shaders not supported!" << endl;
  }

  // init OpenGL settings
  glClearColor(0.0,0.0,0.0,1.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glViewport(0,0,width(),height());

  // initialize camera
  _cam->initialize(width(),height(),true);

  // load shader files
  createShaders();

  // init VAO
  createVAO();
  
  // init textures 
  createTextures();
  
  // create/init FBO
  createFBO();

  // starts the timer 
  _timer->start();
}

