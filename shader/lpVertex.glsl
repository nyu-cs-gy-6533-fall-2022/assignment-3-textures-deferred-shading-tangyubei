#version 150 core

in vec3 quadPosition;

out vec2 uv;

void main()
{
    gl_Position = vec4(quadPosition, 1.0);
    uv = (quadPosition.xy+vec2(1.,1.))/2.0f;
}
