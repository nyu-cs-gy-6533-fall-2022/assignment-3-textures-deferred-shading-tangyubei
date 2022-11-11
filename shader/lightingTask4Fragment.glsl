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

float Sobel_filter();
float linearizeDepth(float depth);

void main()
{
    vec3 pos = vec3(texture(gPosition, uv));
    vec3 n = vec3(texture(gNormal, uv));
    vec3 tex = vec3(texture(gColor, uv));
    vec3 depth = vec3(texture(gDepth, uv));

    vec3 normal = normalize(n);
    vec3 lightDir = normalize(lightPos - pos);
    vec3 col = vec3(1.0f, 0.5f, 0.0f);
    float linearizedDepth = linearizeDepth(depth.r);
    col = clamp(tex * lightParams.x + tex * max(0.0, dot(normal, lightDir)) + vec3(1.0) * pow(max(0.0, dot(normalize((camPos) - pos), normalize(reflect(-lightDir, normal)))), 50.0f), 0.0, 1.0);

    float g = Sobel_filter();

    outQuadColor = vec4(col-g, 1.0f);
     if (tex == vec3(0.0f, 0.0f, 0.0f)) {
        outQuadColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
         return;
    }
//    else {
//        outQuadColor = vec4(col, 1.0f);
//    }
  // outQuadColor = vec4(linearizeDepth(depth.r), linearizeDepth(depth.r), linearizeDepth(depth.r), 1.0f);
}

float Sobel_filter() {

    mat3 vertical = mat3(1.0, 0.0, -1.0, 2.0, 0.0, -2.0, 1.0, 0.0, -1.0);

    mat3 horizontal = mat3(1.0, 2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, -1.0);

    mat3 I;
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            float depth = texture(gDepth, uv + vec2(i-1, j-1)/500).r;
            float linearizedDepth = linearizeDepth(depth);
            I[i][j] = linearizedDepth;
        }
    }

    float G_x = dot(vertical[0], I[0]) + dot(vertical[1], I[1]) + dot(vertical[2], I[2]);
    float G_y = dot(horizontal[0], I[0]) + dot(horizontal[1], I[1]) + dot(horizontal[2], I[2]);

    float G = sqrt(pow(G_x, 2.0f)+pow(G_y, 2.0f));

    return G;
}

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    float near = 0.1f;
    float far  = 1.0f;

    float linearizedDepth = (2.0 * near * far) / (far + near - z * (far - near));
    return linearizedDepth/far;
}
