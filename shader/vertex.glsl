#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec3 triangleColor;
uniform vec3 lightPos;
uniform vec3 lightParams;
uniform vec3 camPos;

in vec3 position;
in vec3 normal;
in vec2 aTexCoord;

out vec3 n;
out vec3 color;
out vec3 pos;
out vec2 TexCoord;

void main()
{
    n = mat3(transpose(inverse(modelMatrix))) * normal;
    color = triangleColor;
    pos = vec3(modelMatrix * vec4(position, 1.0));
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    TexCoord = aTexCoord;
}