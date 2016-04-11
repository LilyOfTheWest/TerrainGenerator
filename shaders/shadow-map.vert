#version 330

// input attributes 
layout(location = 0) in vec3 position; 

// input uniforms
uniform mat4 mvpMat;

void main() {
  // Update vertex position according to the (light space) modelviewprojection matrix  *** 
  gl_Position =  mvpMat*vec4(position,1);
}
