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
    _move(glm::vec3(0,0,0)),
    _mode(false),
    _showShadowMap(false),
    _showNormalMap(false),
    _showHeightMap(false),
    _showFog(true),
    _showShadow(true),
    _showLight(true),
    _useColorMap(true),
    _depthResol(512) {

  setlocale(LC_ALL,"C");

  _grid = new Grid();
  //_depthResol = _grid->size();
  //_cam = new Camera(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
  _cam = _cam = new Camera(/*_grid->radius()*/0.7f, glm::vec3(0.0f, 0.0f, 0.0f));
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
  glDeleteFramebuffers(1,&_fbo[0]);
  glDeleteFramebuffers(1, &_fbo[1]);
  glDeleteFramebuffers(1, &_fbo[2]);
  glDeleteTextures(1,&_texHeight);
  glDeleteTextures(1, &_texNormal);
  glDeleteTextures(1, &_texRendu);
  glDeleteTextures(1, &_texDepth);
}

void Viewer::createFBO() {
  // generate fbo and associated textures
    // Fbo de height map et normal map
    glGenFramebuffers(1, &_fbo[0]);
    // Fbo de rendu
    glGenFramebuffers(1, &_fbo[1]);
    // Fbo de shadow map
    glGenFramebuffers(1, &_fbo[2]);
    glGenTextures(1,&_texHeight);
    glGenTextures(1, &_texNormal);
    glGenTextures(1, &_texRendu);
    glGenTextures(1, &_texDepth);

    // Texture de la hauteur
    glBindTexture(GL_TEXTURE_2D, _texHeight);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,_grid->size(),_grid->size(),0,GL_RGBA,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    // Texture des normales
    glBindTexture(GL_TEXTURE_2D, _texNormal);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,_grid->size(),_grid->size(),0,GL_RGBA,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Texture du rendu
    glBindTexture(GL_TEXTURE_2D, _texRendu);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,width(),height(),0,GL_RGBA,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Texture de profondeur
    glBindTexture(GL_TEXTURE_2D,_texDepth);
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,_depthResol,_depthResol,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

  // attach textures to framebuffer object 
    glBindFramebuffer(GL_FRAMEBUFFER,_fbo[0]);
    glBindTexture(GL_TEXTURE_2D,_texHeight);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_texHeight,0);

    glBindTexture(GL_TEXTURE_2D,_texNormal);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,_texNormal,0);

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo[1]);
    glBindTexture(GL_TEXTURE_2D,_texRendu);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_texRendu,0);

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo[2]);
    glBindTexture(GL_TEXTURE_2D, _texDepth);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _texDepth,0);

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
  if(id == _texColor[0])
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  else glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  
  // store texture in the GPU
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,image.width(),image.height(),0,
  	       GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)image.bits());
  
  // generate mipmaps 
  glGenerateMipmap(GL_TEXTURE_2D);
}

