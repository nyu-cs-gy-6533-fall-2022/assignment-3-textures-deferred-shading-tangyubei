// This example is heavily based on the tutorial at https://open.gl

int task = 1;
// OpenGL Helpers to reduce the clutter
#include "Helpers.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#else
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#endif

// OpenGL Mathematics Library
#include <glm/glm.hpp> // glm::vec3
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

#define _USE_MATH_DEFINES
#include <math.h>

// VertexBufferObject wrapper
BufferObject VBO;
// VertexBufferObject wrapper
BufferObject NBO;
// TextureBufferObject wrapper
BufferObject TBO;
//QuadBufferObject wrapper
BufferObject FBO;

// VertexBufferObject wrapper
BufferObject IndexBuffer;

// Contains the vertex positions
std::vector<glm::vec3> V(3);
// Contains the vertex positions
std::vector<glm::vec3> VN(3);
// Contains the vertex positions
std::vector<glm::ivec3> T(3);
// Contains the texture positions
std::vector<glm::vec2> VT(3);
// Last position of the mouse on click
double xpos, ypos;

// camera setup and matrix calculations
glm::vec3 cameraPos;
glm::vec3 cameraTarget;
glm::vec3 cameraDirection;
glm::vec3 cameraUp;
glm::vec3 cameraRight;
glm::mat4 viewMatrix;
glm::mat4 projMatrix;

float camRadius = 5.0f;

// PPM Reader code from http://josiahmanson.com/prose/optimize_ppm/

struct RGB {
    unsigned char r, g, b;
};

struct ImageRGB {
    int w, h;
    std::vector<RGB> data;
};

void eat_comment(std::ifstream& f) {
    char linebuf[1024];
    char ppp;
    while (ppp = f.peek(), ppp == '\n' || ppp == '\r')
        f.get();
    if (ppp == '#')
        f.getline(linebuf, 1023);
}

bool loadPPM(ImageRGB& img, const std::string& name) {
    std::ifstream f(name.c_str(), std::ios::binary);
    if (f.fail()) {
        std::cout << "Could not open file: " << name << std::endl;
        return false;
    }

    // get type of file
    eat_comment(f);
    int mode = 0;
    std::string s;
    f >> s;
    if (s == "P3")
        mode = 3;
    else if (s == "P6")
        mode = 6;

    // get w
    eat_comment(f);
    f >> img.w;

    // get h
    eat_comment(f);
    f >> img.h;

    // get bits
    eat_comment(f);
    int bits = 0;
    f >> bits;

    // error checking
    if (mode != 3 && mode != 6) {
        std::cout << "Unsupported magic number" << std::endl;
        f.close();
        return false;
    }
    if (img.w < 1) {
        std::cout << "Unsupported width: " << img.w << std::endl;
        f.close();
        return false;
    }
    if (img.h < 1) {
        std::cout << "Unsupported height: " << img.h << std::endl;
        f.close();
        return false;
    }
    if (bits < 1 || bits > 255) {
        std::cout << "Unsupported number of bits: " << bits << std::    endl;
        f.close();
        return false;
    }

    // load image data
    img.data.resize(img.w * img.h);

    if (mode == 6) {
        f.get();
        f.read((char*)&img.data[0], img.data.size() * 3);
    }
    else if (mode == 3) {
        for (int i = 0; i < img.data.size(); i++) {
            int v;
            f >> v;
            img.data[i].r = v;
            f >> v;
            img.data[i].g = v;
            f >> v;
            img.data[i].b = v;
        }
    }

    // close file
    f.close();
    return true;
}

