#GLEW_PATH = ../../ext/glew-1.9.0
GLM_PATH  = ../../ext/glm-0.9.4.1

TEMPLATE  = app
TARGET    = tp08

#LIBS     += -Wl,-rpath $${GLEW_PATH}/lib -L$${GLEW_PATH}/lib
LIBS     += -lGLEW -lGL -lGLU -lm
INCLUDEPATH  +=  $${GLM_PATH}

SOURCES   = shader.cpp trackball.cpp camera.cpp main.cpp viewer.cpp grid.cpp
HEADERS   = shader.h trackball.h camera.h viewer.h grid.h

CONFIG   += qt opengl warn_on thread uic4 release
QT       *= xml opengl core