void Viewer::createTextures() {
    _nbTextures = 6;
    _texColor  = new GLuint[_nbTextures];

    /**********************************
                2D Version
    **********************************/

    // enable the use of 1D textures
    glEnable(GL_TEXTURE_2D);

    // create textures on the GPU
    glGenTextures(_nbTextures,_texColor);

    // load images (CPU side)
    loadTexture(_texColor[0], "textures/2DcolorTexv2.png");
    loadTexture(_texColor[1], "textures/rock.jpg");
    loadTexture(_texColor[2], "textures/sand.jpg");
    loadTexture(_texColor[3], "textures/water.jpg");
    loadTexture(_texColor[4], "textures/snow.jpg");
    loadTexture(_texColor[5], "textures/grass.jpg");

//    // ------ activate this texture : colorImg
//    glBindTexture(GL_TEXTURE_2D,_texColor);

//    // texture sampling/filtering operation.
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//    // transfer data from CPU to GPU memory
//    glTexImage2D(
//        GL_TEXTURE_2D,
//        0,
//        GL_RGBA32F,
//        colorImg.width(),
//        colorImg.height(),
//        0,
//        GL_RGBA,
//        GL_UNSIGNED_BYTE,
//        (const GLvoid *)colorImg.bits()
//    );

    // generate mipmaps
//    glGenerateMipmap(GL_TEXTURE_2D);

//    /**********************************
//                1D Version
//    **********************************/

//    // enable the use of 1D textures
//    glEnable(GL_TEXTURE_1D);

//    // create one texture on the GPU
//    glGenTextures(1,&_texColor);

//    // load an image (CPU side)
//    colorImg = QGLWidget::convertToGLFormat(QImage("textures/1DcolorTex.png"));

//    // ------ activate this texture : colorImg
//    glBindTexture(GL_TEXTURE_1D,_texColor);

//    // texture sampling/filtering operation.
//    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

//    // transfer data from CPU to GPU memory
//    glTexImage1D(
//        GL_TEXTURE_1D,                  // Specifies the target texture. Must be GL_TEXTURE_1D or GL_PROXY_TEXTURE_1D.
//        0,                              // Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
//        GL_RGBA32F,
//        colorImg.width(),               // size of the image (just width because 1D)
//        0,                              // border: This value must be 0
//        GL_RGBA,
//        GL_UNSIGNED_BYTE,               // TODO : GL_FLOAT ?
//        (const GLvoid *)colorImg.bits() // data
//    );

//    // generate mipmaps
//    glGenerateMipmap(GL_TEXTURE_1D);
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
  _normalShader = new Shader();
//  _postProcessShader = new Shader();
  _shadowMapShader = new Shader(); // will create the shadow map
  _showShadowMapShader = new Shader();
  _showHeightMapShader = new Shader();
  _showNormalMapShader = new Shader();
  _renderingShader = new Shader(); // render the scene, use shadow map

  _noiseShader->load("shaders/noise.vert", "shaders/noise.frag");
  _normalShader->load("shaders/normal.vert", "shaders/normal.frag");
//  _postProcessShader->load("shaders/post-process.vert", "shaders/post-process.frag");
  _shadowMapShader->load("shaders/shadow-map.vert","shaders/shadow-map.frag");
  _showShadowMapShader->load("shaders/show-shadow-map.vert","shaders/show-shadow-map.frag");
  _showHeightMapShader->load("shaders/show-height-map.vert","shaders/show-height-map.frag");
  _showNormalMapShader->load("shaders/show-normal-map.vert","shaders/show-normal-map.frag");
  _renderingShader->load("shaders/rendering.vert","shaders/rendering.frag");
}


void Viewer::deleteShaders() {
  delete _noiseShader;  _noiseShader = NULL;
  delete _normalShader; _normalShader = NULL;
//  delete _postProcessShader;    _postProcessShader = NULL;
  delete _shadowMapShader; _shadowMapShader = NULL;
  delete _renderingShader; _renderingShader = NULL;
  delete _showShadowMapShader; _showShadowMapShader = NULL;
  delete _showHeightMapShader; _showHeightMapShader = NULL;
  delete _showNormalMapShader; _showNormalMapShader = NULL;
}

void Viewer::drawTerrain(GLuint id) {
  // mdv matrix from the light point of view
//  const float size = sqrt(2*pow(_grid->size(),2))/2.0;
//  glm::vec3 l   = glm::transpose(_cam->normalMatrix())*_light;
//  glm::mat4 p   = glm::ortho<float>(-size,size,-size,size,-size,2*size);
//  glm::mat4 v   = glm::lookAt(l, glm::vec3(0,0,0), glm::vec3(0,1,0));
//  glm::mat4 m   = glm::mat4(1.0);
//  glm::mat4 mv  = v*m;

//  const glm::mat4 mvpDepth = p*mv;
//  glUniformMatrix4fv(glGetUniformLocation(id,"mvpDepthMat"),1,GL_FALSE,&mvpDepth[0][0]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texHeight);
  glUniform1i(glGetUniformLocation(id,"heightmap"),0);

  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);
  glBindVertexArray(0);

  // disable VAO
  glBindVertexArray(0);
}

void Viewer::drawHeight(GLuint id) {

  // envoi du vecteur de mouvement
  glUniform3fv(glGetUniformLocation(id, "move"),1, &(_move[0]));
  glViewport(0, 0, _grid->size(),_grid->size());
  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);
  glBindVertexArray(0);

  // disable VAO
  glBindVertexArray(0);
}

