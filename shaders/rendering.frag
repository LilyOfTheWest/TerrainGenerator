#version 330

layout(location = 0) out vec4 bufferColor;
layout(location = 1) out vec4 outHeight;
layout(location = 2) out vec4 outNormal;

uniform sampler2D normalmap;
uniform sampler2DShadow shadowMapTex;
//2D
uniform sampler2D colormap;
//1D
//uniform sampler1D colormap;

uniform vec3 light;

in  vec3  normalView;
in  vec3  tangentView;
in  vec3  normal;
in  vec4  height;
in  vec2  uvcoord;
in  vec3  eyeView;
in  vec4  project_position;
in  vec4 viewspace;

// used for the last question
vec2 poissonDisk[16] = vec2[](
   vec2( -0.94201624, -0.39906216 ),
   vec2( 0.94558609, -0.76890725 ),
   vec2( -0.094184101, -0.92938870 ),
   vec2( 0.34495938, 0.29387760 ),
   vec2( -0.91588581, 0.45771432 ),
   vec2( -0.81544232, -0.87912464 ),
   vec2( -0.38277543, 0.27676845 ),
   vec2( 0.97484398, 0.75648379 ),
   vec2( 0.44323325, -0.97511554 ),
   vec2( 0.53742981, -0.47373420 ),
   vec2( -0.26496911, -0.41893023 ),
   vec2( 0.79197514, 0.19090188 ),
   vec2( -0.24188840, 0.99706507 ),
   vec2( -0.81409955, 0.91437590 ),
   vec2( 0.19984126, 0.78641367 ),
   vec2( 0.14383161, -0.14100790 )
);

// used for the last question
float random(vec3 seed, int i){
  vec4 seed4 = vec4(seed,i);
  float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
  return fract(sin(dot_product) * 43758.5453);
}

void main() {
  float et = 10.0;
  vec3 norm = normalize(normalView);
  vec3 t = normalize(tangentView);
  vec3 b = normalize(cross(norm,t));
  mat3 tbn = mat3(t,b,norm);

  vec4 texNormalMap = texture(normalmap,uvcoord);
  vec4 shadCoord = project_position*0.5+0.5;
  float bias = 0.015;

  //2D
  vec2 pickColor = vec2(0.5+height.z, height.x);
  //1D
  //float pickColor = 0.4+height.z; //TODO : peut mieux faire pour le calcul de la coordonée ?
  vec4 texColorMap = texture(colormap,pickColor);
  if(texColorMap == vec4(0.0,0.0,0.0,0.0)) texColorMap = vec4(0.0,0.0,1.0,1.0);

//  vec3 newNorm = normalize(tbn*normal);

  vec3 normTex = texNormalMap.xyz*2.0-1.0;
  vec3 newNorm = tbn*normTex;

  vec3 n = normalize(newNorm);
  vec3 e = normalize(eyeView);
  vec3 l = normalize(light);
  l*=vec3(-1.0,1.0,1.0);

  float diff = max(dot(l,n),0.);
  float spec = pow(max(dot(reflect(l,n),e),0.0),et);

  bufferColor = (diff*texColorMap);

  vec4 color = bufferColor;
  // Shadow with soft shadows
   for(int i = 0; i < 16; i++) {
     color = color * texture(shadowMapTex,vec3(shadCoord.xy + poissonDisk[i]/300.0,(shadCoord.z-bias)/shadCoord.w));
   }

  // Shadow with percentage closer filtering
  //color = color*texture(shadowMapTex, vec3(shadCoord.xy, (shadCoord.z-bias)/shadCoord.w));

  // FOG
  const vec4 fogColor = vec4(0.5,0.5,0.5,1.0);
  const float fogDensity = 0.05;
  const float LOG2 = 1.442695;
  float z = gl_FragCoord.z / gl_FragCoord.w;
  float fogFactor = exp2(-fogDensity*fogDensity*z*z*LOG2);
  fogFactor = clamp(fogFactor, 0.0,1.0);

  //bufferColor = color;
  bufferColor = mix(fogColor, color, fogFactor);

  outHeight = height;
  outNormal=vec4(newNorm, 1.0);
}
