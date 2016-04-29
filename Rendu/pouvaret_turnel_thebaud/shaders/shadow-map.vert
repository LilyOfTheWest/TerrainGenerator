#version 330

// input attributes
layout(location = 0) in vec3 position;

// input uniforms
uniform mat4 mvpMat;
uniform sampler2D heightMap;

void main() {
  // Update vertex position according to the (light space) modelviewprojection matrix  ***
  vec2 coord = position.xy * 0.5 + vec2(0.5);
  vec3 normHeight = texture(heightMap, coord).xyz*0.05;
  vec3 newPos = position + vec3(0.0,0.0,1.0) * normHeight;

  gl_Position =  mvpMat*vec4(newPos,1.0);
  //gl_Position =  mvpMat*vec4(position,1);
}
