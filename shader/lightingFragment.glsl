#version 150 core

out vec4 outQuadColor;

in vec2 uv;

uniform vec3 lightPos;
uniform vec3 lightParams;
uniform vec3 camPos;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColor;
uniform sampler2D gDepth;

float linearizeDepth(float depth);

void main()
{
    vec3 pos = vec3(texture(gPosition, uv));
    vec3 n = vec3(texture(gNormal, uv));
    vec3 tex = vec3(texture(gColor, uv));
    vec3 depth = vec3(texture(gDepth, uv));

    vec3 normal = normalize(n);
    vec3 lightDir = normalize(lightPos - pos);
    vec3 col = tex;
    float linearizedDepth = linearizeDepth(depth.r);
        col = clamp(tex * lightParams.x +
        tex * max(0.0, dot(normal, lightDir)) +
        vec3(1.0) * pow(max(0.0, dot(normalize(camPos - pos), normalize(reflect(-lightDir, normal)))), 50.0f),
                         0.0, 1.0);

    if (n == vec3(0.0, 0.0, 0.0)) {
        outQuadColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    }
    else {
        outQuadColor = vec4(col, 1.0f);
    }
   // outQuadColor = vec4(linearizeDepth(depth.r), linearizeDepth(depth.r), linearizeDepth(depth.r), 1.0f);
}

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    float near = 0.1f;
    float far  = 1.0f;

    float linearizedDepth = (2.0 * near * far) / (far + near - z * (far - near));
    return linearizedDepth/far;
}
