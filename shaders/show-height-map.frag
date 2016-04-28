#version 330

out vec4 outBuffer;

uniform sampler2D heightmap;

void main() {
  outBuffer = texelFetch(heightmap,ivec2(gl_FragCoord.xy),0);
}
