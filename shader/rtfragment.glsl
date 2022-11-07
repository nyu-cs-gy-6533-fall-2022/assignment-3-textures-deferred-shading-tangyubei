#version 150 core

out vec4 outQuadColor;

in vec2 uv;

uniform sampler2D renderedTexture;

void main()
{
  outQuadColor = texture(renderedTexture, uv);
 // outQuadColor = vec4(uv, 0.0f, 1.0f);
  //outColor = vec3(0.5f, 0.5f, 1.0f);
}
