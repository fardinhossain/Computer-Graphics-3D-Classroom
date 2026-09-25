// =========================================================================
// Computer Graphics Laboratory - CSE 444
// 3D Classroom Project
// 
// Key Requirements Implemented:
// 1. 3D Transformation: Hierarchical modeling (scale, rotate, translate) for all
//    classroom objects (desks, chairs, podium, blackboard, fan, door, windows, clock).
//    Interactive model transformation (X, Y, Z rotation, translation, scaling).
// 2. Viewing Transformation: Camera system supporting multiple preset angles:
//    - Back View (Main view down the central aisle)
//    - Side View (Profile view of desks, fan, and windows)
//    - Top View (Bird's-eye view of classroom arrangement)
//    - Teacher View (Front view looking at students from the podium)
//    - Free Look (First-person WASD + Mouse exploration)
// 3. Moving Object: Continuously rotating Ceiling Fan with speed & toggle controls.
// 4. Two Kinds of Light:
//    - Point Lights (4 ceiling fixtures providing soft room illumination)
//    - Spotlight (Focused directional cone light illuminating the blackboard)
// 5. Rich Color Palette: Distinct materials and colors for all surfaces.
// =========================================================================

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "pointLight.h"
#include "spotLight.h"

#include <iostream>
#include <algorithm>

using namespace std;

// -------------------------------------------------------------------------
// Function Declarations
// -------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Drawing Helper Functions (All models composed hierarchically of unit cubes)
void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model, 
              float r, float g, float b, float spec = 0.3f, float shininess = 32.0f);
void drawFloor(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 roomBase);
void drawWallsAndCeiling(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 roomBase);
void drawStudentDesk(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 deskBase);
void drawStudentChair(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 chairBase);
void drawTeacherPodium(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 podiumBase);
void drawBlackboard(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 boardBase);
void drawWallClock(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 clockBase, float clockSecondAngle);
void drawCeilingFan(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 fanBase, float fanAngle);
void drawWindow(unsigned int& cubeVAO, Shader& lightingShader, Shader& ourShader, glm::mat4 windowBase);
void drawDoor(unsigned int& cubeVAO, Shader& lightingShader, Shader& ourShader, glm::mat4 doorBase, float doorAngle);
void drawCeilingLightFixture(unsigned int& cubeVAO, Shader& lightingShader, Shader& ourShader, 
                             glm::mat4 lightBase, bool isLightOn);
void drawSpotlightFixture(unsigned int& cubeVAO, Shader& lightingShader, Shader& ourShader, 
                          glm::mat4 fixtureBase, bool isSpotlightOn);
void drawRobot(unsigned int& cubeVAO, Shader& lightingShader, Shader& ourShader, 
               glm::mat4 robotBase, float waveAngle);

// -------------------------------------------------------------------------
// Settings & Window Constants
// -------------------------------------------------------------------------
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// -------------------------------------------------------------------------
// Interactive 3D Model Transformation State
// -------------------------------------------------------------------------
float rotateAngle_X = 0.0f;
float rotateAngle_Y = 0.0f;
float rotateAngle_Z = 0.0f;
float rotateAxis_X = 0.0f;
float rotateAxis_Y = 1.0f;
float rotateAxis_Z = 0.0f;

float translate_X = 0.0f;
float translate_Y = 0.0f;
float translate_Z = 0.0f;

float scale_X = 1.0f;
float scale_Y = 1.0f;
float scale_Z = 1.0f;

