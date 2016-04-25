#version 330

layout(location = 0) out vec4 bufferColor;
layout(location = 1) out vec4 outHeight;
layout(location = 2) out vec4 outNormal;

uniform sampler2D normalmap;
uniform sampler2DShadow shadowMapTex;
uniform vec3 light;
in  vec3  normalView;
in  vec3  tangentView;
in  vec3  normal;
in  vec4  height;
in  vec2  uvcoord;
in  vec3  eyeView;
in  vec4  project_position;

void main() {
  float et = 10.0;
  vec4 marron = vec4(0.345, 0.16, 0.0, 1.0);
  vec3 norm = normalize(normalView);
  vec3 t = normalize(tangentView);
  vec3 b = normalize(cross(norm,t));
  mat3 tbn = mat3(t,b,norm);

  vec4 texNormalMap = texture(normalmap,uvcoord);
  vec4 shadCoord = project_position*0.5+0.5;
  float bias = 0.015;

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

  // Shadow with percentage closer filtering
  // DECOMMENTER LA LIGNE SUIVANTE QUAND LE BLOC SHADOW MAP FONCTIONNE DANS VIEWER.CPP
  //bufferColor *= texture(shadowMapTex, vec3(shadCoord.xy, (shadCoord.z-bias)/shadCoord.w));

  outHeight = height;
  outNormal=vec4(newNorm, 1.0);
}