bool loadOFFFile(std::string filename, std::vector<glm::vec3>& vertex, std::vector<glm::ivec3>& tria, glm::vec3& min, glm::vec3& max)
{
    min.x = FLT_MAX;
    max.x = FLT_MIN;
    min.y = FLT_MAX;
    max.y = FLT_MIN;
    min.z = FLT_MAX;
    max.z = FLT_MIN;
    try {
        std::ifstream ofs(filename, std::ios::in | std::ios_base::binary);
        if (ofs.fail()) return false;
        std::string line, tmpStr;
        // First line(optional) : the letters OFF to mark the file type.
        // Second line : the number of vertices, number of faces, and number of edges, in order (the latter can be ignored by writing 0 instead).
        int numVert = 0;
        int numFace = 0;
        int numEdge = 0;
        // first line must be OFF
        getline(ofs, line);
        if (line.rfind("OFF", 0) == 0)
            getline(ofs, line);
        std::stringstream tmpStream(line);
        getline(tmpStream, tmpStr, ' ');
        numVert = std::stoi(tmpStr);
        getline(tmpStream, tmpStr, ' ');
        numFace = std::stoi(tmpStr);
        getline(tmpStream, tmpStr, ' ');
        numEdge = std::stoi(tmpStr);

        // read all vertices and get min/max values
        V.resize(numVert);
        for (int i = 0; i < numVert; i++) {
            getline(ofs, line);
            tmpStream.clear();
            tmpStream.str(line);
            getline(tmpStream, tmpStr, ' ');
            V[i].x = std::stof(tmpStr);
            min.x = std::fminf(V[i].x, min.x);
            max.x = std::fmaxf(V[i].x, max.x);
            getline(tmpStream, tmpStr, ' ');
            V[i].y = std::stof(tmpStr);
            min.y = std::fminf(V[i].y, min.y);
            max.y = std::fmaxf(V[i].y, max.y);
            getline(tmpStream, tmpStr, ' ');
            V[i].z = std::stof(tmpStr);
            min.z = std::fminf(V[i].z, min.z);
            max.z = std::fmaxf(V[i].z, max.z);
        }

        // read all faces (triangles)
        T.resize(numFace);
        for (int i = 0; i < numFace; i++) {
            getline(ofs, line);
            tmpStream.clear();
            tmpStream.str(line);
            getline(tmpStream, tmpStr, ' ');
            if (std::stoi(tmpStr) != 3) return false;
            getline(tmpStream, tmpStr, ' ');
            T[i].x = std::stoi(tmpStr);
            getline(tmpStream, tmpStr, ' ');
            T[i].y = std::stoi(tmpStr);
            getline(tmpStream, tmpStr, ' ');
            T[i].z = std::stoi(tmpStr);
        }

        ofs.close();
    }
    catch (const std::exception& e) {
        // return false if an exception occurred
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

void sphere(float sphereRadius, int sectorCount, int stackCount, std::vector<glm::vec3>& vertex, std::vector<glm::vec3>& normal, std::vector<glm::ivec3>& tria, std::vector<glm::vec2>& texture) {
    // init variables
    vertex.resize(0);
    normal.resize(0);
    tria.resize(0);
    texture.resize(0);
    // temp variables
    glm::vec3 sphereVertexPos;
    float xy;
    float sectorStep = 2.0f * M_PI / float(sectorCount);
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    // compute vertices and normals
    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = M_PI / 2.0f - i * stackStep;
        xy = sphereRadius * cosf(stackAngle);
        sphereVertexPos.z = sphereRadius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;

            // vertex position
            sphereVertexPos.x = xy * cosf(sectorAngle);
            sphereVertexPos.y = xy * sinf(sectorAngle);
            vertex.push_back(sphereVertexPos);

            // normalized vertex normal
            normal.push_back(sphereVertexPos / sphereRadius);
            // texture coordinates
            glm::vec2 texturePos;
            texturePos.x = atan2(sphereVertexPos.x/sphereRadius, sphereVertexPos.z/sphereRadius) / (2. * M_PI) + 0.5;
            texturePos.y = -asin(sphereVertexPos.y/sphereRadius) / M_PI + .5;
            texture.push_back(texturePos);
        }
    }

    // compute triangle indices
    int k1, k2;
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0) {
                tria.push_back(glm::ivec3(k1, k2, k1 + 1));
            }
            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1)) {
                tria.push_back(glm::ivec3(k1 + 1, k2, k2 + 1));
            }
        }
    }

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Update the position of the first vertex if the left button is pressed
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Get the position of the mouse in the window
        glfwGetCursorPos(window, &xpos, &ypos);
        std::cout << xpos << " " << ypos << std::endl;
    }

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // temp variables
    glm::mat3 rot;
    // Update the position of the first vertex if the keys 1,2, or 3 are pressed
    switch (key)
    {
        case GLFW_KEY_A:
            rot = glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f), cameraUp);
            cameraPos = rot * cameraPos;
            cameraDirection = glm::normalize(cameraPos - cameraTarget);
            cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
            break;
        case GLFW_KEY_D:
            rot = glm::rotate(glm::mat4(1.0f), glm::radians(5.0f), cameraUp);
            cameraPos = rot * cameraPos;
            cameraDirection = glm::normalize(cameraPos - cameraTarget);
            cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
            break;
        case GLFW_KEY_W:
            rot = glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f), cameraRight);
            cameraPos = rot * cameraPos;
            cameraDirection = glm::normalize(cameraPos - cameraTarget);
            cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
            break;
        case GLFW_KEY_S:
            rot = glm::rotate(glm::mat4(1.0f), glm::radians(5.0f), cameraRight);
            cameraPos = rot * cameraPos;
            cameraDirection = glm::normalize(cameraPos - cameraTarget);
            cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
            break;
        case GLFW_KEY_UP:
            cameraPos -= cameraDirection * 0.25f;
            break;
        case GLFW_KEY_DOWN:
            cameraPos += cameraDirection * 0.25f;
            break;
        case GLFW_KEY_R:
            cameraPos = glm::vec3(0.0f, 0.0f, camRadius);
            cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
            cameraDirection = glm::normalize(cameraPos - cameraTarget);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
            cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
            break;
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        default:
            break;
    }

}