// -------------------------------------------------------------------------
// Camera & Viewing Transformations
// -------------------------------------------------------------------------
Camera camera(glm::vec3(0.0f, 2.10f, 4.8f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// The 4 Core Camera Views matching Classroom.jpg reference panels
enum CameraViewMode {
    VIEW_BACK = 0,    // Key 1: Back View (Initial View - looking down center aisle at blackboard)
    VIEW_SIDE = 1,    // Key 2: Side View (SIDE VIEW: profile of desks, fan, and windows)
    VIEW_TOP = 2,     // Key 3: Top View (TOP VIEW: overhead bird's-eye arrangement)
    VIEW_TEACHER = 3  // Key 4: Teacher View (Front view looking from podium at students)
};
CameraViewMode currentView = VIEW_BACK;

void setCameraPreset(CameraViewMode mode) {
    currentView = mode;
    switch (mode) {
        case VIEW_BACK: // 1. Initial / Back View (Classroom.jpg main & BACK VIEW panel)
            camera.Position = glm::vec3(0.0f, 2.10f, 4.8f);
            camera.Yaw = -90.0f;
            camera.Pitch = 0.0f;
            camera.Zoom = 45.0f;
            camera.updateCameraVectors();
            cout << "[Camera View 1] INITIAL / BACK VIEW (Looking down aisle at blackboard)" << endl;
            break;
        case VIEW_SIDE: // 2. Side View (Classroom.jpg SIDE VIEW panel)
            camera.Position = glm::vec3(4.3f, 2.10f, 3.2f);
            camera.Yaw = -140.0f;
            camera.Pitch = -3.0f;
            camera.Zoom = 45.0f;
            camera.updateCameraVectors();
            cout << "[Camera View 2] SIDE VIEW (Profile perspective of desks & windows)" << endl;
            break;
        case VIEW_TOP: // 3. Top View (Classroom.jpg TOP VIEW panel)
            camera.Position = glm::vec3(0.0f, 9.2f, -0.2f);
            camera.Yaw = -90.0f;
            camera.Pitch = -89.0f;
            camera.Zoom = 45.0f;
            camera.updateCameraVectors();
            cout << "[Camera View 3] TOP VIEW (Overhead bird's-eye layout)" << endl;
            break;
        case VIEW_TEACHER: // 4. Teacher View (Front view from podium)
            camera.Position = glm::vec3(0.0f, 1.80f, -4.8f);
            camera.Yaw = 90.0f;
            camera.Pitch = 0.0f;
            camera.Zoom = 45.0f;
            camera.updateCameraVectors();
            cout << "[Camera View 4] TEACHER VIEW (Podium looking at students)" << endl;
            break;
    }
}

// -------------------------------------------------------------------------
// Requirement 3: Moving Objects (Ceiling Fan, Clock Hand & Interactive Door)
// -------------------------------------------------------------------------

// Moving Object 1: Animated Rotating Ceiling Fan
float fanAngle = 0.0f;
float fanSpeed = 220.0f; // rotation speed in degrees per second
bool isFanOn = true;

// Moving Object 2: Animated Wall Clock Second Hand (Rotates CLOCKWISE around Z)
float clockSecondAngle = 0.0f;
float clockSpeed = 6.0f; // Real-world speed: 360 degrees in 60 seconds = 6.0 deg/sec

// Moving Object 3: Interactive Classroom Door (Open / Close smoothly)
bool isDoorOpen = false;
float doorAngle = 0.0f;
float targetDoorAngle = 0.0f;

// Moving Object 4: Animated 3D Classroom Robot Waving Hand ("Bye-Bye" Motion)
float robotWaveAngle = 0.0f;

// -------------------------------------------------------------------------
// Requirement 4: Lighting (Point Lights & Spotlight)
// -------------------------------------------------------------------------

// Four Point Light positions on the ceiling for even illumination
glm::vec3 pointLightPositions[] = {
    glm::vec3(-2.3f, 3.85f, -3.2f),
    glm::vec3( 2.3f, 3.85f, -3.2f),
    glm::vec3(-2.3f, 3.85f,  1.2f),
    glm::vec3( 2.3f, 3.85f,  1.2f)
};

PointLight pointlight1(
    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,
    0.15f, 0.15f, 0.13f,   // ambient
    0.85f, 0.82f, 0.75f,   // diffuse (warm classroom light)
    0.50f, 0.50f, 0.50f,   // specular
    1.0f, 0.09f, 0.032f,   // attenuation factors (constant, linear, quadratic)
    1
);

PointLight pointlight2(
    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,
    0.15f, 0.15f, 0.13f,
    0.85f, 0.82f, 0.75f,
    0.50f, 0.50f, 0.50f,
    1.0f, 0.09f, 0.032f,
    2
);

PointLight pointlight3(
    pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z,
    0.15f, 0.15f, 0.13f,
    0.85f, 0.82f, 0.75f,
    0.50f, 0.50f, 0.50f,
    1.0f, 0.09f, 0.032f,
    3
);

PointLight pointlight4(
    pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z,
    0.15f, 0.15f, 0.13f,
    0.85f, 0.82f, 0.75f,
    0.50f, 0.50f, 0.50f,
    1.0f, 0.09f, 0.032f,
    4
);

bool pointLightOn = true;

// Blackboard Spotlight: Mounted at ceiling, pointed directly towards the chalkboard
SpotLight blackboardSpotlight(
    0.0f, 3.85f, -3.2f,        // position
    0.0f, -0.45f, -0.89f,      // direction vector towards the blackboard center
    18.0f, 26.0f,              // inner and outer cutoff angles
    0.05f, 0.05f, 0.05f,       // ambient
    1.0f, 0.96f, 0.88f,        // diffuse (bright spotlight beam)
    0.9f, 0.9f, 0.9f,          // specular
    1.0f, 0.07f, 0.017f,       // attenuation
    1
);

// Timing variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// =========================================================================
// Main Entry Point
// =========================================================================
int main()
{
    // 1. Initialize and configure GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 2. Create GLFW Window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, 
        "CSE 444: 3D Classroom Model with Shading, Animation & Multi-Light", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // 3. Load OpenGL function pointers via GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Configure global OpenGL depth state
    glEnable(GL_DEPTH_TEST);

    // 4. Build and compile Shader programs
    // Uses Phong shading for smooth specular highlights and spotlight cones
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    // 5. Unit Cube Vertex Data (Positions & Surface Normals)
    float cube_vertices[] = {
        // positions          // normals
        // Back Face
        0.0f, 0.0f, 0.0f,     0.0f,  0.0f, -1.0f,
        1.0f, 0.0f, 0.0f,     0.0f,  0.0f, -1.0f,
        1.0f, 1.0f, 0.0f,     0.0f,  0.0f, -1.0f,
        0.0f, 1.0f, 0.0f,     0.0f,  0.0f, -1.0f,

        // Right Face
        1.0f, 0.0f, 0.0f,     1.0f,  0.0f,  0.0f,
        1.0f, 1.0f, 0.0f,     1.0f,  0.0f,  0.0f,
        1.0f, 0.0f, 1.0f,     1.0f,  0.0f,  0.0f,
        1.0f, 1.0f, 1.0f,     1.0f,  0.0f,  0.0f,

        // Front Face
        0.0f, 0.0f, 1.0f,     0.0f,  0.0f,  1.0f,
        1.0f, 0.0f, 1.0f,     0.0f,  0.0f,  1.0f,
        1.0f, 1.0f, 1.0f,     0.0f,  0.0f,  1.0f,
        0.0f, 1.0f, 1.0f,     0.0f,  0.0f,  1.0f,

        // Left Face
        0.0f, 0.0f, 1.0f,    -1.0f,  0.0f,  0.0f,
        0.0f, 1.0f, 1.0f,    -1.0f,  0.0f,  0.0f,
        0.0f, 1.0f, 0.0f,    -1.0f,  0.0f,  0.0f,
        0.0f, 0.0f, 0.0f,    -1.0f,  0.0f,  0.0f,

        // Top Face
        1.0f, 1.0f, 1.0f,     0.0f,  1.0f,  0.0f,
        1.0f, 1.0f, 0.0f,     0.0f,  1.0f,  0.0f,
        0.0f, 1.0f, 0.0f,     0.0f,  1.0f,  0.0f,
        0.0f, 1.0f, 1.0f,     0.0f,  1.0f,  0.0f,

        // Bottom Face
        0.0f, 0.0f, 0.0f,     0.0f, -1.0f,  0.0f,
        1.0f, 0.0f, 0.0f,     0.0f, -1.0f,  0.0f,
        1.0f, 0.0f, 1.0f,     0.0f, -1.0f,  0.0f,
        0.0f, 0.0f, 1.0f,     0.0f, -1.0f,  0.0f
    };

    unsigned int cube_indices[] = {
        0, 3, 2,  2, 1, 0,       // back
        4, 5, 7,  7, 6, 4,       // right
        8, 9, 10, 10, 11, 8,     // front
        12, 13, 14, 14, 15, 12,  // left
        16, 17, 18, 18, 19, 16,  // top
        20, 21, 22, 22, 23, 20   // bottom
    };

    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // Position attribute (layout location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute (layout location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Initial camera view
    setCameraPreset(VIEW_BACK);

    cout << "==========================================================" << endl;
    cout << " 3D CLASSROOM - EASY 4-VIEW & INTERACTION CONTROLS" << endl;
    cout << "==========================================================" << endl;
    cout << " The 4 Core Views (Matching Classroom.jpg Reference):" << endl;
    cout << "   [1] or [B]           : View 1 - BACK VIEW (Initial Main Perspective)" << endl;
    cout << "   [2] or [G]           : View 2 - SIDE VIEW (Profile of Desks & Windows)" << endl;
    cout << "   [3] or [T]           : View 3 - TOP VIEW (Overhead Layout Arrangement)" << endl;
    cout << "   [4] or [F]           : View 4 - TEACHER VIEW (Podium looking at students)" << endl;
    cout << "   [0] or [Home]        : Reset to View 1 (Initial View)" << endl;
    cout << "   [V] or [Tab]         : Cycle through all 4 views (1 -> 2 -> 3 -> 4)" << endl;
    cout << endl;
    cout << " Camera Navigation:" << endl;
    cout << "   [Up / Down Arrow]    : Move Forward / Backward" << endl;
    cout << "   [Left / Right Arrow] : Turn Left / Turn Right (Smooth Look)" << endl;
    cout << "   [Shift + Left/Right] : Slide / Strafe Left / Right" << endl;
    cout << "   [W, A, S, D]         : Standard Move (Forward, Back, Left, Right)" << endl;
    cout << "   [Shift + Up/Down]    : Move Camera Height Up / Down" << endl;
    cout << "   [Mouse Drag]         : Free Look around" << endl;
    cout << endl;
    cout << " Moving Objects & Interactive Features:" << endl;
    cout << "   [O]                  : Open / Close Classroom Door (Animated!)" << endl;
    cout << "   [Clock Second Hand]  : Rotates Clockwise continuously" << endl;
    cout << "   [SPACE]              : Toggle Ceiling Fan On / Off" << endl;
    cout << "   [+] / [-]            : Increase / Decrease Fan Speed" << endl;
    cout << "   [L] or [8]           : Toggle Point Lights (Ceiling Lamps)" << endl;
    cout << "   [K] or [9]           : Toggle Spotlight (Blackboard Lamp)" << endl;
    cout << "==========================================================" << endl;

    // ---------------------------------------------------------------------
    // Main Render Loop
    // ---------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // Per-frame timing calculation
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process user input
        processInput(window);

        // 1. Update Moving Object: Ceiling Fan Rotation
        if (isFanOn) {
            fanAngle += fanSpeed * deltaTime;
            if (fanAngle >= 360.0f) {
                fanAngle -= 360.0f;
            }
        }

        // 2. Update Moving Object: Wall Clock Second Hand (Rotates CLOCKWISE around Z)
        clockSecondAngle -= clockSpeed * deltaTime;
        if (clockSecondAngle <= -360.0f) {
            clockSecondAngle += 360.0f;
        }

        // 3. Update Moving Object: Door Open / Close Smooth Transition
        targetDoorAngle = isDoorOpen ? 85.0f : 0.0f;
        doorAngle += (targetDoorAngle - doorAngle) * 5.0f * deltaTime;

        // 4. Update Moving Object: 3D Robot Hand Waving (Continuous "Bye-Bye" Motion)
        robotWaveAngle = sin(currentFrame * 5.5f) * 28.0f;

        // Render pass setup
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate lighting shader and configure camera/eye position
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        // Set up Point Lights in the shader
        pointlight1.setUpPointLight(lightingShader);
        pointlight2.setUpPointLight(lightingShader);
        pointlight3.setUpPointLight(lightingShader);
        pointlight4.setUpPointLight(lightingShader);

        // Set up Spotlight in the shader
        blackboardSpotlight.setUpSpotLight(lightingShader);

        // Viewing Transformation (Projection and View Matrices)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        lightingShader.use();
        lightingShader.setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("view", view);

        // Also configure ourShader matrices for emissive/unlit components (sky, LEDs, lights)
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // Global Classroom 3D Modeling Transformation
        // (Allows interactive rotation, translation, and scaling of the whole room)
        glm::mat4 identityMatrix = glm::mat4(1.0f);
        glm::mat4 translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        glm::mat4 rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 scaleMatrix   = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));

        glm::mat4 roomBase = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

        // -----------------------------------------------------------------
        // Draw Classroom Components
        // -----------------------------------------------------------------

        // 1. Floor, Walls, and Ceiling Shell
        drawFloor(cubeVAO, lightingShader, roomBase);
        drawWallsAndCeiling(cubeVAO, lightingShader, roomBase);

        // 2. Blackboard & Wall Clock on the front wall (with clockwise second hand)
        drawBlackboard(cubeVAO, lightingShader, roomBase);
        drawWallClock(cubeVAO, lightingShader, roomBase, clockSecondAngle);

        // 3. Windows (Left Wall with Outdoor View) & Door (Right Wall with Open/Close, Color Shift & Hallway)
        drawWindow(cubeVAO, lightingShader, ourShader, roomBase);
        drawDoor(cubeVAO, lightingShader, ourShader, roomBase, doorAngle);

        // 4. Teacher's Podium & Laptop (Front of classroom)
        glm::mat4 podiumModel = glm::translate(roomBase, glm::vec3(0.0f, 0.0f, -4.2f));
        drawTeacherPodium(cubeVAO, lightingShader, podiumModel);

        // 5. Student Desks & Chairs (2 Columns x 4 Rows = 8 sets with center aisle)
        float colX[2] = { -2.3f, 2.3f };
        float rowZ[4] = { -2.0f, -0.6f, 0.8f, 2.2f };

        for (int c = 0; c < 2; ++c) {
            for (int r = 0; r < 4; ++r) {
                // Desk
                glm::mat4 deskMat = glm::translate(roomBase, glm::vec3(colX[c], 0.0f, rowZ[r]));
                drawStudentDesk(cubeVAO, lightingShader, deskMat);

                // Matching Chair directly behind the desk
                glm::mat4 chairMat = glm::translate(roomBase, glm::vec3(colX[c], 0.0f, rowZ[r] + 0.52f));
                drawStudentChair(cubeVAO, lightingShader, chairMat);
            }
        }

        // 6. Moving Object: Rotating Ceiling Fan
        glm::mat4 fanMat = glm::translate(roomBase, glm::vec3(0.0f, 3.95f, -0.5f));
        drawCeilingFan(cubeVAO, lightingShader, fanMat, fanAngle);

        // 7. Lighting Fixtures
        // Rectangular ceiling point-light fixtures
        for (int i = 0; i < 4; ++i) {
            glm::mat4 fixtureMat = glm::translate(roomBase, pointLightPositions[i]);
            drawCeilingLightFixture(cubeVAO, lightingShader, ourShader, fixtureMat, pointLightOn);
        }

        // Directed spotlight fixture
        glm::mat4 spotFixtureMat = glm::translate(roomBase, blackboardSpotlight.position);
        drawSpotlightFixture(cubeVAO, lightingShader, ourShader, spotFixtureMat, blackboardSpotlight.isOn);

        // 8. Moving Object: 3D Classroom Robot beside Blackboard (Waving Bye-Bye Hand!)
        glm::mat4 robotMat = glm::translate(roomBase, glm::vec3(3.2f, 0.0f, -5.2f));
        drawRobot(cubeVAO, lightingShader, ourShader, robotMat, robotWaveAngle);

        // Swap buffers and poll input events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup resources
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    glfwTerminate();
    return 0;
}

// =========================================================================
// Drawing Functions: Modular Hierarchical Cube-based Models
// =========================================================================

// Base Cube Drawing Helper: Binds material properties and draws a transformed unit cube
void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model, 
              float r, float g, float b, float spec, float shininess)
{
    lightingShader.use();
    lightingShader.setVec3("material.ambient", glm::vec3(r, g, b));
    lightingShader.setVec3("material.diffuse", glm::vec3(r, g, b));
    lightingShader.setVec3("material.specular", glm::vec3(spec, spec, spec));
    lightingShader.setFloat("material.shininess", shininess);
    lightingShader.setMat4("model", model);

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

// 1. Classroom Floor (Light Gray Tile Appearance)
void drawFloor(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 roomBase)
{
    // Floor slab: width = 10.0, length = 12.0, thickness = 0.1
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-5.0f, -0.1f, -6.0f));
    model = glm::scale(model, glm::vec3(10.0f, 0.1f, 12.0f));
    drawCube(cubeVAO, lightingShader, roomBase * model, 0.82f, 0.82f, 0.84f, 0.4f, 64.0f);
}

