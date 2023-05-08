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
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1200;

unsigned int VBO;
//Vertex Array Object (VAO)
unsigned int cubeVAO;
unsigned int lightVAO;

unsigned int lightingShader;
unsigned int lampShader;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 8.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//lighting
float lightX, lightY, lightZ;
glm::vec3 lightPos;

bool rotFlg1 = false;
bool rotFlg2 = false;
float angle1 = 0.0f;
float angle2 = 0.0f;

void init(void)
{

    lightingShader = loadShader("../../src/shader/lighting_maps.vert", "../../src/shader/lighting_maps.frag");
    lampShader = loadShader("../../src/shader/lamp.vert", "../../src/shader/lamp.frag");

    float vertices[] = {
        // Vertex position    // Normals     
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };


    // second, configure the light's VAO (VBO stays the same; 
    // the vertices are the same for the light object which is also a 3D cube)
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute for cubeVAO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute for cubeVAO
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Configure the light's VAO (using the same VBO)
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Position attribute for lightVAO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    lightX = 1.0f;
    lightY = 0.1f;
    lightZ = 2.0f;
    lightPos = glm::vec3(lightX, lightY, lightZ);
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 04: Q1 Kaleb", NULL, NULL);
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


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("../../src/shader/1.model_loading.vert", "../../src/shader/1.model_loading.frag");

    // load models
    // -----------
    Model ourModel(FileSystem::getPath("resources/objects/moon/smallMoon.obj"));


    // call init
    init();

    lightX = 2.2f;
    lightY = 0.1f;
    lightZ = 2.2f;

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

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate
        glUseProgram(lightingShader);

        unsigned int objectColorLoc = glGetUniformLocation(lightingShader, "objectColor");
        glUniform3f(objectColorLoc, 0.3f, 0.3f, 0.3f);

        unsigned int lightColorLoc = glGetUniformLocation(lightingShader, "lightColor");
        glUniform3f(lightColorLoc, 10.0f, 10.0f, 10.0f);

        unsigned int lightPosLoc = glGetUniformLocation(lightingShader, "lightPos");
        glUniform3f(lightPosLoc, lightX, lightY, lightZ);

        unsigned int cameraPosLoc = glGetUniformLocation(lightingShader, "viewPos");
        glUniform3f(cameraPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

        //material properties
        unsigned int materialAmbientLoc = glGetUniformLocation(lightingShader, "material.ambient");
        glUniform3f(materialAmbientLoc, 3.3f, 3.3f, 3.3f);

        unsigned int materialDiffuseLoc = glGetUniformLocation(lightingShader, "material.diffuse");
        glUniform3f(materialDiffuseLoc, 3.3f, 3.3f, 3.3f);

        //specular lighting doesn't have full effect on this object's material
        unsigned int materialSpecularLoc = glGetUniformLocation(lightingShader, "material.specular");
        glUniform3f(materialSpecularLoc, 0.0f, 0.0f, 1.0f);

        unsigned int materialShininessLoc = glGetUniformLocation(lightingShader, "material.shininess");
        glUniform1f(materialShininessLoc, 64.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        unsigned int projectionLoc = glGetUniformLocation(lightingShader, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        unsigned int viewLoc = glGetUniformLocation(lightingShader, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);

        if (rotFlg1)
        {
            angle1 = -(float)glfwGetTime() / 2.0;
            model = glm::rotate(model, angle1, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else {
            model = glm::rotate(model, -angle1, glm::vec3(0.0f, 1.0f, 0.0f));
        }

        model = glm::scale(model, glm::vec3(1.0f));
        unsigned int modelLoc = glGetUniformLocation(lightingShader, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        if (rotFlg2)
        {
            lightX = 2.0f * sin(glfwGetTime());
            //lightY = 0.1f;
            lightZ = 2.0f * cos(glfwGetTime());

            lightPos = glm::vec3(lightX, lightY, lightZ);
        }
        else
            lightPos = glm::vec3(lightX, lightY, lightZ);

        // Draw the light source cube
        glUseProgram(lampShader);

        // Set the view and projection matrices for the lamp shader
        projectionLoc = glGetUniformLocation(lampShader, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        viewLoc = glGetUniformLocation(lampShader, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Set the model matrix for the lamp shader
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(lightX, lightY, lightZ));
        model = glm::scale(model, glm::vec3(0.2f)); // Scale down the light source cube
        modelLoc = glGetUniformLocation(lampShader, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Draw the light source cube using the lightVAO
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 ourModelMatrix = glm::mat4(1.0f);
        ourModelMatrix = glm::translate(ourModelMatrix, glm::vec3(10000.0f, 250.0f, -10000.0f)); // Move the model to the desired position
        ourModelMatrix = glm::scale(ourModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f)); // Scale down the model if it's too large

        if (rotFlg1)
        {
            angle1 = (float)glfwGetTime() / 2.0;
            model = glm::rotate(model, angle1, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else {
            model = glm::rotate(model, angle1, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        
        ourShader.setMat4("model", ourModelMatrix);
        ourModel.Draw(ourShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
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

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        rotFlg1 = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        rotFlg1 = false;
        rotFlg2 = false;
    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        rotFlg2 = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        rotFlg1 = false;
        rotFlg2 = false;
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
