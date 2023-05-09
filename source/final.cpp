#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "model.h"
#include "filesystem.h"
#include "loadShaders.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

unsigned int floorTexture;

// plane VAO & VBO
unsigned int planeVAO, planeVBO;

// light VAOs & VBO
unsigned int VBO;
unsigned int cubeVAO;
unsigned int lightVAO;

unsigned int lightingShader;

// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//lighting
float lightX, lightY, lightZ;
glm::vec3 lightPos;
glm::vec3 sunLightPos;

float cameraYVelocity = 5.0f * deltaTime;
bool isJumping = false;
float initialCameraYPosition = camera.Position.y; // Store the initial Y position of the camera

bool rotFlg1 = false;
float angle1 = 0.0f;

void init(void)
{
    //lightingShader = loadShader("../../src/shader/lighting_maps.vert", "../../src/shader/lighting_maps.frag");

    float planeVertices[] = {
        // positions          // texture Coords 
        10000.0f, -0.5f,  10000.0f,  10000.0f, 0.0f,
        -10000.0f, -0.5f,  10000.0f,  0.0f, 0.0f,
        -10000.0f, -0.5f, -10000.0f,  0.0f, 10000.0f,

        10000.0f, -0.5f,  10000.0f,  10000.0f, 0.0f,
        -10000.0f, -0.5f, -10000.0f,  0.0f, 10000.0f,
        10000.0f, -0.5f, -10000.0f,  10000.0f, 10000.0f
    };

    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glBindVertexArray(0);


    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    floorTexture = loadTexture("../../src/resources/textures/moonFloor.png");

}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "IT 328 Final Project: Kaleb Liang", NULL, NULL);
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

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // flip
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("../../src/shader/1.model_loading.vert", "../../src/shader/1.model_loading.frag");

    // load models
    // -----------
    Model ourModel(FileSystem::getPath("resources/objects/earth/earth.obj"));
    Model sunModel(FileSystem::getPath("resources/objects/sun/sun.obj"));
    Model marsModel(FileSystem::getPath("resources/objects/mars/planet.obj"));
    Model flagModel(FileSystem::getPath("resources/objects/flag/flag.obj"));

    // unflip skybox
    stbi_set_flip_vertically_on_load(false);
    
    // build and compile shaders
    // -------------------------
    Shader skyboxShader("../../src/shader/2.skybox.vert", "../../src/shader/2.skybox.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float skyboxVertices[] = {
        // positions          
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

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    // load textures
    // -------------
    //unsigned int cubeTexture = loadTexture(FileSystem::getPath("resources/textures/container.jpg").c_str());

    vector<std::string> faces
    {
        FileSystem::getPath("resources/textures/nightsky/right.jpg"),
        FileSystem::getPath("resources/textures/nightsky/left.jpg"),
        FileSystem::getPath("resources/textures/nightsky/top.jpg"),
        FileSystem::getPath("resources/textures/nightsky/bottom.jpg"),
        FileSystem::getPath("resources/textures/nightsky/front.jpg"),
        FileSystem::getPath("resources/textures/nightsky/back.jpg")
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    // shader configuration
    // --------------------
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(1);

    init();
    Shader lampShader("../../src/shader/lamp.vert", "../../src/shader/lamp.frag");

    Shader planeShader("../../src/shader/planeShader.vert", "../../src/shader/planeShader.frag");

    // initialize lighting
    // -------------------
    glm::vec3 lightPos(0.0f, 10.0f, -20.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 0.0f); // yellow light
    glm::vec3 lightAmbient(0.1f, 0.1f, 0.1f);
    glm::vec3 lightDiffuse(0.8f, 0.8f, 0.8f);
    glm::vec3 lightSpecular(1.0f, 1.0f, 1.0f);

    // Pass light properties to shaders
    ourShader.setVec3("light.position", lightPos);
    ourShader.setVec3("light.ambient", lightAmbient);
    ourShader.setVec3("light.diffuse", lightDiffuse);
    ourShader.setVec3("light.specular", lightSpecular);

    glm::vec3 materialAmbient(1.0f, 0.5f, 0.31f);
    glm::vec3 materialDiffuse(1.0f, 0.5f, 0.31f);
    glm::vec3 materialSpecular(0.5f, 0.5f, 0.5f);
    float shininess = 32.0f;

    // Pass material properties to shaders
    ourShader.setVec3("material.ambient", materialAmbient);
    ourShader.setVec3("material.diffuse", materialDiffuse);
    ourShader.setVec3("material.specular", materialSpecular);
    ourShader.setFloat("material.shininess", shininess);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // jumping action
        camera.Position.y += cameraYVelocity * deltaTime;
        cameraYVelocity -= 9.81f * deltaTime;
        if (camera.Position.y <= initialCameraYPosition)
        {
            camera.Position.y = initialCameraYPosition;
            cameraYVelocity = 0.0f;
            isJumping = false;
        }


        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // render the sun
        glm::mat4 model = glm::mat4(1.0f); // initialize model matrix
        glm::vec3 sunPosition = glm::vec3(0.0f, 0.0f, -5.0f); // position of the sun
        model = glm::translate(model, sunPosition); // move the sun to its position
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        lampShader.setMat4("model", model);
        sunModel.Draw(lampShader);

        // update light position to match sun position
        lightPos = sunPosition;

        // use shader
        ourShader.use();
        ourShader.setVec3("light.position", lightPos);

        // view/projection transformations
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
        //glm::mat4 view = camera.GetViewMatrix();
        //glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 10.0f, -25.0f));

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);

        // earth object
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.0f, 0.0f, -30.0f));
        //model = glm::translate(model, glm::vec3(12.0f, 50.0f, -80.0f)); // Adjust the translation values to position the model
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // Scale it down if needed

        if (rotFlg1)
        {
            angle1 = (float)glfwGetTime() / 2.0;
            model = glm::rotate(model, angle1, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else {
            model = glm::rotate(model, angle1, glm::vec3(0.0f, 1.0f, 0.0f));
        }

        
        //glBindTexture(GL_TEXTURE_2D, 0);
        ourShader.setInt("texture_diffuse1", 1);
        ourModel.Draw(ourShader);

        // draw mars object
        glm::mat4 marsModelMatrix = glm::mat4(1.0f);
        marsModelMatrix = glm::translate(marsModelMatrix, glm::vec3(-20.0f, 5.0f, -40.0f));
        marsModelMatrix = glm::scale(marsModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

        ourShader.setMat4("model", marsModelMatrix);
        //ourShader.setInt("texture_diffuse1", 0);
        marsModel.Draw(ourShader);
        //sunLightPos = glm::vec3(marsModelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        //ourShader.setVec3("light.position", sunLightPos);


        // draw the flag
        glm::mat4 flagModelMatrix = glm::mat4(1.0f);
        flagModelMatrix = glm::translate(flagModelMatrix, glm::vec3(0.0f, 0.25f, 0.0f)); // Set the flag model's position within the skybox
        flagModelMatrix = glm::scale(flagModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

        ourShader.setMat4("model", flagModelMatrix);
        //ourShader.setInt("texture_diffuse1", 1);
        flagModel.Draw(ourShader);

        // sun object
        glm::vec3 lightColor(1.0f, 1.0f, 0.0f);
        glm::vec3 lightAmbient = 0.1f * lightColor;
        glm::vec3 lightDiffuse = 0.5f * lightColor;
        glm::vec3 lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

        ourShader.setVec3("light.ambient", lightAmbient);
        ourShader.setVec3("light.diffuse", lightDiffuse);
        ourShader.setVec3("light.specular", lightSpecular);

        glm::mat4 lightModel = glm::mat4(1.0f);

        sunPosition = glm::vec3(0.0f, 30.0f, -80.0f);
        ourShader.setVec3("sun.position", sunPosition);
        //lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::translate(lightModel, glm::vec3(0.0f, 30.0f, -80.0f));
        lightModel = glm::scale(lightModel, glm::vec3(1.0f, 1.0f, 1.0f)); // Scale the sphere down
        ourShader.setMat4("model", lightModel);
        sunModel.Draw(ourShader);


        // floor
        planeShader.use();
        planeShader.setMat4("projection", projection);
        planeShader.setMat4("view", view);

        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        planeShader.setInt("texture_diffuse1", 1);

        model = glm::mat4(1.0f);
        planeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // draw scene as normal
        model = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
    

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isJumping)
    {
        cameraYVelocity = 5.0f; // Adjust the value to control the jump strength
        isJumping = true;
    }


    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        rotFlg1 = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        rotFlg1 = false;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
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
        else
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

// loads a cubemap texture from 6 individual texture faces
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