// 2. Classroom Walls and Ceiling
void drawWallsAndCeiling(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 roomBase)
{
    // Wall color: Warm Cream / Off-White
    float wr = 0.92f, wg = 0.90f, wb = 0.86f;

    // --- FRONT WALL (Mounts the Blackboard and Clock, Z = -6.0) ---
    glm::mat4 frontWall = glm::mat4(1.0f);
    frontWall = glm::translate(frontWall, glm::vec3(-5.0f, 0.0f, -6.1f));
    frontWall = glm::scale(frontWall, glm::vec3(10.0f, 4.0f, 0.1f));
    drawCube(cubeVAO, lightingShader, roomBase * frontWall, wr, wg, wb);

    // --- BACK WALL (Behind Students, Z = +6.0) ---
    glm::mat4 backWall = glm::mat4(1.0f);
    backWall = glm::translate(backWall, glm::vec3(-5.0f, 0.0f, 6.0f));
    backWall = glm::scale(backWall, glm::vec3(10.0f, 4.0f, 0.1f));
    drawCube(cubeVAO, lightingShader, roomBase * backWall, wr, wg, wb);

    // --- LEFT WALL SECTIONS (Enclosing the 2 Windows, X = -5.0) ---
    // Bottom sill wall section
    glm::mat4 leftWallBottom = glm::mat4(1.0f);
    leftWallBottom = glm::translate(leftWallBottom, glm::vec3(-5.1f, 0.0f, -6.0f));
    leftWallBottom = glm::scale(leftWallBottom, glm::vec3(0.1f, 1.4f, 12.0f));
    drawCube(cubeVAO, lightingShader, roomBase * leftWallBottom, wr, wg, wb);

    // Top lintel wall section above windows
    glm::mat4 leftWallTop = glm::mat4(1.0f);
    leftWallTop = glm::translate(leftWallTop, glm::vec3(-5.1f, 3.2f, -6.0f));
    leftWallTop = glm::scale(leftWallTop, glm::vec3(0.1f, 0.8f, 12.0f));
    drawCube(cubeVAO, lightingShader, roomBase * leftWallTop, wr, wg, wb);

    // Vertical wall pillars framing windows
    // Front corner pillar
    glm::mat4 leftPillar1 = glm::mat4(1.0f);
    leftPillar1 = glm::translate(leftPillar1, glm::vec3(-5.1f, 1.4f, -6.0f));
    leftPillar1 = glm::scale(leftPillar1, glm::vec3(0.1f, 1.8f, 1.2f));
    drawCube(cubeVAO, lightingShader, roomBase * leftPillar1, wr, wg, wb);

    // Middle pillar between Window 1 and Window 2
    glm::mat4 leftPillar2 = glm::mat4(1.0f);
    leftPillar2 = glm::translate(leftPillar2, glm::vec3(-5.1f, 1.4f, -2.4f));
    leftPillar2 = glm::scale(leftPillar2, glm::vec3(0.1f, 1.8f, 1.4f));
    drawCube(cubeVAO, lightingShader, roomBase * leftPillar2, wr, wg, wb);

    // Back pillar
    glm::mat4 leftPillar3 = glm::mat4(1.0f);
    leftPillar3 = glm::translate(leftPillar3, glm::vec3(-5.1f, 1.4f, 1.4f));
    leftPillar3 = glm::scale(leftPillar3, glm::vec3(0.1f, 1.8f, 4.6f));
    drawCube(cubeVAO, lightingShader, roomBase * leftPillar3, wr, wg, wb);

    // --- RIGHT WALL (With Door Cutout, X = +5.0) ---
    // Front section before door
    glm::mat4 rightFront = glm::mat4(1.0f);
    rightFront = glm::translate(rightFront, glm::vec3(5.0f, 0.0f, -6.0f));
    rightFront = glm::scale(rightFront, glm::vec3(0.1f, 4.0f, 2.0f));
    drawCube(cubeVAO, lightingShader, roomBase * rightFront, wr, wg, wb);

    // Section above door
    glm::mat4 rightAboveDoor = glm::mat4(1.0f);
    rightAboveDoor = glm::translate(rightAboveDoor, glm::vec3(5.0f, 2.8f, -4.0f));
    rightAboveDoor = glm::scale(rightAboveDoor, glm::vec3(0.1f, 1.2f, 1.5f));
    drawCube(cubeVAO, lightingShader, roomBase * rightAboveDoor, wr, wg, wb);

    // Back section after door
    glm::mat4 rightBack = glm::mat4(1.0f);
    rightBack = glm::translate(rightBack, glm::vec3(5.0f, 0.0f, -2.5f));
    rightBack = glm::scale(rightBack, glm::vec3(0.1f, 4.0f, 8.5f));
    drawCube(cubeVAO, lightingShader, roomBase * rightBack, wr, wg, wb);

    // --- CEILING ---
    glm::mat4 ceiling = glm::mat4(1.0f);
    ceiling = glm::translate(ceiling, glm::vec3(-5.0f, 4.0f, -6.0f));
    ceiling = glm::scale(ceiling, glm::vec3(10.0f, 0.1f, 12.0f));
    drawCube(cubeVAO, lightingShader, roomBase * ceiling, 0.94f, 0.94f, 0.94f);
}

// 3. Student Desk (Warm Honey Wooden Top, Dark Charcoal Legs & Under-desk Apron)
void drawStudentDesk(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 deskBase)
{
    float topWidth = 1.15f;
    float topLength = 0.60f;
    float topThickness = 0.05f;
    float deskHeight = 0.72f;

    float legThick = 0.055f;
    float legHeight = deskHeight - topThickness;

    // Dark charcoal leg & frame color
    float lr = 0.16f, lg = 0.15f, lb = 0.14f;
    // Warm wood top color
    float tr = 0.82f, tg = 0.52f, tb = 0.28f;

    // 4 Legs
    float halfW = topWidth / 2.0f - 0.04f;
    float halfL = topLength / 2.0f - 0.04f;
    float legX[4] = { -halfW, halfW - legThick, -halfW, halfW - legThick };
    float legZ[4] = { -halfL, -halfL, halfL - legThick, halfL - legThick };

    for (int i = 0; i < 4; ++i) {
        glm::mat4 leg = glm::mat4(1.0f);
        leg = glm::translate(leg, glm::vec3(legX[i], 0.0f, legZ[i]));
        leg = glm::scale(leg, glm::vec3(legThick, legHeight, legThick));
        drawCube(cubeVAO, lightingShader, deskBase * leg, lr, lg, lb);
    }

    // Under-table structural apron rails
    glm::mat4 apronFront = glm::mat4(1.0f);
    apronFront = glm::translate(apronFront, glm::vec3(-halfW, legHeight - 0.06f, -halfL));
    apronFront = glm::scale(apronFront, glm::vec3(2 * halfW, 0.06f, 0.03f));
    drawCube(cubeVAO, lightingShader, deskBase * apronFront, lr, lg, lb);

    // Wooden Tabletop
    glm::mat4 top = glm::mat4(1.0f);
    top = glm::translate(top, glm::vec3(-topWidth / 2.0f, legHeight, -topLength / 2.0f));
    top = glm::scale(top, glm::vec3(topWidth, topThickness, topLength));
    drawCube(cubeVAO, lightingShader, deskBase * top, tr, tg, tb, 0.4f, 48.0f);

    // Small pencil case / notebook accessory on desk
    glm::mat4 book = glm::mat4(1.0f);
    book = glm::translate(book, glm::vec3(-0.25f, deskHeight, -0.1f));
    book = glm::scale(book, glm::vec3(0.25f, 0.015f, 0.18f));
    drawCube(cubeVAO, lightingShader, deskBase * book, 0.35f, 0.35f, 0.40f);
}

// 4. Student Chair (Warm Honey Wooden Seat & Backrest, Dark Legs)
void drawStudentChair(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 chairBase)
{
    float seatW = 0.46f;
    float seatL = 0.42f;
    float seatThick = 0.04f;
    float seatHeight = 0.44f;

    float legThick = 0.045f;
    float legH = seatHeight - seatThick;

    float lr = 0.16f, lg = 0.15f, lb = 0.14f; // Legs
    float wr = 0.82f, wg = 0.52f, wb = 0.28f; // Wood

    // 4 Legs
    float hw = seatW / 2.0f - 0.03f;
    float hl = seatL / 2.0f - 0.03f;
    float legX[4] = { -hw, hw - legThick, -hw, hw - legThick };
    float legZ[4] = { -hl, -hl, hl - legThick, hl - legThick };

    for (int i = 0; i < 4; ++i) {
        glm::mat4 leg = glm::mat4(1.0f);
        leg = glm::translate(leg, glm::vec3(legX[i], 0.0f, legZ[i]));
        leg = glm::scale(leg, glm::vec3(legThick, legH, legThick));
        drawCube(cubeVAO, lightingShader, chairBase * leg, lr, lg, lb);
    }

    // Chair Seat
    glm::mat4 seat = glm::mat4(1.0f);
    seat = glm::translate(seat, glm::vec3(-seatW / 2.0f, legH, -seatL / 2.0f));
    seat = glm::scale(seat, glm::vec3(seatW, seatThick, seatL));
    drawCube(cubeVAO, lightingShader, chairBase * seat, wr, wg, wb);

    // Backrest Upright Posts (Rising from the back edge of the seat)
    float postH = 0.40f;
    glm::mat4 postLeft = glm::mat4(1.0f);
    postLeft = glm::translate(postLeft, glm::vec3(-hw, seatHeight, hl - legThick));
    postLeft = glm::scale(postLeft, glm::vec3(legThick, postH, legThick));
    drawCube(cubeVAO, lightingShader, chairBase * postLeft, lr, lg, lb);

    glm::mat4 postRight = glm::mat4(1.0f);
    postRight = glm::translate(postRight, glm::vec3(hw - legThick, seatHeight, hl - legThick));
    postRight = glm::scale(postRight, glm::vec3(legThick, postH, legThick));
    drawCube(cubeVAO, lightingShader, chairBase * postRight, lr, lg, lb);

    // Backrest Panel
    glm::mat4 backPanel = glm::mat4(1.0f);
    backPanel = glm::translate(backPanel, glm::vec3(-seatW / 2.0f, seatHeight + postH - 0.22f, hl - 0.035f));
    backPanel = glm::scale(backPanel, glm::vec3(seatW, 0.20f, 0.035f));
    drawCube(cubeVAO, lightingShader, chairBase * backPanel, wr, wg, wb);
}