int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 600, "Hello OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

#ifndef __APPLE__
    glewExperimental = true;
      GLenum err = glewInit();
      if(GLEW_OK != err)
      {
        /* Problem: glewInit failed, something is seriously wrong. */
       std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
      }
      glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
      std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
#endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    std::cout << "OpenGL version recieved: " << major << "." << minor << "." << rev << std::endl;
    std::cout << "Supported OpenGL is " << (const char*)glGetString(GL_VERSION) << std::endl;
    std::cout << "Supported GLSL is " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Initialize the VAO
    // A Vertex Array Object (or VAO) is an object that describes how the vertex
    // attributes are stored in a Vertex Buffer Object (or VBO). This means that
    // the VAO is not the actual object storing the vertex data,
    // but the descriptor of the vertex data.
    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();

    // Initialize the VBO with the vertices data
    VBO.init();
    // initialize normal array buffer
    NBO.init();
    // initialize texture array buffer
    TBO.init();
    // initialize element array buffer
    IndexBuffer.init(GL_ELEMENT_ARRAY_BUFFER);
    // initialize model matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    unsigned int texture;
    // 1: generate sphere, 0: load OFF model
#if 1
    // generate sphere (radius, #sectors, #stacks, vertices, normals, triangle indices, texture coordinates)
    sphere(1.0f, 36, 18, V, VN, T, VT);
    VBO.update(V);
    NBO.update(VN);
    TBO.update(VT);
    IndexBuffer.update(T);

    // load PPM image file
    ImageRGB image;
    bool imageAvailable = loadPPM(image, "../data/land_shallow_topo_2048.ppm");

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

// load and generate the texture
    if (imageAvailable)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.w, image.h, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, &image.data[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, texture);
#else
    // load  OFF file
    glm::vec3 min, max, tmpVec;
    std::cout << "Loading OFF file...";
    loadOFFFile("../data/stanford_dragon2.off", V, T, min, max);
    //loadOFFFile("../data/bunny.off", V, T, min, max);
    std::cout << " done! " << V.size() << " vertices, " << T.size() << " triangles" << std::endl;
    VBO.update(V);
    IndexBuffer.update(T);

    // compute model matrix so that the mesh is inside a -1..1 cube
    tmpVec = max - min;
    float maxVal = glm::max(tmpVec.x, glm::max(tmpVec.y, tmpVec.z));
    tmpVec /= 2.0f;
    modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f / maxVal));
    modelMatrix *= glm::translate(glm::mat4(1.0f), -(min + tmpVec));

    // compute face normals
    std::cout << "Computing face normals...";
    std::vector<glm::vec3> faceN(3);
    faceN.resize(T.size());
    for (unsigned int i = 0; i < faceN.size(); i++) {
        faceN[i] = glm::normalize(glm::cross(V[T[i].y] - V[T[i].x], V[T[i].z] - V[T[i].x]));
    }
    std::cout << " done!" << std::endl;
    // compute vertex normals
    std::cout << "Computing vertex normals...";
    VN.resize(V.size());
    for (unsigned int i = 0; i < VN.size(); i++) {
        VN[i] = glm::vec3(0.0f);
    }
    for (unsigned int j = 0; j < T.size(); j++) {
        VN[T[j].x] += faceN[j];
        VN[T[j].y] += faceN[j];
        VN[T[j].z] += faceN[j];
    }
    for (unsigned int i = 0; i < VN.size(); i++) {
        VN[i] = glm::normalize(VN[i]);
    }
    std::cout << " done!" << std::endl;
    // initialize normal array buffer
    NBO.init();
   NBO.update(VN);
