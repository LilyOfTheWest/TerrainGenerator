#version 330

in  vec3  normalView;
//in  vec3  tangentView;
in  vec3  eyeView;
in  vec2  uvcoord;


out vec4 bufferColor;

uniform vec3 light;
uniform sampler2D normalmap;


void main() {
  float et      = 10.0;
  vec4 texNormal = texture(normalmap,uvcoord);

  // On re normalise
  //vec3 norm = normalize(normalView);
  //vec3 tangent = normalize(tangentView);

  // Vecteur binormal
  //vec3 binormal = cross(norm, tangent);

  // Matrice TBN
  //mat3 TBN = mat3(tangent, binormal, norm);

  //vec3 normTex = texNormal.xyz*2.0-1.0;
  //vec3 newNorm = TBN*normTex;
//  vec3 n = normalize(newNorm);
  vec3 n = normalize(normalView);
  vec3 e = normalize(eyeView);
  vec3 l = normalize(light);

  // float diff = max(dot(l,n),0.);
  // float spec = pow(max(dot(reflect(l,n),e),0.0),et);

  //bufferColor = texColor*(diff + spec)*2.0;
//bufferColor = (diff*vec3(1.0)+spec*vec4(1));
  bufferColor = vec4(0.5);
}