// 5. Teacher's Podium with Laptop/Monitor
void drawTeacherPodium(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 podiumBase)
{
    float podW = 2.0f;
    float podD = 0.9f;
    float podH = 0.85f;

    float pr = 0.70f, pg = 0.40f, pb = 0.22f; // Wood body

    // Front Modesty Panel (Facing the students)
    glm::mat4 frontPanel = glm::mat4(1.0f);
    frontPanel = glm::translate(frontPanel, glm::vec3(-podW / 2.0f, 0.0f, podD / 2.0f - 0.05f));
    frontPanel = glm::scale(frontPanel, glm::vec3(podW, podH, 0.05f));
    drawCube(cubeVAO, lightingShader, podiumBase * frontPanel, pr, pg, pb);

    // Left Side Panel
    glm::mat4 leftSide = glm::mat4(1.0f);
    leftSide = glm::translate(leftSide, glm::vec3(-podW / 2.0f, 0.0f, -podD / 2.0f));
    leftSide = glm::scale(leftSide, glm::vec3(0.05f, podH, podD));
    drawCube(cubeVAO, lightingShader, podiumBase * leftSide, pr, pg, pb);

    // Right Side Panel
    glm::mat4 rightSide = glm::mat4(1.0f);
    rightSide = glm::translate(rightSide, glm::vec3(podW / 2.0f - 0.05f, 0.0f, -podD / 2.0f));
    rightSide = glm::scale(rightSide, glm::vec3(0.05f, podH, podD));
    drawCube(cubeVAO, lightingShader, podiumBase * rightSide, pr, pg, pb);

    // Desktop Surface (Slight overhang)
    glm::mat4 top = glm::mat4(1.0f);
    top = glm::translate(top, glm::vec3(-podW / 2.0f - 0.05f, podH, -podD / 2.0f - 0.05f));
    top = glm::scale(top, glm::vec3(podW + 0.1f, 0.06f, podD + 0.1f));
    drawCube(cubeVAO, lightingShader, podiumBase * top, 0.76f, 0.46f, 0.25f, 0.4f, 48.0f);

    // --- Laptop / Monitor on Teacher's Desk ---
    float mr = 0.15f, mg = 0.15f, mb = 0.16f; // Dark plastic
    // Base
    glm::mat4 lapBase = glm::mat4(1.0f);
    lapBase = glm::translate(lapBase, glm::vec3(-0.25f, podH + 0.06f, -0.15f));
    lapBase = glm::scale(lapBase, glm::vec3(0.50f, 0.02f, 0.35f));
    drawCube(cubeVAO, lightingShader, podiumBase * lapBase, mr, mg, mb);

    // Angled Monitor Screen
    glm::mat4 lapScreen = glm::mat4(1.0f);
    lapScreen = glm::translate(lapScreen, glm::vec3(-0.25f, podH + 0.08f, -0.15f));
    lapScreen = glm::rotate(lapScreen, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    lapScreen = glm::scale(lapScreen, glm::vec3(0.50f, 0.32f, 0.025f));
    drawCube(cubeVAO, lightingShader, podiumBase * lapScreen, mr, mg, mb);

    // Screen Display Surface
    glm::mat4 screenFace = glm::mat4(1.0f);
    screenFace = glm::translate(screenFace, glm::vec3(-0.23f, podH + 0.10f, -0.145f));
    screenFace = glm::rotate(screenFace, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    screenFace = glm::scale(screenFace, glm::vec3(0.46f, 0.28f, 0.02f));
    drawCube(cubeVAO, lightingShader, podiumBase * screenFace, 0.35f, 0.48f, 0.60f, 0.8f, 128.0f);

    // Teacher's Chair
    glm::mat4 tChair = glm::translate(podiumBase, glm::vec3(0.0f, 0.0f, -0.65f));
    drawStudentChair(cubeVAO, lightingShader, tChair);
}

// 6. Classroom Blackboard (Dark Green Board, Wooden Border & Chalk Tray)
void drawBlackboard(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 boardBase)
{
    float bWidth = 4.4f;
    float bHeight = 2.1f;
    float posY = 1.4f;
    float posZ = -5.98f;

    // Wooden Border Frame
    glm::mat4 frame = glm::mat4(1.0f);
    frame = glm::translate(frame, glm::vec3(-bWidth / 2.0f, posY, posZ));
    frame = glm::scale(frame, glm::vec3(bWidth, bHeight, 0.04f));
    drawCube(cubeVAO, lightingShader, boardBase * frame, 0.45f, 0.25f, 0.12f);

    // Dark Green Blackboard Surface
    float innerW = bWidth - 0.20f;
    float innerH = bHeight - 0.20f;
    glm::mat4 board = glm::mat4(1.0f);
    board = glm::translate(board, glm::vec3(-innerW / 2.0f, posY + 0.10f, posZ + 0.02f));
    board = glm::scale(board, glm::vec3(innerW, innerH, 0.03f));
    drawCube(cubeVAO, lightingShader, boardBase * board, 0.10f, 0.28f, 0.18f, 0.2f, 16.0f);

    // Chalk Shelf / Ledge along the bottom
    glm::mat4 ledge = glm::mat4(1.0f);
    ledge = glm::translate(ledge, glm::vec3(-innerW / 2.0f, posY + 0.06f, posZ + 0.04f));
    ledge = glm::scale(ledge, glm::vec3(innerW, 0.04f, 0.10f));
    drawCube(cubeVAO, lightingShader, boardBase * ledge, 0.45f, 0.25f, 0.12f);

    // Wooden Chalk Eraser
    glm::mat4 eraser = glm::mat4(1.0f);
    eraser = glm::translate(eraser, glm::vec3(0.2f, posY + 0.10f, posZ + 0.06f));
    eraser = glm::scale(eraser, glm::vec3(0.18f, 0.035f, 0.06f));
    drawCube(cubeVAO, lightingShader, boardBase * eraser, 0.60f, 0.35f, 0.15f);

    // Chalk Pieces
    glm::mat4 chalk = glm::mat4(1.0f);
    chalk = glm::translate(chalk, glm::vec3(-0.3f, posY + 0.10f, posZ + 0.07f));
    chalk = glm::scale(chalk, glm::vec3(0.08f, 0.02f, 0.02f));
    drawCube(cubeVAO, lightingShader, boardBase * chalk, 0.98f, 0.98f, 0.98f);
}

// 7. Wall Clock with Animated Clockwise Second Hand
void drawWallClock(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 clockBase, float clockSecondAngle)
{
    float posX = 3.2f;
    float posY = 2.6f;
    float posZ = -5.98f;
    float size = 0.70f;

    // Square Outer Wooden Frame
    glm::mat4 frame = glm::mat4(1.0f);
    frame = glm::translate(frame, glm::vec3(posX - size / 2.0f, posY - size / 2.0f, posZ));
    frame = glm::scale(frame, glm::vec3(size, size, 0.04f));
    drawCube(cubeVAO, lightingShader, clockBase * frame, 0.42f, 0.24f, 0.12f);

    // Inner White Clock Face
    float faceSize = size - 0.14f;
    glm::mat4 face = glm::mat4(1.0f);
    face = glm::translate(face, glm::vec3(posX - faceSize / 2.0f, posY - faceSize / 2.0f, posZ + 0.02f));
    face = glm::scale(face, glm::vec3(faceSize, faceSize, 0.03f));
    drawCube(cubeVAO, lightingShader, clockBase * face, 0.95f, 0.95f, 0.92f);

    // Center Black Pin
    glm::mat4 pin = glm::mat4(1.0f);
    pin = glm::translate(pin, glm::vec3(posX - 0.02f, posY - 0.02f, posZ + 0.045f));
    pin = glm::scale(pin, glm::vec3(0.04f, 0.04f, 0.02f));
    drawCube(cubeVAO, lightingShader, clockBase * pin, 0.05f, 0.05f, 0.05f);

    // Black Clock Hands (Displaying 3:00 / "L" pattern as in Classroom.jpg)
    // Minute Hand (Pointing up to 12)
    glm::mat4 minHand = glm::mat4(1.0f);
    minHand = glm::translate(minHand, glm::vec3(posX - 0.015f, posY, posZ + 0.04f));
    minHand = glm::scale(minHand, glm::vec3(0.03f, 0.18f, 0.015f));
    drawCube(cubeVAO, lightingShader, clockBase * minHand, 0.05f, 0.05f, 0.05f);

    // Hour Hand (Pointing right to 3)
    glm::mat4 hourHand = glm::mat4(1.0f);
    hourHand = glm::translate(hourHand, glm::vec3(posX, posY - 0.015f, posZ + 0.04f));
    hourHand = glm::scale(hourHand, glm::vec3(0.13f, 0.03f, 0.015f));
    drawCube(cubeVAO, lightingShader, clockBase * hourHand, 0.05f, 0.05f, 0.05f);

    // Rotating Second Hand (Bright red needle rotating CLOCKWISE around Z-axis)
    glm::mat4 secHandPivot = glm::translate(clockBase, glm::vec3(posX, posY, posZ + 0.055f));
    secHandPivot = glm::rotate(secHandPivot, glm::radians(clockSecondAngle), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 secHand = glm::mat4(1.0f);
    secHand = glm::translate(secHand, glm::vec3(-0.007f, -0.04f, 0.0f)); // slight tail behind center pivot
    secHand = glm::scale(secHand, glm::vec3(0.014f, 0.23f, 0.014f));
    drawCube(cubeVAO, lightingShader, secHandPivot * secHand, 0.88f, 0.12f, 0.12f);
}

// 8. Requirement 3: Moving Object - Rotating Ceiling Fan
void drawCeilingFan(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 fanBase, float fanAngle)
{
    float fr = 0.24f, fg = 0.16f, fb = 0.10f; // Deep mahogany brown

    // Ceiling Mount Canopy
    glm::mat4 canopy = glm::mat4(1.0f);
    canopy = glm::translate(canopy, glm::vec3(-0.10f, -0.05f, -0.10f));
    canopy = glm::scale(canopy, glm::vec3(0.20f, 0.05f, 0.20f));
    drawCube(cubeVAO, lightingShader, fanBase * canopy, fr, fg, fb);

    // Stationary Downrod Hanging from Ceiling
    glm::mat4 rod = glm::mat4(1.0f);
    rod = glm::translate(rod, glm::vec3(-0.035f, -0.45f, -0.035f));
    rod = glm::scale(rod, glm::vec3(0.07f, 0.40f, 0.07f));
    drawCube(cubeVAO, lightingShader, fanBase * rod, fr, fg, fb);

    // --- ROTATING ROTOR & BLADES ---
    // Rotates continuously about the vertical Y-axis
    glm::mat4 rotorMatrix = glm::translate(fanBase, glm::vec3(0.0f, -0.52f, 0.0f));
    rotorMatrix = glm::rotate(rotorMatrix, glm::radians(fanAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    // Central Motor Hub
    glm::mat4 hub = glm::mat4(1.0f);
    hub = glm::translate(hub, glm::vec3(-0.16f, -0.06f, -0.16f));
    hub = glm::scale(hub, glm::vec3(0.32f, 0.12f, 0.32f));
    drawCube(cubeVAO, lightingShader, rotorMatrix * hub, fr, fg, fb);

    // 4 Symmetrical Fan Blades extending radially at 90-degree offsets
    float bladeLength = 1.15f;
    float bladeWidth = 0.18f;
    float bladeThick = 0.02f;

    for (int i = 0; i < 4; ++i) {
        glm::mat4 bladeMat = glm::rotate(rotorMatrix, glm::radians(i * 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 blade = glm::mat4(1.0f);
        blade = glm::translate(blade, glm::vec3(0.16f, -0.02f, -bladeWidth / 2.0f));
        blade = glm::scale(blade, glm::vec3(bladeLength, bladeThick, bladeWidth));
        drawCube(cubeVAO, lightingShader, bladeMat * blade, 0.28f, 0.18f, 0.12f, 0.4f, 32.0f);
    }
}

// 9. Windows on Left Wall (Realistic Casing, Interior Sills, Sashes, Panes & Outdoor Scenery)
void drawWindow(unsigned int& cubeVAO, Shader& lightingShader, Shader& ourShader, glm::mat4 windowBase)
{
    float winZ[2] = { -4.8f, -1.0f }; // Start Z coordinates of the 2 window openings
    float winW = 2.4f;                 // Width of each window
    float winH = 1.8f;                 // Height of each window
    float winY = 1.4f;                 // Bottom sill Y level
    float winX = -5.0f;                // Wall X plane

    // Colors
    float fr = 0.94f, fg = 0.94f, fb = 0.95f; // Crisp white window frame & sashes
    float sr = 0.88f, sg = 0.85f, sb = 0.80f; // Light polished stone/wood interior sill
    float gr = 0.70f, gg = 0.86f, gb = 0.95f; // Daylight glass

    // -------------------------------------------------------------
    // A. Outdoor Scenery Backdrop (Outside the windows at X = -5.8f to -6.2f)
    // -------------------------------------------------------------
    // 1. Bright Sunny Blue Sky Panel (Rendered with unlit ourShader so it glows with daylight)
    ourShader.use();
    glm::mat4 sky = glm::mat4(1.0f);
    sky = glm::translate(sky, glm::vec3(-5.8f, 1.2f, -6.0f));
    sky = glm::scale(sky, glm::vec3(0.05f, 3.8f, 8.5f));
    ourShader.setMat4("model", windowBase * sky);
    ourShader.setVec3("color", glm::vec3(0.56f, 0.80f, 0.98f)); // Vivid sunny sky blue
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 2. Distant Sun / Horizon Glow
    glm::mat4 sun = glm::mat4(1.0f);
    sun = glm::translate(sun, glm::vec3(-5.75f, 3.2f, -4.5f));
    sun = glm::scale(sun, glm::vec3(0.04f, 1.2f, 2.0f));
    ourShader.setMat4("model", windowBase * sun);
    ourShader.setVec3("color", glm::vec3(1.0f, 0.98f, 0.85f)); // Warm daylight sun glow
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 3. Outdoor Ground Lawn / Grass (Positioned completely outside at X <= -5.35f to eliminate z-fighting)
    lightingShader.use();
    glm::mat4 grass = glm::mat4(1.0f);
    grass = glm::translate(grass, glm::vec3(-6.8f, 0.0f, -6.0f));
    grass = glm::scale(grass, glm::vec3(1.45f, 1.35f, 8.5f));
    drawCube(cubeVAO, lightingShader, windowBase * grass, 0.28f, 0.58f, 0.24f);

    // 4. Distant Trees / Foliage outside Window 1 & Window 2
    // Tree 1 (Outside Window 1)
    glm::mat4 trunk1 = glm::mat4(1.0f);
    trunk1 = glm::translate(trunk1, glm::vec3(-5.65f, 1.2f, -4.0f));
    trunk1 = glm::scale(trunk1, glm::vec3(0.08f, 0.7f, 0.16f));
    drawCube(cubeVAO, lightingShader, windowBase * trunk1, 0.35f, 0.22f, 0.14f);

    glm::mat4 foliage1 = glm::mat4(1.0f);
    foliage1 = glm::translate(foliage1, glm::vec3(-5.68f, 1.7f, -4.5f));
    foliage1 = glm::scale(foliage1, glm::vec3(0.12f, 1.1f, 1.2f));
    drawCube(cubeVAO, lightingShader, windowBase * foliage1, 0.22f, 0.52f, 0.20f);

    // Tree 2 (Outside Window 2)
    glm::mat4 trunk2 = glm::mat4(1.0f);
    trunk2 = glm::translate(trunk2, glm::vec3(-5.65f, 1.2f, 0.1f));
    trunk2 = glm::scale(trunk2, glm::vec3(0.08f, 0.7f, 0.16f));
    drawCube(cubeVAO, lightingShader, windowBase * trunk2, 0.35f, 0.22f, 0.14f);

    glm::mat4 foliage2 = glm::mat4(1.0f);
    foliage2 = glm::translate(foliage2, glm::vec3(-5.68f, 1.6f, -0.4f));
    foliage2 = glm::scale(foliage2, glm::vec3(0.12f, 1.2f, 1.3f));
    drawCube(cubeVAO, lightingShader, windowBase * foliage2, 0.18f, 0.48f, 0.18f);

    // -------------------------------------------------------------
    // B. Architectural Window Casings, Sills, and Glass Panes
    // -------------------------------------------------------------
    for (int i = 0; i < 2; ++i) {
        float z0 = winZ[i];

        // 1. Prominent Interior Window Sill Ledge (Protrudes nicely into the room)
        glm::mat4 sill = glm::mat4(1.0f);
        sill = glm::translate(sill, glm::vec3(winX - 0.06f, winY - 0.05f, z0 - 0.08f));
        sill = glm::scale(sill, glm::vec3(0.20f, 0.06f, winW + 0.16f));
        drawCube(cubeVAO, lightingShader, windowBase * sill, sr, sg, sb, 0.5f, 64.0f);

        // 2. Outer Frame Jambs & Header (Hollow perimeter casing)
        // Bottom Rail
        glm::mat4 bRail = glm::mat4(1.0f);
        bRail = glm::translate(bRail, glm::vec3(winX - 0.05f, winY, z0));
        bRail = glm::scale(bRail, glm::vec3(0.10f, 0.05f, winW));
        drawCube(cubeVAO, lightingShader, windowBase * bRail, fr, fg, fb);

        // Top Header
        glm::mat4 tRail = glm::mat4(1.0f);
        tRail = glm::translate(tRail, glm::vec3(winX - 0.05f, winY + winH - 0.05f, z0));
        tRail = glm::scale(tRail, glm::vec3(0.10f, 0.05f, winW));
        drawCube(cubeVAO, lightingShader, windowBase * tRail, fr, fg, fb);

        // Left Stile
        glm::mat4 lStile = glm::mat4(1.0f);
        lStile = glm::translate(lStile, glm::vec3(winX - 0.05f, winY, z0));
        lStile = glm::scale(lStile, glm::vec3(0.10f, winH, 0.05f));
        drawCube(cubeVAO, lightingShader, windowBase * lStile, fr, fg, fb);

        // Right Stile
        glm::mat4 rStile = glm::mat4(1.0f);
        rStile = glm::translate(rStile, glm::vec3(winX - 0.05f, winY, z0 + winW - 0.05f));
        rStile = glm::scale(rStile, glm::vec3(0.10f, winH, 0.05f));
        drawCube(cubeVAO, lightingShader, windowBase * rStile, fr, fg, fb);

        // 3. Sashes / Dividing Grids (6 Panes per window: 2 tiers x 3 columns)
        // Horizontal Transom Bar
        glm::mat4 hTransom = glm::mat4(1.0f);
        hTransom = glm::translate(hTransom, glm::vec3(winX - 0.04f, winY + winH * 0.55f - 0.02f, z0 + 0.05f));
        hTransom = glm::scale(hTransom, glm::vec3(0.08f, 0.04f, winW - 0.10f));
        drawCube(cubeVAO, lightingShader, windowBase * hTransom, fr, fg, fb);

        // 2 Vertical Mullion Bars
        for (int m = 1; m <= 2; ++m) {
            float mullionZ = z0 + (winW / 3.0f) * m - 0.02f;
            glm::mat4 vMullion = glm::mat4(1.0f);
            vMullion = glm::translate(vMullion, glm::vec3(winX - 0.04f, winY + 0.05f, mullionZ));
            vMullion = glm::scale(vMullion, glm::vec3(0.08f, winH - 0.10f, 0.04f));
            drawCube(cubeVAO, lightingShader, windowBase * vMullion, fr, fg, fb);
        }

        // 4. Glass Pane Layer
        glm::mat4 glass = glm::mat4(1.0f);
        glass = glm::translate(glass, glm::vec3(winX - 0.01f, winY + 0.05f, z0 + 0.05f));
        glass = glm::scale(glass, glm::vec3(0.02f, winH - 0.10f, winW - 0.10f));
        drawCube(cubeVAO, lightingShader, windowBase * glass, gr, gg, gb, 0.9f, 128.0f);
    }
}

// 10. Door on Right Wall (Wood Door, Glass Panel, Handle with Open/Close, Dynamic Color & Hallway Corridor)
void drawDoor(unsigned int& cubeVAO, Shader& lightingShader, Shader& ourShader, glm::mat4 doorBase, float doorAngle)
{
    float posX = 5.0f;
    float posY = 0.0f;
    float posZ = -4.0f;  // Opening spans Z in [-4.0f, -2.5f]
    float doorW = 1.5f;
    float doorH = 2.8f;

    // -------------------------------------------------------------
    // A. School Hallway / Corridor Outside the Doorway (X in [5.05f, 7.5f])
    // -------------------------------------------------------------
    // 1. Corridor Floor (Light Tiled Hallway Slab)
    glm::mat4 hallFloor = glm::mat4(1.0f);
    hallFloor = glm::translate(hallFloor, glm::vec3(posX + 0.05f, -0.1f, -5.5f));
    hallFloor = glm::scale(hallFloor, glm::vec3(2.5f, 0.1f, 4.5f));
    drawCube(cubeVAO, lightingShader, doorBase * hallFloor, 0.86f, 0.84f, 0.80f, 0.4f, 64.0f);

    // 2. Corridor Opposite Wall (Bright illuminated school corridor wall at X = 7.5f)
    glm::mat4 hallWall = glm::mat4(1.0f);
    hallWall = glm::translate(hallWall, glm::vec3(posX + 2.5f, 0.0f, -5.5f));
    hallWall = glm::scale(hallWall, glm::vec3(0.1f, 4.0f, 4.5f));
    drawCube(cubeVAO, lightingShader, doorBase * hallWall, 0.93f, 0.91f, 0.87f);

    // 3. Corridor Side Walls (Enclosing the hallway view)
    glm::mat4 hallSide1 = glm::mat4(1.0f);
    hallSide1 = glm::translate(hallSide1, glm::vec3(posX + 0.05f, 0.0f, -5.6f));
    hallSide1 = glm::scale(hallSide1, glm::vec3(2.5f, 4.0f, 0.1f));
    drawCube(cubeVAO, lightingShader, doorBase * hallSide1, 0.90f, 0.88f, 0.84f);

    glm::mat4 hallSide2 = glm::mat4(1.0f);
    hallSide2 = glm::translate(hallSide2, glm::vec3(posX + 0.05f, 0.0f, -1.0f));
    hallSide2 = glm::scale(hallSide2, glm::vec3(2.5f, 4.0f, 0.1f));
    drawCube(cubeVAO, lightingShader, doorBase * hallSide2, 0.90f, 0.88f, 0.84f);

    // 4. Corridor Ceiling
    glm::mat4 hallCeil = glm::mat4(1.0f);
    hallCeil = glm::translate(hallCeil, glm::vec3(posX + 0.05f, 4.0f, -5.5f));
    hallCeil = glm::scale(hallCeil, glm::vec3(2.5f, 0.1f, 4.5f));
    drawCube(cubeVAO, lightingShader, doorBase * hallCeil, 0.94f, 0.94f, 0.94f);

    // 5. Hallway Warm Emissive Light Fixture (Casts light into doorway)
    ourShader.use();
    glm::mat4 hallLight = glm::mat4(1.0f);
    hallLight = glm::translate(hallLight, glm::vec3(posX + 1.25f, 3.92f, -3.25f));
    hallLight = glm::scale(hallLight, glm::vec3(0.40f, 0.06f, 0.90f));
    ourShader.setMat4("model", doorBase * hallLight);
    ourShader.setVec3("color", glm::vec3(1.0f, 0.95f, 0.72f)); // Warm hallway glow
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 6. Opposite Classroom Door across the Hallway
    lightingShader.use();
    glm::mat4 oppDoor = glm::mat4(1.0f);
    oppDoor = glm::translate(oppDoor, glm::vec3(posX + 2.45f, 0.0f, -3.9f));
    oppDoor = glm::scale(oppDoor, glm::vec3(0.04f, 2.7f, 1.3f));
    drawCube(cubeVAO, lightingShader, doorBase * oppDoor, 0.48f, 0.28f, 0.15f);

    // -------------------------------------------------------------
    // B. Door Frame Casing (Jambs & Header framing the opening cleanly)
    // -------------------------------------------------------------
    float frameThick = 0.06f;
    float frameDepth = 0.14f;
    float frameX = posX - 0.02f;
    float fr = 0.36f, fg = 0.22f, fb = 0.12f; // Dark walnut frame wood

    // Left Jamb (at Z = posZ = -4.0f)
    glm::mat4 lJamb = glm::mat4(1.0f);
    lJamb = glm::translate(lJamb, glm::vec3(frameX, posY, posZ));
    lJamb = glm::scale(lJamb, glm::vec3(frameDepth, doorH, frameThick));
    drawCube(cubeVAO, lightingShader, doorBase * lJamb, fr, fg, fb);

    // Right Jamb (at Z = posZ + doorW - frameThick = -2.56f)
    glm::mat4 rJamb = glm::mat4(1.0f);
    rJamb = glm::translate(rJamb, glm::vec3(frameX, posY, posZ + doorW - frameThick));
    rJamb = glm::scale(rJamb, glm::vec3(frameDepth, doorH, frameThick));
    drawCube(cubeVAO, lightingShader, doorBase * rJamb, fr, fg, fb);

    // Top Header (at Y = doorH - frameThick = 2.74f)
    glm::mat4 tHeader = glm::mat4(1.0f);
    tHeader = glm::translate(tHeader, glm::vec3(frameX, posY + doorH - frameThick, posZ));
    tHeader = glm::scale(tHeader, glm::vec3(frameDepth, frameThick, doorW));
    drawCube(cubeVAO, lightingShader, doorBase * tHeader, fr, fg, fb);

    // -------------------------------------------------------------
    // C. Animated Door Leaf with Dynamic Open / Close Color
    // -------------------------------------------------------------
    // Hinge pivot at Z = posZ + doorW - frameThick = -2.56f, X = posX
    float hingeZ = posZ + doorW - frameThick;
    glm::mat4 doorHingeMat = glm::translate(doorBase, glm::vec3(posX, 0.0f, hingeZ));
    doorHingeMat = glm::rotate(doorHingeMat, glm::radians(doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    // Dynamic Color Calculation:
    // When closed (0 deg): Classic deep rich mahogany classroom door
    // When open (85 deg): Illuminated warm golden honey-oak reflecting hallway daylight
    float openFactor = glm::clamp(doorAngle / 85.0f, 0.0f, 1.0f);
    glm::vec3 closedColor = glm::vec3(0.48f, 0.25f, 0.12f);
    glm::vec3 openColor   = glm::vec3(0.78f, 0.50f, 0.24f);
    glm::vec3 curDoorColor = glm::mix(closedColor, openColor, openFactor);

    float leafLen = doorW - 2.0f * frameThick; // approx 1.38f
    float leafH = doorH - frameThick - 0.01f;   // approx 2.73f
    float leafThick = 0.045f;

    // Main Door Panel
    glm::mat4 panel = glm::mat4(1.0f);
    panel = glm::translate(panel, glm::vec3(-leafThick / 2.0f, 0.01f, -leafLen));
    panel = glm::scale(panel, glm::vec3(leafThick, leafH, leafLen));
    drawCube(cubeVAO, lightingShader, doorHingeMat * panel, 
             curDoorColor.r, curDoorColor.g, curDoorColor.b, 0.4f, 32.0f);

    // Upper Glass Inspection Window with White Trim
    // Window frame
    glm::mat4 winTrim = glm::mat4(1.0f);
    winTrim = glm::translate(winTrim, glm::vec3(-leafThick / 2.0f - 0.005f, 1.40f, -leafLen * 0.65f - 0.18f));
    winTrim = glm::scale(winTrim, glm::vec3(leafThick + 0.01f, 0.70f, 0.38f));
    drawCube(cubeVAO, lightingShader, doorHingeMat * winTrim, 0.90f, 0.90f, 0.92f);

    // Window glass pane
    glm::mat4 doorGlass = glm::mat4(1.0f);
    doorGlass = glm::translate(doorGlass, glm::vec3(-leafThick / 2.0f - 0.008f, 1.45f, -leafLen * 0.65f - 0.14f));
    doorGlass = glm::scale(doorGlass, glm::vec3(leafThick + 0.016f, 0.60f, 0.30f));
    drawCube(cubeVAO, lightingShader, doorHingeMat * doorGlass, 0.55f, 0.78f, 0.92f, 0.8f, 128.0f);

    // Metallic Brass / Chrome Door Handle & Rosette Plate
    glm::mat4 handlePlate = glm::mat4(1.0f);
    handlePlate = glm::translate(handlePlate, glm::vec3(-leafThick / 2.0f - 0.015f, 1.10f, -leafLen + 0.08f));
    handlePlate = glm::scale(handlePlate, glm::vec3(leafThick + 0.03f, 0.16f, 0.06f));
    drawCube(cubeVAO, lightingShader, doorHingeMat * handlePlate, 0.82f, 0.78f, 0.40f, 0.9f, 128.0f);

    // Horizontal Lever
    glm::mat4 lever = glm::mat4(1.0f);
    lever = glm::translate(lever, glm::vec3(-leafThick / 2.0f - 0.06f, 1.16f, -leafLen + 0.08f));
    lever = glm::scale(lever, glm::vec3(leafThick + 0.12f, 0.03f, 0.12f));
    drawCube(cubeVAO, lightingShader, doorHingeMat * lever, 0.88f, 0.85f, 0.45f, 0.9f, 128.0f);
}

// 11. 3D Classroom Robot (Beside the Blackboard with Continuous Bye-Bye Waving Hand)
void drawRobot(unsigned int& cubeVAO, Shader& lightingShader, Shader& ourShader, glm::mat4 robotBase, float waveAngle)
{
    // Robot Color Palette
    float mr = 0.85f, mg = 0.88f, mb = 0.92f; // High-tech pearlescent silver/white body
    float dr = 0.22f, dg = 0.24f, db = 0.28f; // Dark graphite trim / joints
    float cr = 0.00f, cg = 0.90f, cb = 1.00f; // Electric cyan glowing visor / screen

    // 1. Mobile Wheeled / Tread Base Platform
    glm::mat4 base = glm::mat4(1.0f);
    base = glm::translate(base, glm::vec3(-0.30f, 0.0f, -0.25f));
    base = glm::scale(base, glm::vec3(0.60f, 0.12f, 0.50f));
    drawCube(cubeVAO, lightingShader, robotBase * base, dr, dg, db, 0.5f, 64.0f);

    // Tread rollers / side accent strips
    glm::mat4 leftTrack = glm::mat4(1.0f);
    leftTrack = glm::translate(leftTrack, glm::vec3(-0.33f, 0.01f, -0.27f));
    leftTrack = glm::scale(leftTrack, glm::vec3(0.08f, 0.10f, 0.54f));
    drawCube(cubeVAO, lightingShader, robotBase * leftTrack, 0.12f, 0.14f, 0.16f);

    glm::mat4 rightTrack = glm::mat4(1.0f);
    rightTrack = glm::translate(rightTrack, glm::vec3(0.25f, 0.01f, -0.27f));
    rightTrack = glm::scale(rightTrack, glm::vec3(0.08f, 0.10f, 0.54f));
    drawCube(cubeVAO, lightingShader, robotBase * rightTrack, 0.12f, 0.14f, 0.16f);

    // 2. Dual Robot Legs
    // Left Leg
    glm::mat4 lLeg = glm::mat4(1.0f);
    lLeg = glm::translate(lLeg, glm::vec3(-0.18f, 0.12f, -0.06f));
    lLeg = glm::scale(lLeg, glm::vec3(0.10f, 0.32f, 0.12f));
    drawCube(cubeVAO, lightingShader, robotBase * lLeg, dr, dg, db);

    // Right Leg
    glm::mat4 rLeg = glm::mat4(1.0f);
    rLeg = glm::translate(rLeg, glm::vec3(0.08f, 0.12f, -0.06f));
    rLeg = glm::scale(rLeg, glm::vec3(0.10f, 0.32f, 0.12f));
    drawCube(cubeVAO, lightingShader, robotBase * rLeg, dr, dg, db);

    // 3. Pelvis Joint
    glm::mat4 pelvis = glm::mat4(1.0f);
    pelvis = glm::translate(pelvis, glm::vec3(-0.22f, 0.44f, -0.10f));
    pelvis = glm::scale(pelvis, glm::vec3(0.44f, 0.08f, 0.20f));
    drawCube(cubeVAO, lightingShader, robotBase * pelvis, dr, dg, db);

    // 4. Main Torso / Body
    glm::mat4 torso = glm::mat4(1.0f);
    torso = glm::translate(torso, glm::vec3(-0.24f, 0.52f, -0.15f));
    torso = glm::scale(torso, glm::vec3(0.48f, 0.55f, 0.30f));
    drawCube(cubeVAO, lightingShader, robotBase * torso, mr, mg, mb, 0.6f, 64.0f);

    // Chest Interactive Screen (Unlit glowing panel with digital display)
    ourShader.use();
    glm::mat4 screen = glm::mat4(1.0f);
    screen = glm::translate(screen, glm::vec3(-0.16f, 0.65f, 0.152f));
    screen = glm::scale(screen, glm::vec3(0.32f, 0.24f, 0.01f));
    ourShader.setMat4("model", robotBase * screen);
    ourShader.setVec3("color", glm::vec3(0.08f, 0.12f, 0.18f)); // Dark screen glass
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Chest Status Indicator LEDs
    glm::mat4 led1 = glm::mat4(1.0f);
    led1 = glm::translate(led1, glm::vec3(-0.10f, 0.72f, 0.165f));
    led1 = glm::scale(led1, glm::vec3(0.05f, 0.05f, 0.01f));
    ourShader.setMat4("model", robotBase * led1);
    ourShader.setVec3("color", glm::vec3(0.1f, 0.95f, 0.2f)); // Glowing Green LED
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glm::mat4 led2 = glm::mat4(1.0f);
    led2 = glm::translate(led2, glm::vec3(-0.02f, 0.72f, 0.165f));
    led2 = glm::scale(led2, glm::vec3(0.05f, 0.05f, 0.01f));
    ourShader.setMat4("model", robotBase * led2);
    ourShader.setVec3("color", glm::vec3(0.1f, 0.85f, 1.0f)); // Glowing Cyan LED
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glm::mat4 led3 = glm::mat4(1.0f);
    led3 = glm::translate(led3, glm::vec3(0.06f, 0.72f, 0.165f));
    led3 = glm::scale(led3, glm::vec3(0.05f, 0.05f, 0.01f));
    ourShader.setMat4("model", robotBase * led3);
    ourShader.setVec3("color", glm::vec3(1.0f, 0.75f, 0.1f)); // Glowing Amber LED
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 5. Neck Joint
    lightingShader.use();
    glm::mat4 neck = glm::mat4(1.0f);
    neck = glm::translate(neck, glm::vec3(-0.07f, 1.07f, -0.06f));
    neck = glm::scale(neck, glm::vec3(0.14f, 0.08f, 0.12f));
    drawCube(cubeVAO, lightingShader, robotBase * neck, dr, dg, db);

    // 6. Robot Head
    glm::mat4 head = glm::mat4(1.0f);
    head = glm::translate(head, glm::vec3(-0.19f, 1.15f, -0.14f));
    head = glm::scale(head, glm::vec3(0.38f, 0.30f, 0.28f));
    drawCube(cubeVAO, lightingShader, robotBase * head, mr, mg, mb, 0.6f, 64.0f);

    // Ear bolts / sensors on sides of head
    glm::mat4 leftEar = glm::mat4(1.0f);
    leftEar = glm::translate(leftEar, glm::vec3(-0.22f, 1.25f, -0.04f));
    leftEar = glm::scale(leftEar, glm::vec3(0.04f, 0.10f, 0.08f));
    drawCube(cubeVAO, lightingShader, robotBase * leftEar, dr, dg, db);

    glm::mat4 rightEar = glm::mat4(1.0f);
    rightEar = glm::translate(rightEar, glm::vec3(0.18f, 1.25f, -0.04f));
    rightEar = glm::scale(rightEar, glm::vec3(0.04f, 0.10f, 0.08f));
    drawCube(cubeVAO, lightingShader, robotBase * rightEar, dr, dg, db);

    // Glowing Electric-Cyan Visor / Eyes (Rendered with unlit ourShader)
    ourShader.use();
    glm::mat4 visor = glm::mat4(1.0f);
    visor = glm::translate(visor, glm::vec3(-0.15f, 1.26f, 0.142f));
    visor = glm::scale(visor, glm::vec3(0.30f, 0.10f, 0.02f));
    ourShader.setMat4("model", robotBase * visor);
    ourShader.setVec3("color", glm::vec3(cr, cg, cb)); // Glowing Cyan Eye Visor!
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Antenna Mast
    lightingShader.use();
    glm::mat4 mast = glm::mat4(1.0f);
    mast = glm::translate(mast, glm::vec3(-0.015f, 1.45f, -0.015f));
    mast = glm::scale(mast, glm::vec3(0.03f, 0.16f, 0.03f));
    drawCube(cubeVAO, lightingShader, robotBase * mast, dr, dg, db);

    // Antenna Glowing Beacon Ball
    ourShader.use();
    glm::mat4 beacon = glm::mat4(1.0f);
    beacon = glm::translate(beacon, glm::vec3(-0.035f, 1.61f, -0.035f));
    beacon = glm::scale(beacon, glm::vec3(0.07f, 0.07f, 0.07f));
    ourShader.setMat4("model", robotBase * beacon);
    ourShader.setVec3("color", glm::vec3(1.0f, 0.25f, 0.25f)); // Pulsing red beacon tip
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // -------------------------------------------------------------
    // 7. Left Arm (Resting peacefully at robot's side)
    // -------------------------------------------------------------
    lightingShader.use();
    // Shoulder Joint
    glm::mat4 lShoulder = glm::mat4(1.0f);
    lShoulder = glm::translate(lShoulder, glm::vec3(-0.31f, 0.94f, -0.04f));
    lShoulder = glm::scale(lShoulder, glm::vec3(0.07f, 0.07f, 0.08f));
    drawCube(cubeVAO, lightingShader, robotBase * lShoulder, dr, dg, db);

    // Left Upper Arm
    glm::mat4 lArm = glm::mat4(1.0f);
    lArm = glm::translate(lArm, glm::vec3(-0.30f, 0.68f, -0.03f));
    lArm = glm::scale(lArm, glm::vec3(0.06f, 0.26f, 0.06f));
    drawCube(cubeVAO, lightingShader, robotBase * lArm, mr, mg, mb);

    // Left Hand Gripper
    glm::mat4 lHand = glm::mat4(1.0f);
    lHand = glm::translate(lHand, glm::vec3(-0.30f, 0.58f, -0.03f));
    lHand = glm::scale(lHand, glm::vec3(0.06f, 0.10f, 0.06f));
    drawCube(cubeVAO, lightingShader, robotBase * lHand, dr, dg, db);

    // -------------------------------------------------------------
    // 8. Right Arm: CONTINUOUS "BYE-BYE" WAVING HAND (Moving Object!)
    // -------------------------------------------------------------
    // Shoulder Pivot: Positioned at right side of upper torso
    glm::vec3 rShoulderPos = glm::vec3(0.25f, 0.98f, 0.0f);
    glm::mat4 shoulderMat = glm::translate(robotBase, rShoulderPos);
    // Raise upper arm upward and slightly outward into waving posture
    shoulderMat = glm::rotate(shoulderMat, glm::radians(50.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    shoulderMat = glm::rotate(shoulderMat, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // Right Shoulder Joint sphere/cube
    glm::mat4 rShoulderCube = glm::mat4(1.0f);
    rShoulderCube = glm::translate(rShoulderCube, glm::vec3(-0.035f, -0.035f, -0.035f));
    rShoulderCube = glm::scale(rShoulderCube, glm::vec3(0.07f, 0.07f, 0.07f));
    drawCube(cubeVAO, lightingShader, shoulderMat * rShoulderCube, dr, dg, db);

    // Right Upper Arm (extends outward)
    glm::mat4 rUpperArm = glm::mat4(1.0f);
    rUpperArm = glm::translate(rUpperArm, glm::vec3(-0.03f, 0.0f, -0.03f));
    rUpperArm = glm::scale(rUpperArm, glm::vec3(0.06f, 0.24f, 0.06f));
    drawCube(cubeVAO, lightingShader, shoulderMat * rUpperArm, mr, mg, mb);

    // Elbow Pivot: located at end of upper arm (Y = 0.24f)
    glm::mat4 elbowMat = glm::translate(shoulderMat, glm::vec3(0.0f, 0.24f, 0.0f));
    // Continuous Bye-Bye Waving: Waving hand oscillates back and forth!
    elbowMat = glm::rotate(elbowMat, glm::radians(waveAngle), glm::vec3(0.0f, 0.0f, 1.0f));

    // Elbow Joint
    glm::mat4 rElbowCube = glm::mat4(1.0f);
    rElbowCube = glm::translate(rElbowCube, glm::vec3(-0.035f, -0.035f, -0.035f));
    rElbowCube = glm::scale(rElbowCube, glm::vec3(0.07f, 0.07f, 0.07f));
    drawCube(cubeVAO, lightingShader, elbowMat * rElbowCube, dr, dg, db);

    // Right Forearm
    glm::mat4 rForearm = glm::mat4(1.0f);
    rForearm = glm::translate(rForearm, glm::vec3(-0.025f, 0.0f, -0.025f));
    rForearm = glm::scale(rForearm, glm::vec3(0.05f, 0.22f, 0.05f));
    drawCube(cubeVAO, lightingShader, elbowMat * rForearm, mr, mg, mb);

    // Wrist Pivot
    glm::mat4 wristMat = glm::translate(elbowMat, glm::vec3(0.0f, 0.22f, 0.0f));
    // Hand adds a friendly complementary wrist wave tilt
    wristMat = glm::rotate(wristMat, glm::radians(waveAngle * 0.4f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Hand Palm
    glm::mat4 palm = glm::mat4(1.0f);
    palm = glm::translate(palm, glm::vec3(-0.04f, 0.0f, -0.02f));
    palm = glm::scale(palm, glm::vec3(0.08f, 0.07f, 0.04f));
    drawCube(cubeVAO, lightingShader, wristMat * palm, dr, dg, db);

    // 3 Articulated Waving Fingers (Giving bye-bye!)
    for (int f = 0; f < 3; ++f) {
        float fx = -0.035f + f * 0.028f;
        glm::mat4 finger = glm::mat4(1.0f);
        finger = glm::translate(finger, glm::vec3(fx, 0.07f, -0.015f));
        finger = glm::scale(finger, glm::vec3(0.02f, 0.06f, 0.03f));
        drawCube(cubeVAO, lightingShader, wristMat * finger, mr, mg, mb);
    }
}

// 11. Ceiling Point Light Fixture (Box casing with glowing emissive underside)
void drawCeilingLightFixture(unsigned int& cubeVAO, Shader& lightingShader, Shader& ourShader, 
                             glm::mat4 lightBase, bool isLightOn)
{
    // Dark brown/bronze fixture housing
    glm::mat4 casing = glm::mat4(1.0f);
    casing = glm::translate(casing, glm::vec3(-0.45f, -0.08f, -0.25f));
    casing = glm::scale(casing, glm::vec3(0.90f, 0.08f, 0.50f));
    drawCube(cubeVAO, lightingShader, lightBase * casing, 0.20f, 0.16f, 0.12f);

    // Glowing diffuser panel (rendered with unlit shader)
    ourShader.use();
    glm::mat4 diffuser = glm::mat4(1.0f);
    diffuser = glm::translate(diffuser, glm::vec3(-0.40f, -0.09f, -0.20f));
    diffuser = glm::scale(diffuser, glm::vec3(0.80f, 0.02f, 0.40f));
    ourShader.setMat4("model", lightBase * diffuser);

    if (isLightOn) {
        ourShader.setVec3("color", glm::vec3(1.0f, 0.88f, 0.50f)); // Warm yellow glow
    } else {
        ourShader.setVec3("color", glm::vec3(0.25f, 0.25f, 0.25f)); // Dark off
    }

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

// 12. Directed Blackboard Spotlight Fixture
void drawSpotlightFixture(unsigned int& cubeVAO, Shader& lightingShader, Shader& ourShader, 
                          glm::mat4 fixtureBase, bool isSpotlightOn)
{
    // Mounting base
    glm::mat4 base = glm::mat4(1.0f);
    base = glm::translate(base, glm::vec3(-0.15f, -0.06f, -0.15f));
    base = glm::scale(base, glm::vec3(0.30f, 0.06f, 0.30f));
    drawCube(cubeVAO, lightingShader, fixtureBase * base, 0.18f, 0.18f, 0.20f);

    // Angled spotlight casing directed at the blackboard
    glm::mat4 head = glm::mat4(1.0f);
    head = glm::translate(head, glm::vec3(-0.12f, -0.22f, -0.12f));
    head = glm::rotate(head, glm::radians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    head = glm::scale(head, glm::vec3(0.24f, 0.20f, 0.24f));
    drawCube(cubeVAO, lightingShader, fixtureBase * head, 0.15f, 0.15f, 0.16f);

    // Glowing spotlight lens
    ourShader.use();
    glm::mat4 lens = glm::mat4(1.0f);
    lens = glm::translate(lens, glm::vec3(-0.09f, -0.24f, -0.09f));
    lens = glm::rotate(lens, glm::radians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    lens = glm::scale(lens, glm::vec3(0.18f, 0.03f, 0.18f));
    ourShader.setMat4("model", fixtureBase * lens);

    if (isSpotlightOn) {
        ourShader.setVec3("color", glm::vec3(1.0f, 0.96f, 0.85f));
    } else {
        ourShader.setVec3("color", glm::vec3(0.20f, 0.20f, 0.20f));
    }

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

// =========================================================================
// Input Processing & Callbacks
// =========================================================================

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // ---------------------------------------------------------------------
    // 1. Easy Camera Navigation using Keyboard 4 Arrow Keys & WASD
    // ---------------------------------------------------------------------
    float moveSpeed = camera.MovementSpeed;
    float turnSpeed = 65.0f; // degrees per second for smooth keyboard looking

    bool shiftPressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || 
                         glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

    // Forward / Backward / Vertical Navigation with Up/Down Arrow & W/S:
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        if (shiftPressed) {
            // Shift + Up Arrow: Move Camera Height UP (replaces E)
            camera.Position.y += moveSpeed * deltaTime;
        } else {
            camera.ProcessKeyboard(FORWARD, deltaTime);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        if (shiftPressed) {
            // Shift + Down Arrow: Move Camera Height DOWN (replaces Q)
            camera.Position.y = std::max(0.2f, camera.Position.y - moveSpeed * deltaTime);
        } else {
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    // Left Navigation (Left Arrow: turn look left by default; strafe left with Shift)
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        if (shiftPressed)
            camera.ProcessKeyboard(LEFT, deltaTime);
        else {
            camera.Yaw -= turnSpeed * deltaTime;
            camera.updateCameraVectors();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);

    // Right Navigation (Right Arrow: turn look right by default; strafe right with Shift)
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        if (shiftPressed)
            camera.ProcessKeyboard(RIGHT, deltaTime);
        else {
            camera.Yaw += turnSpeed * deltaTime;
            camera.updateCameraVectors();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // Optional PageUp / PageDown for vertical camera height
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        camera.Position.y += moveSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        camera.Position.y = std::max(0.2f, camera.Position.y - moveSpeed * deltaTime);

    // ---------------------------------------------------------------------
    // 2. Interactive 3D Model Transformations
    // ---------------------------------------------------------------------
    // Rotation (X, Y, Z, R)
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        if (rotateAxis_X) rotateAngle_X -= 15.0f * deltaTime;
        else if (rotateAxis_Y) rotateAngle_Y -= 15.0f * deltaTime;
        else rotateAngle_Z -= 15.0f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        rotateAngle_X += 15.0f * deltaTime;
        rotateAxis_X = 1.0f; rotateAxis_Y = 0.0f; rotateAxis_Z = 0.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        rotateAngle_Y += 15.0f * deltaTime;
        rotateAxis_X = 0.0f; rotateAxis_Y = 1.0f; rotateAxis_Z = 0.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        rotateAngle_Z += 15.0f * deltaTime;
        rotateAxis_X = 0.0f; rotateAxis_Y = 0.0f; rotateAxis_Z = 1.0f;
    }

    // Translation (I, K, J, L, P)
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) translate_Y += 1.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) translate_Y -= 1.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) translate_X += 1.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) translate_X -= 1.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) translate_Z -= 1.5f * deltaTime;

    // Scaling (C, N, M, U)
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) scale_X += 0.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) scale_Y = max(0.1f, scale_Y - 0.5f * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) scale_Z += 0.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) scale_Z = max(0.1f, scale_Z - 0.5f * deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

    // --- REQUIREMENT 2: THE 4 CORE CAMERA VIEWS (KEYS 1, 2, 3, 4) ---
    // When clicking the 4 keys, it immediately arrives at each perspective:
    if (key == GLFW_KEY_1 || key == GLFW_KEY_B) setCameraPreset(VIEW_BACK);     // Key 1: Back / Initial View
    if (key == GLFW_KEY_2 || key == GLFW_KEY_G) setCameraPreset(VIEW_SIDE);     // Key 2: Side View
    if (key == GLFW_KEY_3 || key == GLFW_KEY_T) setCameraPreset(VIEW_TOP);      // Key 3: Top View
    if (key == GLFW_KEY_4 || key == GLFW_KEY_F) setCameraPreset(VIEW_TEACHER);  // Key 4: Teacher View

    // Reset to Initial View: Key 0, HOME, or BACKSPACE
    if (key == GLFW_KEY_0 || key == GLFW_KEY_HOME || key == GLFW_KEY_BACKSPACE)
    {
        setCameraPreset(VIEW_BACK);
    }

    // Cycle through the 4 views: Key V or TAB
    if (key == GLFW_KEY_V || key == GLFW_KEY_TAB)
    {
        int nextMode = (currentView + 1) % 4; // Cycles: View 1 -> 2 -> 3 -> 4 -> 1
        setCameraPreset((CameraViewMode)nextMode);
    }

    // --- REQUIREMENT 3: INTERACTIVE DOOR OPEN / CLOSE (KEY O) ---
    if (key == GLFW_KEY_O)
    {
        isDoorOpen = !isDoorOpen;
        cout << "[Door] " << (isDoorOpen ? "OPENING (Swinging Inward)" : "CLOSING") << endl;
    }

    // --- CEILING FAN CONTROLS ---
    // Toggle fan rotation on/off
    if (key == GLFW_KEY_SPACE)
    {
        isFanOn = !isFanOn;
        cout << "[Ceiling Fan] " << (isFanOn ? "ROTATING (ON)" : "STOPPED (OFF)") << endl;
    }
    // Increase fan speed
    if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD)
    {
        fanSpeed += 60.0f;
        cout << "[Ceiling Fan] Speed increased to " << fanSpeed << " deg/s" << endl;
    }
    // Decrease fan speed
    if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT)
    {
        fanSpeed = max(0.0f, fanSpeed - 60.0f);
        cout << "[Ceiling Fan] Speed decreased to " << fanSpeed << " deg/s" << endl;
    }

    // --- REQUIREMENT 4: TWO KINDS OF LIGHT CONTROLS ---
    // 1. Toggle Point Lights (Ceiling fixtures): Key L or Key 8
    if (key == GLFW_KEY_L || key == GLFW_KEY_8)
    {
        if (pointLightOn) {
            pointlight1.turnOff();
            pointlight2.turnOff();
            pointlight3.turnOff();
            pointlight4.turnOff();
            pointLightOn = false;
            cout << "[Point Lights] OFF" << endl;
        } else {
            pointlight1.turnOn();
            pointlight2.turnOn();
            pointlight3.turnOn();
            pointlight4.turnOn();
            pointLightOn = true;
            cout << "[Point Lights] ON" << endl;
        }
    }

    // 2. Toggle Spotlight (Blackboard Lamp): Key K or Key 9
    if (key == GLFW_KEY_K || key == GLFW_KEY_9)
    {
        blackboardSpotlight.toggle();
        cout << "[Spotlight] " << (blackboardSpotlight.isOn ? "ON (Focused on Blackboard)" : "OFF") << endl;
    }

    // Ambient light intensity adjustments
    if (key == GLFW_KEY_2)
    {
        pointlight1.ambient += 0.05f; pointlight2.ambient += 0.05f;
        pointlight3.ambient += 0.05f; pointlight4.ambient += 0.05f;
        cout << "[Lighting] Ambient increased" << endl;
    }
    if (key == GLFW_KEY_3)
    {
        pointlight1.ambient = max(glm::vec3(0.0f), pointlight1.ambient - 0.05f);
        pointlight2.ambient = max(glm::vec3(0.0f), pointlight2.ambient - 0.05f);
        pointlight3.ambient = max(glm::vec3(0.0f), pointlight3.ambient - 0.05f);
        pointlight4.ambient = max(glm::vec3(0.0f), pointlight4.ambient - 0.05f);
        cout << "[Lighting] Ambient decreased" << endl;
    }

    // Diffuse light intensity adjustments
    if (key == GLFW_KEY_4)
    {
        pointlight1.diffuse += 0.1f; pointlight2.diffuse += 0.1f;
        pointlight3.diffuse += 0.1f; pointlight4.diffuse += 0.1f;
        cout << "[Lighting] Diffuse increased" << endl;
    }
    if (key == GLFW_KEY_5)
    {
        pointlight1.diffuse = max(glm::vec3(0.0f), pointlight1.diffuse - 0.1f);
        pointlight2.diffuse = max(glm::vec3(0.0f), pointlight2.diffuse - 0.1f);
        pointlight3.diffuse = max(glm::vec3(0.0f), pointlight3.diffuse - 0.1f);
        pointlight4.diffuse = max(glm::vec3(0.0f), pointlight4.diffuse - 0.1f);
        cout << "[Lighting] Diffuse decreased" << endl;
    }

    // Specular light intensity adjustments
    if (key == GLFW_KEY_6)
    {
        pointlight1.specular += 0.1f; pointlight2.specular += 0.1f;
        pointlight3.specular += 0.1f; pointlight4.specular += 0.1f;
        cout << "[Lighting] Specular increased" << endl;
    }
    if (key == GLFW_KEY_7)
    {
        pointlight1.specular = max(glm::vec3(0.0f), pointlight1.specular - 0.1f);
        pointlight2.specular = max(glm::vec3(0.0f), pointlight2.specular - 0.1f);
        pointlight3.specular = max(glm::vec3(0.0f), pointlight3.specular - 0.1f);
        pointlight4.specular = max(glm::vec3(0.0f), pointlight4.specular - 0.1f);
        cout << "[Lighting] Specular decreased" << endl;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
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

    // Enable mouse look on left or right mouse button drag
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ||
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
