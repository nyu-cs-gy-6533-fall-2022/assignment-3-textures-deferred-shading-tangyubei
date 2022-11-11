#version 150 core

out vec4 gPosition;
out vec4 gNormal;
out vec4 gColor;
//out vec4 gDepth;

in vec3 pos;
in vec3 n;
in vec2 TexCoord;

uniform vec3 triangleColor;
uniform vec3 lightPos;
uniform vec3 lightParams;
uniform vec3 camPos;

void main()
{
    gPosition = vec4(pos, 1.0f);
    gNormal = vec4(normalize(n), 1.0f);
    gColor = vec4(triangleColor, 1.0f);
}
