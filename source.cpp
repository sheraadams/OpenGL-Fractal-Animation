// Shera Adams
// shera.adams@snhu.edu
// February 13, 2024
//
// Version: 2.0, CMake cross-platform compatible.
//
// The main purpose of source.cpp is to create an OpenGL application with a full interactive scene in which the user can
// navigate the point of view using keyboard and mouse controls. It initializes GLFW and GLAD and follows the
// OpenGL rendering pipeline to create a fractal flower animation that changes color over time.
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>
#ifndef __APPLE__
#include "irrKlang.h"
#pragma comment(lib, "irrKlang.lib")
using namespace irrklang;
ISoundEngine* SoundEngine = createIrrKlangDevice();
#endif
#include <ft2build.h>
#include FT_FREETYPE_H

#include "petal.h"
#include "objects.h"
#include "icosphere.h"

#include "filesystem.h"
#include "shader.h"
#include "camera.h"
#include "model.h"

/* FUNCTIONS */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

#ifndef __APPLE__
void GetDesktopResolution(float& horizontal, float& vertical);
#endif
/* FUNCTIONS */

/* VARIABLES */
unsigned int planeVAO;
unsigned int VBO, VAO = 0;
bool Keys[1024];
bool firstMouse = true;
bool onPerspective = true;
float SCR_WIDTH = 1000; 
float SCR_HEIGHT = 900; 
float speed = .1f;
float lastX = (float)SCR_WIDTH / 2.0; 
float lastY = (float)SCR_HEIGHT / 2.0;
float lastFrame = 0.0f;
float deltaTime = 0.0f;
GLfloat rotateX = 0.0f;
GLfloat rotateY = 0.0f;
GLfloat xoffset = 0.0f;
GLfloat yoffset = 0.0f;