#endif

    // Initialize the OpenGL Program
    // A program controls the OpenGL pipeline and it must contains
    // at least a vertex shader and a fragment shader to be valid
    Program program;
    std::string fragPath;
    std::string vertPath;
    fragPath = "../shader/fragment.glsl";
    vertPath = "../shader/vertex.glsl";

    // load fragment shader file
    std::ifstream fragShaderFile(fragPath);
    std::stringstream fragCode;
    fragCode << fragShaderFile.rdbuf();
    // load vertex shader file
    std::ifstream vertShaderFile(vertPath);
    std::stringstream vertCode;
    vertCode << vertShaderFile.rdbuf();
    // Compile the two shaders and upload the binary to the GPU
    // Note that we have to explicitly specify that the output "slot" called outColor
    // is the one that we want in the fragment buffer (and thus on screen)
    program.init(vertCode.str(), fragCode.str());
    program.bind();

    // ------------------------------
    // RENDER TO TEXTURE
    // ------------------------------

    // Define some variables out of scope of the switch statement
    GLuint FramebufferName = 0;
    int windowWidth = 1024;
    int windowHeight = 768;
    Program quadProgram;
    GLuint renderedTexture;

    if (task == 2) {
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

        glGenFramebuffers(1, &FramebufferName);
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

        // Create new texture
        glGenTextures(1, &renderedTexture);

        // Bind new texture
        glBindTexture(GL_TEXTURE_2D, renderedTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // depth buffer
        GLuint depthrenderbuffer;
        glGenRenderbuffers(1, &depthrenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

        // Check framebuffer
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            return false;

        std::vector<glm::vec3> quad_buffer_data;
        quad_buffer_data.resize(0);
        quad_buffer_data.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
        quad_buffer_data.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
        quad_buffer_data.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
        quad_buffer_data.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
        quad_buffer_data.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
        quad_buffer_data.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
        FBO.init();
        FBO.update(quad_buffer_data);

        //   Program quadProgram;
        // load fragment shader file
        std::ifstream rtFragShaderFile("../shader/rtfragment.glsl");
        std::stringstream rtfragCode;
        rtfragCode << rtFragShaderFile.rdbuf();
        // load vertex shader file
        std::ifstream rtVertShaderFile("../shader/rtvertex.glsl");
        std::stringstream rtvertCode;
        rtvertCode << rtVertShaderFile.rdbuf();
        // Compile the two shaders and upload the binary to the GPU
        quadProgram.init(rtvertCode.str(), rtfragCode.str());
        quadProgram.bind();
    }
    // Register the keyboard callbackx
    glfwSetKeyCallback(window, key_callback);

    // Register the mouse callback
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Update viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // camera setup
    cameraPos = glm::vec3(0.0f, 0.0f, camRadius);
    cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    cameraDirection = glm::normalize(cameraPos - cameraTarget);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Get the size of the window
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        if (task == 2) {
            // Render to the framebuffer with texture attached
            glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
            glViewport(0,0,windowWidth,windowHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right
        }
        // matrix calculations
        viewMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        projMatrix = glm::perspective(glm::radians(35.0f), (float)width / (float)height, 0.1f, 100.0f);

        // Bind your VAO (not necessary if you have only one)
        VAO.bind();

        // bind your element array
        IndexBuffer.bind();

        // Bind your program
        program.bind();

        // Set the uniform values
        // Set active texture to map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        program.bindVertexAttribArray("position", VBO);
        program.bindVertexAttribArray("normal", NBO);
        program.bindVertexAttribArray("aTexCoord", TBO);
        glUniform1i(program.uniform("ourTexture"), 0);
        glUniform3f(program.uniform("triangleColor"), 1.0f, 0.5f, 0.0f);
        glUniform3f(program.uniform("camPos"), cameraPos.x, cameraPos.y, cameraPos.z);
        glUniformMatrix4fv(program.uniform("modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(program.uniform("viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(program.uniform("projMatrix"), 1, GL_FALSE, glm::value_ptr(projMatrix));
        // direction towards the light
        glUniform3fv(program.uniform("lightPos"), 1, glm::value_ptr(glm::vec3(-1.0f, 2.0f, 3.0f)));
        // x: ambient;
        glUniform3f(program.uniform("lightParams"), 0.1f, 50.0f, 0.0f);

        // Discards all faces that are back facing
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Clear the framebuffer
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);

        // Draw globe
        //glDrawArrays(GL_TRIANGLES, 0, V.size());
        glDrawElements(GL_TRIANGLES, T.size() * 3, GL_UNSIGNED_INT, 0);
        // Uncomment for wireframe
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        if (task == 2) {

            // Render to the screen
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // Render on the whole framebuffer, complete from the lower left corner to the upper right
            glViewport(0,0,windowWidth,windowHeight);

            // Clear the screen
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            quadProgram.bind();
            quadProgram.bindVertexAttribArray("quadPosition", FBO);
            // Set active texture to quad texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, renderedTexture);
            // Set our "renderedTexture" sampler to use Texture Unit 0
            glUniform1i(program.uniform("renderedTexture"), 0);

            // Draw quads, 6 vertices for 2 triangles
            glDrawArrays(GL_TRIANGLES, 0, 6);

        }
        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Deallocate opengl memory
    program.free();
    IndexBuffer.free();
    VAO.free();
    VBO.free();
    NBO.free();
    TBO.free();
    if (task == 2) {
        quadProgram.free();
        FBO.free();
    }

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}
