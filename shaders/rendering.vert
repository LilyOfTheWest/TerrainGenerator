#version 330

// input attributes (world space) 
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 coord;

// input uniforms 
uniform mat4 mdvMat;      // modelview matrix 
uniform mat4 projMat;     // projection matrix
uniform mat3 normalMat;   // normal matrix
uniform mat4 mvpDepthMat; // mvp depth matrix

// output vectors (camera space)
out vec3 normalView;
out vec3 tangentView;
out vec3 eyeView;
out vec2 uvcoord;
out vec4 project_position;

void main() {
  gl_Position = projMat*mdvMat*vec4(position,1.0);
  normalView  = normalize(normalMat*normal);
  tangentView = normalize(normalMat*tangent);
  eyeView     = normalize((mdvMat*vec4(position,1.0)).xyz);
  uvcoord     = coord*5.0;

  // *** TODO: project position in light space (and send it to the fragment) to be able to compare depths ***
  project_position = mvpDepthMat*vec4(position,1.0);
}

