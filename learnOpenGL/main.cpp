#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <learnopengl/filesystem.h>
#include <iostream>

#include "shader.h"
#include "camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void cursor_enter_callback(GLFWwindow* window, int entered);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

Shader* shader = nullptr;

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
    glfwSetCursorEnterCallback(window, cursor_enter_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // build and compile shaders
    // -------------------------
    //Shader shader("lines.vert", "lines.frag", "lines.geom");
    shader = new Shader("lines.vert", "lines.frag", "lines.geom");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float points[] = {
        //position              //color
        -0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,
         0.0f,  0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,

         0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,
         0.0f, -0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,

        -.25f, .25f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,
        -.25f, .25f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,
         .0f, .0f, -1.0f,         1.0f, 1.0f, 1.0f, 1.0f,
         .25f, -.25f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,
         .25f, -.25f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,
    };
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    shader->use();

    //glm::vec2 viewport(800.f, 600.f);
    //unsigned int viewLoc = glGetUniformLocation(shader.ID, "viewport");
    //glUniform2fv(viewLoc, 1, glm::value_ptr(viewport));
    shader->setVec2("viewport", glm::vec2(800.f, 600.f));
    shader->setFloat("antialias", 1.f);
    shader->setFloat("miter_limit", 1.f);
    //unsigned int antialiasLoc = glGetUniformLocation(shader.ID, "antialias");
    //glUniform1f(antialiasLoc, 1.f);

    //unsigned int linewidthLoc = glGetUniformLocation(shader.ID, "linewidth");
    //glUniform1f(linewidthLoc, 30.f);
    shader->setFloat("linewidth", 3.f);
    //unsigned int miterLimitLoc = glGetUniformLocation(shader.ID, "miter_limit");
    //glUniform1f(miterLimitLoc, 1.f);

    // get matrix's uniform location and set matrix
    glm::mat4 projection = glm::mat4(1.0f);
    //unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");
    //glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    shader->setMat4("projection", projection);

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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        shader->use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //shader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        //shader.setMat4("view", view);
        shader->setMat4("projection", projection * view);

        // draw points
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 15);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    delete shader;
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    shader->use();
    shader->setVec2("viewport", glm::vec2(width, height));
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

void cursor_enter_callback(GLFWwindow* window, int entered)
{
    if (entered)
    {
        // The cursor entered the content area of the window
    }
    else
    {
        // The cursor left the content area of the window
        firstMouse = true;
    }
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}