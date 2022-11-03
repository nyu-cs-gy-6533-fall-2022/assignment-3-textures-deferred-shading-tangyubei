#version 150 core

out vec4 outColor;

in vec3 n;
in vec3 color;
in vec3 pos;
in vec2 TexCoord;

uniform vec3 triangleColor;
uniform vec3 lightPos;
uniform vec3 lightParams;
uniform vec3 camPos;

uniform sampler2D ourTexture;

#define PI 3.141592653589793238462643383279

void main()
{
    //vec3 col = color;
    vec2 temp = TexCoord;
    temp.x =  atan(pos.x/1.0f, pos.z/1.0f) / (2. * PI) + 0.5;
    vec3 tex = texture(ourTexture, temp).rgb;
    vec3 normal = normalize(n);
    vec3 lightDir = normalize(lightPos - pos);
    vec3 col = clamp( tex * lightParams.x +
    tex * max(0.0, dot(normal, lightDir)) +
        vec3(1.0) * pow(max(0.0, dot( normalize(camPos - pos), normalize( reflect(-lightDir, normal)))), lightParams.y),
        0.0, 1.0);
    
    outColor = vec4(col, 1.0);
    //outColor = texture(ourTexture, TexCoord);
    //outColor = texture(ourTexture, temp);
}
