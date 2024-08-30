
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// biblioteke za transf
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <rg/Texture2D.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(0.0f, 1.0f, 1.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// light
glm::vec3 lampColor(1.0, 1.0, 0.5);
glm::vec3 lampPos;
glm::vec3 cottagePos;
bool flashLightOn = true;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
    glfwSetKeyCallback(window, key_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ENABLE DEPTH TESTING
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    Shader ourShader("resources/shaders/mainShader.vs", "resources/shaders/mainShader.fs");
    Shader lampShader("resources/shaders/lampCube.vs", "resources/shaders/lampCube.fs");

    Model cottage("resources/objects/cottage/cottage_blender.obj");
    cottage.SetShaderTextureNamePrefix("material.");

    Model vodooDoll("resources/objects/vodoo/vodoo_model1.obj");
    vodooDoll.SetShaderTextureNamePrefix("material.");

    Model lamp("resources/objects/lamp/lamp.obj");
    lamp.SetShaderTextureNamePrefix("material.");

    Model lampStand("resources/objects/lampStand/lamp.obj");
    lampStand.SetShaderTextureNamePrefix("material.");

    // PREPARING STARTING POSITIONS OF OBJECTS ON THE SCENE
    cottagePos = camera.Position + 5.0f*camera.Front;
    lampPos = cottagePos + glm::vec3(-3.0f, 1.0f, 1.0f);
    glm::vec3 lampStandPos = lampPos - glm::vec3(0.0f, 0.5f, 0.0f);
    std::vector<glm::vec3> dollPositions;
    dollPositions.push_back(camera.Position + 3.0f*camera.Front + camera.Right);
    dollPositions.push_back(camera.Position + 2.0f*camera.Front);
    dollPositions.push_back(camera.Position + 2.6f*camera.Front);
    dollPositions.push_back(camera.Position + 1.7f*camera.Front - camera.Right*0.7f);

    while (!glfwWindowShouldClose(window))
    {
        float currFrame = static_cast<float>(glfwGetTime());
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
        processInput(window);

        std::map <float, glm::vec3> sortedDollPositions;
        for(unsigned int i = 0; i < dollPositions.size(); i++) {
            float distance = glm::length(camera.Position - dollPositions[i]);
            sortedDollPositions[distance] = dollPositions[i];
        }

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // !!!

        // DRAW THE COTTAGE
        ourShader.use();

        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, cottagePos);
        m = glm::scale(m, glm::vec3(0.1f));
        view  = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("model", m);

        // dir light
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light / lamp
        ourShader.setVec3("pointLight.position", lampPos);
        ourShader.setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLight.constant", 1.0f);
        ourShader.setFloat("pointLight.linear", 0.09f);
        ourShader.setFloat("pointLight.quadratic", 0.032f);
        // spot light
        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        if(flashLightOn) {
            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
            ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        } else {
            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
        }
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09f);
        ourShader.setFloat("spotLight.quadratic", 0.032f);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        ourShader.setFloat("material.shininess", 150.0f);

        cottage.Draw(ourShader);

        // DRAW THE LAMP
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        m = glm::mat4(1.0f);
        m = glm::translate(m, lampPos);
        m = glm::scale(m, glm::vec3(0.2f));
        lampShader.setMat4("model", m);
        lampShader.setVec3("lightCubeColor", lampColor);

        lamp.Draw(lampShader);
        // DRAW THE LAMP STAND
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        m = glm::mat4(1.0f);
        m = glm::translate(m, lampStandPos);
        m = glm::scale(m, glm::vec3(0.2f));
        ourShader.setMat4("model", m);
        lampStand.Draw(ourShader);

        // draw the dolls
        ourShader.use();
        ourShader.setFloat("material.shininess", 10.0f);
        for(std::map<float, glm::vec3>::reverse_iterator it = sortedDollPositions.rbegin(); it != sortedDollPositions.rend(); ++it) {
            m = glm::mat4(1.0f);
            m = glm::translate(m, it->second);
            m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            m = glm::scale(m, glm::vec3(0.1f));
            ourShader.setMat4("model", m);
            vodooDoll.Draw(ourShader);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //ourShader.deleteProgram();

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        flashLightOn = !flashLightOn;
}

