#version 330

// input attributes (world space)
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 faces;

// input uniforms
uniform mat4 mdvMat;      // modelview matrix
uniform mat4 projMat;     // projection matrix
uniform mat3 normalMat;   // normal matrix

uniform sampler2D heightmap;
// output vectors (camera space)
out vec3 normalView;
//out vec3 tangentView;
out vec2 uvcoord;
out vec3 tangentView;
out vec3 normal;
out vec4 height;
out vec3 eyeView;

void main() {
  uvcoord     = position.xy*0.5+vec2(0.5);

  // float ps = 1./512;
  // float hh = texture(heightmap, uvcoord + vec2(0, ps)).x;
  // float hb = texture(heightmap, uvcoord - vec2(0, ps)).x;
  // float hd = texture(heightmap, uvcoord + vec2(ps, 0)).x;
  // float hg = texture(heightmap, uvcoord - vec2(ps, 0)).x;
  //
  // // gradient
  // float c = 2.0;
  // float gx = (hd - hg) * c;
  // float gy = (hh - hd) * c;
  //
  // vec3 v1 = vec3(1, 0, gx);
  // vec3 v2 = vec3(0, 1, gy);
  // vec3 normal = cross(v1, v2);

  vec3 normal = vec3(0.0,0.0,1.0);
  vec4 texHeightmap = texture(heightmap,uvcoord);
  vec3 normHeight = texHeightmap.xyz*0.05;
  vec3 newPos = position+vec3(0.0,0.0,1.0)*normHeight.xyz;
  vec3 tangent = vec3(1.0,0.0,0.0);

  normalView = normalize(normalMat*normal);
  tangentView = normalize(normalMat*tangent);
  height = vec4(newPos,1.0);
  eyeView     = normalize((mdvMat*vec4(position,1.0)).xyz);
  gl_Position = projMat*mdvMat*height;
}