void Viewer::drawRendu(GLuint id){
     //mdv matrix from the light point of view
    //const float size = sqrt(2*pow(_grid->size(),2))/2.0;
    const float size=_grid->radius();
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

    // Envoi des bool sur le brouillard, les ombres et la lumière
    glUniform1i(glGetUniformLocation(id,"showFog"), _showFog);
    glUniform1i(glGetUniformLocation(id,"showLight"), _showLight);
    glUniform1i(glGetUniformLocation(id,"showShadow"), _showShadow);
    glUniform1i(glGetUniformLocation(id,"useColorMap"), _useColorMap);

    // send height texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,_texHeight);
    glUniform1i(glGetUniformLocation(id,"heightmap"),0);

    // send normal texture
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D,_texNormal);
    glUniform1i(glGetUniformLocation(id,"normalmap"),1);

    // send shadow texture
    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D, _texDepth);
    glUniform1i(glGetUniformLocation(id, "shadowMapTex"), 2);

    // send color texture
    //2D colormap
    glActiveTexture(GL_TEXTURE0+3);
    glBindTexture(GL_TEXTURE_2D, _texColor[0]);
    glUniform1i(glGetUniformLocation(id,"colormap"),3);

    // rock
    glActiveTexture(GL_TEXTURE0+4);
    glBindTexture(GL_TEXTURE_2D, _texColor[1]);
    glUniform1i(glGetUniformLocation(id,"rock"),4);

    // sand
    glActiveTexture(GL_TEXTURE0+5);
    glBindTexture(GL_TEXTURE_2D, _texColor[2]);
    glUniform1i(glGetUniformLocation(id,"sand"),5);

    // water
    glActiveTexture(GL_TEXTURE0+6);
    glBindTexture(GL_TEXTURE_2D, _texColor[3]);
    glUniform1i(glGetUniformLocation(id,"water"),6);

    // snow
    glActiveTexture(GL_TEXTURE0+7);
    glBindTexture(GL_TEXTURE_2D, _texColor[4]);
    glUniform1i(glGetUniformLocation(id,"snow"),7);

    // grass
    glActiveTexture(GL_TEXTURE0+8);
    glBindTexture(GL_TEXTURE_2D, _texColor[5]);
    glUniform1i(glGetUniformLocation(id,"grass"),8);
//    //1D
//    glActiveTexture(GL_TEXTURE0+3);
//    glBindTexture(GL_TEXTURE_1D, _texColor);
//    glUniform1i(glGetUniformLocation(id,"colormap"),3);

    glBindVertexArray(_vaoTerrain);
    glDrawElements(GL_TRIANGLES,3*_grid->nbFaces(),GL_UNSIGNED_INT,(void *)0);
    glBindVertexArray(0);
}

void Viewer::drawShadow(GLuint id) {
    // mdv matrix from the light point of view
      //const float size = sqrt(2);
      const float size = _grid->radius();
      glm::vec3 l   = glm::transpose(_cam->normalMatrix())*_light;
      glm::mat4 p   = glm::ortho<float>(-size,size,-size,size,-size,2*size);
      glm::mat4 v   = glm::lookAt(l, glm::vec3(0,0,0), glm::vec3(0,1,0));
      glm::mat4 m   = glm::mat4(1.0);
      glm::mat4 mv  = v*m;

      const glm::mat4 mvpDepth = p*mv;
      glUniformMatrix4fv(glGetUniformLocation(id,"mvpMat"),1,GL_FALSE,&mvpDepth[0][0]);

      // On envoit notre information sur la hauteur
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,_texHeight);
      glUniform1i(glGetUniformLocation(id,"heightMap"),0);
      glBindVertexArray(_vaoTerrain);

      glDrawElements(GL_TRIANGLES,3*_grid->nbFaces(),GL_UNSIGNED_INT,(void *)0);
      glBindVertexArray(0);
}

void Viewer::drawShadowMap(GLuint id) {
  // send depth texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texDepth);
  glUniform1i(glGetUniformLocation(id,"shadowmap"),0);

  // draw the quad
  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);

  // disable VAO
  glBindVertexArray(0);
}

void Viewer::drawHeightMap(GLuint id) {
  // send depth texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texHeight);
  glUniform1i(glGetUniformLocation(id,"heightmap"),0);

  // draw the quad
  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);

  // disable VAO
  glBindVertexArray(0);
}

void Viewer::drawNormalMap(GLuint id) {
  // send depth texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texNormal);
  glUniform1i(glGetUniformLocation(id,"normalmap"),0);

  // draw the quad
  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);

  // disable VAO
  glBindVertexArray(0);
}

