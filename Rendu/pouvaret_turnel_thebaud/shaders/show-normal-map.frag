#version 330

out vec4 outBuffer;

uniform sampler2D normalmap;

void main() {
  outBuffer = texelFetch(normalmap,ivec2(gl_FragCoord.xy),0);
}
