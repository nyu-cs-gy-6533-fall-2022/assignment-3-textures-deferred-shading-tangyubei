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

uniform sampler2D ourTexture;

#define PI radians(180.0f)

void main()
{
    vec2 temp = TexCoord;
    temp.x =  atan(pos.x/1.0f, pos.z/1.0f) / (2. * PI) + 0.5;
    vec3 tex = texture(ourTexture, temp).rgb;
    gPosition = vec4(pos, 1.0f);
    gNormal = vec4(normalize(n), 1.0f);
    //gColor = texture(ourTexture, temp);
  //  gColor = vec4(tex, 1.0f);
    gColor = vec4(tex, 1.0f);
    float z = gl_FragCoord.z * 2.0 - 1.0;

    float near = 0.1f;
    float far  = 1.0f;

    float depth = (2.0 * near * far) / (far + near - z * (far - near));
   // gDepth = vec4(vec3(depth/far), 1.0f);
}