void Viewer::paintGL() {
  /* Champ de hauteur */
  // on bind le premier fbo

  glBindFramebuffer(GL_FRAMEBUFFER,_fbo[0]);

  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  // On dessine dans le premier buffer
  glDrawBuffer(GL_COLOR_ATTACHMENT0);

  // Taille du viewport = taille de la heightmap
  glViewport(0, 0, _grid->size(),_grid->size());

  // On clear le buffer de couleur
  glClear(GL_COLOR_BUFFER_BIT);

  // On active le shader de bruit
  glUseProgram(_noiseShader->id());

  // On dessine la texture de bruit
  drawHeight(_noiseShader->id());

  glDrawBuffer(GL_COLOR_ATTACHMENT1);

  // On active le shader des normales
  glUseProgram(_normalShader->id());

  glClear(GL_COLOR_BUFFER_BIT);

  drawTerrain(_normalShader->id());

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  glBindFramebuffer(GL_FRAMEBUFFER, _fbo[2]);

  glDrawBuffer(GL_NONE);
  glClear(GL_DEPTH_BUFFER_BIT);
  glUseProgram(_shadowMapShader->id());
  glViewport(0,0,_depthResol, _depthResol);
  drawShadow(_shadowMapShader->id());

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // On revient au viewport de l'écran
  glViewport(0, 0, width(),height());
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_renderingShader->id());

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(1.0, 1.0, 1.0, 1);// white color same as fog color
  glClearDepth(1);
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  // On dessine le rendu
  drawRendu(_renderingShader->id());

  // Pour afficher la heightmap
  if(_showHeightMap) {
      glDisable(GL_DEPTH_TEST);
      glDepthMask(GL_FALSE);

      // clear buffers
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      // activate the test shader
      glUseProgram(_showHeightMapShader->id());

      // display the shadow map
      drawHeightMap(_showHeightMapShader->id());
    }

  // Pour afficher la normalmap
  if(_showNormalMap) {
      // activate the test shader
      glUseProgram(_showNormalMapShader->id());

      // clear buffers
      glClear(GL_COLOR_BUFFER_BIT);

      // display the shadow map
      drawNormalMap(_showNormalMapShader->id());
    }

  // Pour afficher la shadowmap
  if(_showShadowMap) {
      glDisable(GL_DEPTH_TEST);
      glDepthMask(GL_FALSE);
      // activate the test shader
      glUseProgram(_showShadowMapShader->id());

      // clear buffers
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // display the shadow map
      drawShadowMap(_showShadowMapShader->id());

      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);
    }

  // On désactive les shaders
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
    const float step = 0.025;
    // On avance
    if(ke->key()==Qt::Key_Z){
    glm::vec2 v = glm::vec2(glm::transpose(_cam->normalMatrix())*glm::vec3(0,0,-1))*step;
    if(v[0]!=0.0 && v[1]!=0.0) v = glm::normalize(v)*step;
        else v = glm::vec2(0,1)*step;
        _move[0] += v[0];
        _move[1] += v[1];
    }

    // On recule
  if(ke->key()==Qt::Key_S) {
    glm::vec2 v = glm::vec2(glm::transpose(_cam->normalMatrix())*glm::vec3(0,0,-1))*step;
    if(v[0]!=0.0 && v[1]!=0.0) v = glm::normalize(v)*step;
    else v = glm::vec2(0,1)*step;
    _move[0] -= v[0];
    _move[1] -= v[1];
  }

  // Autres mouvements
  if(ke->key()==Qt::Key_Q) {
    _move[2] += 20*step;
  }

  if(ke->key()==Qt::Key_D) {
    _move[2] -= 20*step;
  }

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
  }

  // key X: show height map
    if(ke->key()==Qt::Key_X) {
      _showHeightMap = !_showHeightMap;
      if(_showNormalMap) _showNormalMap = !_showNormalMap;
      if(_showShadowMap) _showShadowMap = !_showShadowMap;
    }

    // key W: show the shadow map
    if(ke->key()==Qt::Key_W) {
      _showShadowMap = !_showShadowMap;
      if(_showNormalMap) _showNormalMap = !_showNormalMap;
      if(_showHeightMap) _showHeightMap = !_showHeightMap;
    }
    // key C: show the normal map
    if(ke->key()==Qt::Key_C) {
      _showNormalMap = !_showNormalMap;
      if(_showHeightMap) _showHeightMap = !_showHeightMap;
      if(_showShadowMap) _showShadowMap = !_showShadowMap;
    }

    // Key B : afficher le brouillard
    if(ke->key()==Qt::Key_B){
        _showFog = !_showFog;
        // Envoi du booléen concernant le brouillard
    }

    // Key L : afficher la lumière
    if(ke->key()==Qt::Key_L){
        _showLight = !_showLight;
    }

    // Key G : afficher les ombres
    if(ke->key()==Qt::Key_G){
        _showShadow = !_showShadow;
    }

    // Key T : utiliser la colormap ou les textures individuelles
    if(ke->key()==Qt::Key_T){
        _useColorMap = !_useColorMap;
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

