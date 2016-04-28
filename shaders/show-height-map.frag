#version 330

out vec4 outBuffer;

in vec2 coord;

uniform sampler2D heightmap;

void main() {
  //outBuffer = texelFetch(heightmap,ivec2(gl_FragCoord.xy),0)*0.05;
  outBuffer = texture(heightmap, coord)*0.05+0.5;
}
