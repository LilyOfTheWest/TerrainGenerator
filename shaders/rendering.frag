#version 330

layout(location = 0) out vec4 bufferColor;
layout(location = 1) out vec4 outHeight;
layout(location = 2) out vec4 outNormal;

uniform sampler2D normalmap;
uniform vec3 light;
in  vec3  normalView;
in  vec3  tangentView;
in  vec3  normal;
in  vec4  height;
in  vec2  uvcoord;
in  vec3  eyeView;

void main() {
  float et = 10.0;
  vec4 marron = vec4(0.345, 0.16, 0.0, 1.0);
  vec3 norm = normalize(normalView);
  vec3 t = normalize(tangentView);
  vec3 b = normalize(cross(norm,t));
  mat3 tbn = mat3(t,b,norm);

  vec4 texNormalMap = texture(normalmap,uvcoord);

//  vec3 newNorm = normalize(tbn*normal);

  vec3 normTex = texNormalMap.xyz*2.0-1.0;
  vec3 newNorm = tbn*normTex;

  vec3 n = normalize(newNorm);
  vec3 e = normalize(eyeView);
  vec3 l = normalize(light);
  l*=vec3(-1.0,1.0,1.0);

  float diff = max(dot(l,n),0.);
  float spec = pow(max(dot(reflect(l,n),e),0.0),et);

  bufferColor = (diff*marron);

  outHeight = height;
  outNormal=vec4(newNorm, 1.0);
}
