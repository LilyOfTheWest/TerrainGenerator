#version 330

in  vec3  normalView;
in  vec3  tangentView;
in  vec3  normal;
in  vec4  height;
in  vec2  uvcoord;

out vec4 outHeight;
out vec4 outNormal;

uniform vec3 light;
uniform sampler2D normalmap;


void main() {
  vec3 n = normalize(normalView);
  vec3 t = normalize(tangentView);
  vec3 b = normalize(cross(n,t));
  mat3 tbn = mat3(t,b,n);

  vec3 newNorm = normalize(tbn*normal);

  outHeight = height;
  outNormal=vec4(newNorm, 1.0);
}