/* CAMERA */
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
#ifndef __APPLE__
void GetDesktopResolution(float& horizontal, float& vertical)
{
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    horizontal = desktop.right;
    vertical = desktop.bottom;
}
#endif
int main()
{
#ifndef __APPLE__
    GetDesktopResolution(SCR_WIDTH, SCR_HEIGHT);
#endif
    /* GLFW INITIALIZE */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Fractals", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    /* GLFW INITIALIZE */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* SHADERS */
    Shader lightingShader("simpleVS.vs", "simpleFS.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");

    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

    /* VERTICES */
    float cubeVertices[] = {
        -0.5f, -2.5f, -0.5f,    0.0f, 0.0f,
        0.5f, -2.5f, -0.5f, 	1.0f, 0.0f,   // top 
        0.5f, -1.5f, -0.5f, 	1.0f, 1.0f,
        0.5f, -1.5f, -0.5f, 	1.0f, 1.0f,
        -0.5f, -1.5f, -0.5f, 	0.0f, 1.0f,
        -0.5f, -2.5f, -0.5f, 	0.0f, 0.0f,

        -0.5f, -2.5f, 0.5f, 	0.0f, 0.0f,
        0.5f, -2.5f, 0.5f, 	    1.0f, 0.0f,
        0.5f, -1.5f, 0.5f, 		1.0f, 1.0f,
        0.5f, -1.5f, 0.5f, 		1.0f, 1.0f, // bottom 
        -0.5f, -1.5f, 0.5f, 	0.0f, 1.0f,
        -0.5f, -2.5f, 0.5f, 	0.0f, 0.0f,

        -0.5f, -1.5f, 0.5f, 	1.0f, 0.0f,
        -0.5f, -1.5f, -0.5f, 	1.0f, 1.0f,
        -0.5f, -2.5f, -0.5f, 	0.0f, 1.0f,
        -0.5f, -2.5f, -0.5f, 	0.0f, 1.0f,  // left
        -0.5f, -2.5f, 0.5f, 	0.0f, 0.0f,
        -0.5f, -1.5f, 0.5f, 	1.0f, 0.0f,

        0.5f, -1.5f, 0.5f, 		1.0f, 0.0f,
        0.5f, -1.5f, -0.5f, 	1.0f, 1.0f,
        0.5f, -2.5f, -0.5f, 	0.0f, 1.0f,
        0.5f, -2.5f, -0.5f, 	0.0f, 1.0f,  // right
        0.5f, -2.5f, 0.5f, 	    0.0f, 0.0f,
        0.5f, -1.5f, 0.5f, 		1.0f, 0.0f,

        -0.5f, -2.5f, -0.5f, 	0.0f, 1.0f,
        0.5f, -2.5f, -0.5f, 	1.0f, 1.0f,
        0.5f, -2.5f, 0.5f, 	    1.0f, 0.0f,
        0.5f, -2.5f, 0.5f, 	    1.0f, 0.0f,
        -0.5f, -2.5f, 0.5f, 	0.0f, 0.0f,  // back 
        -0.5f, -2.5f, -0.5f, 	0.0f, 1.0f,

        -0.5f, -1.5f, -0.5f, 	0.0f, 1.0f,
        0.5f, -1.5f, -0.5f, 	1.0f, 1.0f,
        0.5f, -1.5f, 0.5f, 		1.0f, 0.0f,
        0.5f, -1.5f, 0.5f, 		1.0f, 0.0f,
        -0.5f, -1.5f, 0.5f, 	0.0f, 0.0f, // front
        -0.5f, -1.5f, -0.5f, 		0.0f, 1.0f
    };

    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3(-5.0f, 7.0f, 5.0f),
        glm::vec3(-5.0f, 7.0f, -5.0f),
        glm::vec3(5.0f, 7.0f, 5.0f),
        glm::vec3(5.0f, 7.0f, -5.0f),
        glm::vec3(0.0f, 7.0f, 0.0f),
        glm::vec3(-5.0f, -7.0f, 5.0f),
        glm::vec3(-5.0f, -7.0f, -5.0f),
        glm::vec3(5.0f, -7.0f, 5.0f),
        glm::vec3(5.0f, -7.0f, -5.0f),
        glm::vec3(0.0f, -7.0f, 0.0f),

    };

    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };

    /* CUBE */
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    /* PLANE */
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);
    /* PLANE */

    /*  SKYBOX */
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    /* TEXTURES */
    unsigned int cubeTexture = loadTexture("resources/textures/AdobeStock_502119125.jpeg");

    vector<std::string> faces
    {
        "resources/textures/right.jpg", // right 
        "resources/textures/left.jpg", // left 
        "resources/textures/top.jpg", // top  
        "resources/textures/bottom.jpg", // bottom  
        "resources/textures/front.jpg", // front
        "resources/textures/back.jpg", // back
    };
    unsigned int cubemap3Texture = loadCubemap(faces);
    /* TEXTURES */

    /* SET SHADERS */
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    /* SOUND ENGINE */
    //SoundEngine->play2D("song.mp3", true);

    /* SET THE PROJECTION */
    onPerspective = true;
    camera.Perspective = false;

    /* RENDER LOOP */
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection, view, model;
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        view = camera.GetViewMatrix();

        /* set shader uniforms */
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", model);

        /* RENDER SCENE */
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);

        model = glm::mat4(1.0f);

        model = glm::scale(model, glm::vec3(.01f));
        lightingShader.setMat4("model", model);
        model = glm::translate(model, glm::vec3(0.0, 0.0, -.5f));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        const float linecolor[] = { 1.0f, 0.0f, 1.0f, 1.0f };

        /* ICOSPHERE */
        Icosphere icosphere(1.0f, 3, false);
        lightingShader.setMat4("model", model);
        model = glm::mat4(1.0f);
        model = glm::rotate(model, (GLfloat)glfwGetTime() * glm::radians(-33.25f) * 2.0f, glm::vec3(0.0f, 0.0f, 1.f));
        lightingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        icosphere.draw();

        double  timeValue = glfwGetTime();
        float greenValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);
        float blueValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);
        float redValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);
       
        // put a lower limit on the color to prevent turning black
        if (greenValue <= .3)
            greenValue = .3;
        if (blueValue <= .3)
            blueValue = .3;
        if (redValue <= .3)
            redValue = .3;

        lightingShader.setInt("spriteColor", 3);
        int vertexColorLocation = glGetUniformLocation(lightingShader.ID, "color");
        for (int i = 0; i < 11; i++)
        {
            glm::vec4 color = glm::vec4(blueValue, 0.0f, redValue, 1.0f);
            lightingShader.setVec3("spriteColor", color);
            glUniform4f(vertexColorLocation, redValue, 0.0f, blueValue, 1.0f);
            model = glm::rotate(model, 20.0f, glm::vec3(0.0f, 0.0f, 1.f));
            lightingShader.setMat4("model", model);
            Petal petal;
            petal.Draw();
        }

        /* RENDER SKYBOX */
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap3Texture);
        glDrawArrays(GL_TRIANGLES, 0, 72);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        /* RENDER SKYBOX */
        switch (onPerspective)
        {
        case 1:

        case 2:
            for (int i = 0; i < 11; i++)
            {
                model = glm::rotate(model, -20.0f, glm::vec3(0.0f, 0.0f, 1.f));
                lightingShader.setMat4("model", model);
                Petal petal;
                petal.Draw();
            }
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /* SWAP BUFFERS AND DELETE VAOS FROM MEMORY */
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}


/* PROCESS INPUT */
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime); 
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if ((glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS))
    {
        onPerspective = true;
        camera.Perspective = false;
        camera.GetViewMatrix();
        camera.ProcessMouseMovement(lastX, lastY);
    }
}

/* CALLBACKS */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (onPerspective && !camera.Perspective)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

/* LOAD TEXTURE WITH STBI */
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

/* LOAD CUBEMAP FOR SKYBOX */
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
