#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <memory>
#include <cstdlib> 
#include <ctime>   

// --- KNIHOVNY TŘETÍCH STRAN ---
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// --- VLASTNÍ MODULY ENGINE ---
#include "Camera.h"
#include "Performance.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "Audio.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(2.0f, 0.0f, 2.0f));
PerformanceTracker perfTracker;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// --- STAVOVÉ PROMĚNNÉ ---
bool inMainMenu = true;
bool inMenu = false;
bool isGameOver = false;
bool isGameWon = false;
bool showDebug = true;
bool flashlightOn = true;
bool simpleFlashlight = false;

// --- NASTAVENÍ GRAFIKY ---
bool isFullscreen = false;
bool vsyncEnabled = true;
bool msaaEnabled = true;
int windowedX, windowedY, windowedWidth, windowedHeight;

// --- FYZIKA A ZVUKY ---
float verticalVelocity = 0.0f;
const float GRAVITY = -18.0f;
const float JUMP_FORCE = 7.0f;
bool isGrounded = true;
bool isWalking = false;
std::unique_ptr<Sound3D> footstepSound;

const int MAZE_SIZE = 25;
int maze[MAZE_SIZE][MAZE_SIZE] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,1},
    {1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1},
    {1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1},
    {1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
    {1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1},
    {1,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,1,0,1},
    {1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1},
    {1,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

bool isColliding(glm::vec3 pos) {
    float objectRadius = 0.3f;
    // Check only the 4 nearby cells instead of all 625
    int minCol = (int)((pos.x - objectRadius + 1.0f) / 2.0f);
    int maxCol = (int)((pos.x + objectRadius + 1.0f) / 2.0f);
    int minRow = (int)((pos.z - objectRadius + 1.0f) / 2.0f);
    int maxRow = (int)((pos.z + objectRadius + 1.0f) / 2.0f);
    for (int row = minRow; row <= maxRow; row++)
        for (int col = minCol; col <= maxCol; col++)
            if (row >= 0 && row < MAZE_SIZE && col >= 0 && col < MAZE_SIZE)
                if (maze[row][col] == 1) return true;
    return false;
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam) {
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
}

#ifndef RESOURCE_DIR
#define RESOURCE_DIR ""
#endif

std::string getAssetPath(const std::string& relativePath) {
    std::string path = std::string(RESOURCE_DIR) + relativePath;
    if (std::filesystem::exists(path)) return path;
    if (std::filesystem::exists(relativePath)) return relativePath;
    return relativePath;
}

struct Particle {
    glm::vec3 position, velocity, color;
    float life, maxLife, size;
};

struct BunnyArtefact {
    glm::vec3 position;
    glm::vec3 color;
    std::unique_ptr<Sound3D> sound;
    BunnyArtefact(glm::vec3 pos, glm::vec3 col, AudioEngine& engine, std::string soundPath)
        : position(pos), color(col) {
        sound = std::make_unique<Sound3D>(engine, soundPath, true);
        sound->SetPosition(pos);
        sound->SetVolume(0.4f);
        sound->Play();
    }
};
std::vector<BunnyArtefact> bunnies;

struct PatrolEnemy {
    glm::vec3 currentPos, velocity;
    float baseY, timeOffset;
    std::unique_ptr<Sound3D> sound;
    std::vector<Particle> particles;

    PatrolEnemy(glm::vec3 start, glm::vec3 vel, float offset, AudioEngine& engine, std::string soundPath)
        : currentPos(start), velocity(vel), baseY(start.y), timeOffset(offset) {
        sound = std::make_unique<Sound3D>(engine, soundPath, true);
        sound->SetPosition(currentPos);
        sound->SetVolume(0.7f);
        sound->Play();
    }

    void Update(float dt, float globalTime) {
        glm::vec3 nextPos = currentPos;
        nextPos.x += velocity.x * dt;
        nextPos.z += velocity.z * dt;
        if (isColliding(nextPos)) velocity = -velocity;
        else { currentPos.x = nextPos.x; currentPos.z = nextPos.z; }
        currentPos.y = baseY + cos(globalTime * 2.5f + timeOffset) * 0.3f;
        if (sound) sound->SetPosition(currentPos);

        if (particles.size() < 40) {
            int emitCount = 1 + (rand() % 2);
            for (int i = 0; i < emitCount; ++i) {
                Particle p;
                p.position = currentPos + glm::vec3(((rand() % 100) / 100.0f - 0.5f) * 0.3f, 0.1f, ((rand() % 100) / 100.0f - 0.5f) * 0.3f);
                p.velocity = glm::vec3(((rand() % 100) / 100.0f - 0.5f) * 0.6f, 0.5f + ((rand() % 100) / 100.0f) * 1.5f, ((rand() % 100) / 100.0f - 0.5f) * 0.6f);
                p.maxLife = 0.3f + ((rand() % 100) / 100.0f) * 0.5f;
                p.life = p.maxLife;
                p.size = 0.01f + ((rand() % 100) / 100.0f) * 0.03f;
                p.color = glm::vec3(1.0f, 0.3f + ((rand() % 100) / 100.0f) * 0.4f, 0.0f);
                particles.push_back(p);
            }

            for (auto it = particles.begin(); it != particles.end(); ) {
                it->life -= dt;
                if (it->life > 0.0f) {
                    glm::vec3 nextPPos = it->position + it->velocity * dt;
                    if (isColliding(nextPPos) || nextPPos.y >= 2.0f) { it = particles.erase(it); continue; }
                    else { it->position = nextPPos; }
                    ++it;
                }
                else { it = particles.erase(it); }
            }
        }
    }
};
std::vector<PatrolEnemy> enemies;

void resetGame(AudioEngine& audio) {
    camera = Camera(glm::vec3(2.0f, 0.0f, 2.0f));
    bunnies.clear();
    enemies.clear();

    int numBunnies = 3 + (rand() % 8);
    for (int i = 0; i < numBunnies; i++) {
        int r, c;
        do { r = rand() % MAZE_SIZE; c = rand() % MAZE_SIZE; } while (maze[r][c] == 1 || r <= 2 || c <= 2);
        glm::vec3 color((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f);
        if (color.r < 0.2f && color.g < 0.2f && color.b < 0.2f) color = glm::vec3(0.8f, 0.8f, 0.8f);
        bunnies.emplace_back(glm::vec3(c * 2.0f, -0.5f, r * 2.0f), color, audio, getAssetPath("assets/audio/artefact.mp3"));
    }

    int numGuards = 2 + (rand() % 4);
    for (int i = 0; i < numGuards; i++) {
        int r, c;
        do { r = rand() % MAZE_SIZE; c = rand() % MAZE_SIZE; } while (maze[r][c] == 1 || r <= 4 || c <= 4);
        glm::vec3 vel(0.0f);
        if (rand() % 2 == 0) vel.x = (rand() % 2 == 0) ? 2.5f : -2.5f;
        else vel.z = (rand() % 2 == 0) ? 2.5f : -2.5f;
        enemies.emplace_back(glm::vec3(c * 2.0f, 0.0f, r * 2.0f), vel, (rand() % 100) / 10.0f, audio, getAssetPath("assets/audio/ghost.mp3"));
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (inMenu || inMainMenu || isGameOver || isGameWon) return;
    if (firstMouse) { lastX = (float)xposIn; lastY = (float)yposIn; firstMouse = false; }
    float xoffset = (float)xposIn - lastX; float yoffset = lastY - (float)yposIn;
    lastX = (float)xposIn; lastY = (float)yposIn;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        if (!inMainMenu && !isGameOver && !isGameWon) {
            inMenu = !inMenu;
            if (inMenu) { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
            else { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); firstMouse = true; }
        }
    }
    if (key == GLFW_KEY_F3 && action == GLFW_PRESS) { showDebug = !showDebug; }
    if (key == GLFW_KEY_F && action == GLFW_PRESS && !inMenu && !inMainMenu && !isGameOver && !isGameWon) flashlightOn = !flashlightOn;
}

void processInput(GLFWwindow* window) {
    float dt = perfTracker.deltaTime;
    glm::vec3 oldPos = camera.Position;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, dt);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded) { verticalVelocity = JUMP_FORCE; isGrounded = false; }

    verticalVelocity += GRAVITY * dt;
    camera.Position.y += verticalVelocity * dt;
    if (camera.Position.y >= 0.8f) { camera.Position.y = 0.8f; if (verticalVelocity > 0.0f) verticalVelocity = 0.0f; }
    if (camera.Position.y <= 0.0f) { camera.Position.y = 0.0f; verticalVelocity = 0.0f; isGrounded = true; }

    glm::vec3 newPos = camera.Position;
    camera.Position = oldPos;
    camera.Position.x = newPos.x;
    if (isColliding(camera.Position)) camera.Position.x = oldPos.x;
    camera.Position.z = newPos.z;
    if (isColliding(camera.Position)) camera.Position.z = oldPos.z;
    camera.Position.y = newPos.y;

    float movedDistance = glm::length(glm::vec2(camera.Position.x - oldPos.x, camera.Position.z - oldPos.z));
    bool wasWalking = isWalking;
    isWalking = (movedDistance > 0.001f && isGrounded);
    if (footstepSound) {
        if (isWalking && !wasWalking) footstepSound->Play();
        else if (!isWalking && wasWalking) footstepSound->Stop();
        if (isWalking) footstepSound->SetPosition(camera.Position);
    }

    for (auto it = bunnies.begin(); it != bunnies.end(); ) {
        if (glm::distance(camera.Position, it->position) < 1.2f) { it->sound->Stop(); it = bunnies.erase(it); }
        else ++it;
    }
}

int main() {
    srand((unsigned int)time(NULL));
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Engine3D - Haunted Maze", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(vsyncEnabled ? 1 : 0);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(glDebugOutput, nullptr);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);

    Shader mainShader(getAssetPath("assets/shaders/main.vert").c_str(), getAssetPath("assets/shaders/main.frag").c_str());
    Model cubeModel(getAssetPath("assets/models/cube.obj"));
    Model bunnyModel(getAssetPath("assets/models/bunny.obj"));
    Model skullModel(getAssetPath("assets/models/skull.obj"));
    Texture wallTex(getAssetPath("assets/textures/wall.jpg").c_str());
    Texture floorTex(getAssetPath("assets/textures/floor.jpg").c_str());
    Texture skullTex(getAssetPath("assets/textures/skull.jpg").c_str());

    AudioEngine audio;
    footstepSound = std::make_unique<Sound3D>(audio, getAssetPath("assets/audio/footsteps.mp3"), true);
    if (footstepSound) { footstepSound->SetVolume(0.5f); footstepSound->Stop(); }
    std::unique_ptr<Sound3D> ambientSound = std::make_unique<Sound3D>(audio, getAssetPath("assets/audio/ambient.mp3"), true);
    if (ambientSound) {
        ambientSound->SetVolume(0.2f);
        ambientSound->SetSpatialization(false); // no 3D positioning at all
        ambientSound->Play();
    }

    resetGame(audio);

    while (!glfwWindowShouldClose(window)) {
        perfTracker.Update(window);
        float t = (float)glfwGetTime();
        float dt = perfTracker.deltaTime;

        if (!inMenu && !inMainMenu && !isGameOver && !isGameWon) {
            processInput(window);
            audio.UpdateListener(camera.Position, camera.Front, camera.Up);
            for (auto& enemy : enemies) {
                enemy.Update(dt, t);
                if (glm::distance(camera.Position, enemy.currentPos) < 0.8f) { isGameOver = true; glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
            }
            if (bunnies.empty()) { isGameWon = true; glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
        }
        else if (isWalking && footstepSound) { isWalking = false; footstepSound->Stop(); }

        glClearColor(0.005f, 0.005f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiIO& io = ImGui::GetIO();

        mainShader.use();
        glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        mainShader.setMat4("projection", proj);
        mainShader.setMat4("view", camera.GetViewMatrix());
        mainShader.setBool("isParticle", false);

        // --- SVĚTLA ---
        mainShader.setVec3("viewPos", camera.Position);
        mainShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        mainShader.setVec3("dirLight.ambient", glm::vec3(0.02f, 0.02f, 0.04f));
        mainShader.setVec3("dirLight.diffuse", glm::vec3(0.05f, 0.05f, 0.1f));
        mainShader.setVec3("dirLight.specular", glm::vec3(0.1f, 0.1f, 0.1f));

        mainShader.setInt("numPointLights", (int)enemies.size());
        for (size_t i = 0; i < enemies.size(); i++) {
            std::string n = std::to_string(i);
            mainShader.setVec3("pointLights[" + n + "].position", enemies[i].currentPos);
            mainShader.setVec3("pointLights[" + n + "].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
            glm::vec3 pColor = (i % 2 == 0) ? glm::vec3(1, 0.1, 0.1) : glm::vec3(0.1, 1, 0.1);
            mainShader.setVec3("pointLights[" + n + "].diffuse", pColor);
            mainShader.setVec3("pointLights[" + n + "].specular", pColor);
            mainShader.setFloat("pointLights[" + n + "].constant", 1.0f);
            mainShader.setFloat("pointLights[" + n + "].linear", 2.0f);
            mainShader.setFloat("pointLights[" + n + "].quadratic", 6.0f);
        }

        // ==========================================
        // --- LOGIKA OPRAVENÉ SVÍTILNY ---
        // ==========================================
        bool isFlashlightActive = (!inMenu && !inMainMenu && !isGameOver && !isGameWon && flashlightOn);
        mainShader.setVec3("spotLight.position", camera.Position);
        mainShader.setVec3("spotLight.direction", camera.Front);
        mainShader.setVec3("spotLight.ambient", glm::vec3(0.0f));

        if (isFlashlightActive) {
            glm::vec3 lColor;
            float flicker;
            float angle;
            float outerAngle;

            if (simpleFlashlight) {
                lColor = glm::vec3(1.5f, 1.5f, 1.5f);
                flicker = 1.0f;
                angle = 17.5f;
                outerAngle = 22.5f;
            }
            else {
                lColor = glm::vec3(1.0f, 0.81f, 0.17f); // warm yellow tint
                flicker = 0.97f + sin(t * 7.0f) * 0.02f  // slow gentle wave
                    + sin(t * 23.0f) * 0.01f;        // faster secondary wobble
                angle = 12.5f;
                outerAngle = 17.5f;
            }

            mainShader.setVec3("spotLight.diffuse", lColor * flicker);
            mainShader.setVec3("spotLight.specular", lColor * flicker);
            mainShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(angle)));
            mainShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(outerAngle)));
        }
        else {
            mainShader.setVec3("spotLight.diffuse", glm::vec3(0.0f));
            mainShader.setVec3("spotLight.specular", glm::vec3(0.0f));
        }

        mainShader.setFloat("spotLight.constant", 1.0f);
        mainShader.setFloat("spotLight.linear", 0.07f); // Baterka dosvítí dál
        mainShader.setFloat("spotLight.quadratic", 0.017f);

        // --- VYKRESLOVÁNÍ ---
        mainShader.setBool("useTexture", true); mainShader.setFloat("alpha", 1.0f);
        mainShader.setFloat("shininess", 8.0f); mainShader.setFloat("specularStrength", 0.1f);
        mainShader.setVec3("objectColor", glm::vec3(1.0f));
        for (int r = 0; r < MAZE_SIZE; r++) {
            for (int c = 0; c < MAZE_SIZE; c++) {
                glm::vec3 pos(c * 2.0f, 0.0f, r * 2.0f);
                if (glm::distance(glm::vec2(pos.x, pos.z),
                    glm::vec2(camera.Position.x, camera.Position.z)) > 20.0f)
                    continue;
                if (maze[r][c] == 1) {
                    wallTex.Bind(0);
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
                    mainShader.setMat4("model", model);
                    mainShader.setMat3("normalMatrix", glm::mat3(glm::transpose(glm::inverse(model)))); // ADD
                    cubeModel.Draw();
                }
                else {
                    floorTex.Bind(0);
                    glm::mat4 modelFloor = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, -2, 0));
                    mainShader.setMat4("model", modelFloor);
                    mainShader.setMat3("normalMatrix", glm::mat3(glm::transpose(glm::inverse(modelFloor)))); // ADD
                    cubeModel.Draw();
                    glm::mat4 modelCeil = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 2, 0));
                    mainShader.setMat4("model", modelCeil);
                    mainShader.setMat3("normalMatrix", glm::mat3(glm::transpose(glm::inverse(modelCeil)))); // ADD
                    cubeModel.Draw();
                }
            }
        }
        mainShader.setBool("useTexture", false); mainShader.setFloat("alpha", 0.3f);
        mainShader.setFloat("shininess", 256.0f); mainShader.setFloat("specularStrength", 4.0f);
        for (auto& bunny : bunnies) {
            mainShader.setVec3("objectColor", bunny.color);
            glm::mat4 m = glm::translate(glm::mat4(1.0f), bunny.position);
            m = glm::translate(m, glm::vec3(0.0f, sin(t * 3.0f) * 0.15f, 0.0f));
            m = glm::rotate(m, t * 1.2f, glm::vec3(0, 1, 0));
            m = glm::scale(m, glm::vec3(0.1f));
            mainShader.setMat4("model", m);
            mainShader.setMat3("normalMatrix", glm::mat3(glm::transpose(glm::inverse(m)))); // ADD
            bunnyModel.Draw();
        }
        mainShader.setBool("useTexture", true); mainShader.setVec3("objectColor", glm::vec3(1)); mainShader.setFloat("alpha", 0.8f);
        mainShader.setFloat("shininess", 32.0f); mainShader.setFloat("specularStrength", 0.5f);
        for (auto& enemy : enemies) {
            skullTex.Bind(0); glm::mat4 m = glm::translate(glm::mat4(1.0f), enemy.currentPos);
            float angle = atan2(enemy.velocity.x, enemy.velocity.z);
            m = glm::rotate(m, angle, glm::vec3(0, 1, 0)); m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1, 0, 0));
            m = glm::scale(m, glm::vec3(0.03f));
            mainShader.setMat4("model", m);
            mainShader.setMat3("normalMatrix", glm::mat3(glm::transpose(glm::inverse(m)))); // ADD
            skullModel.Draw();
        }
        mainShader.setBool("isParticle", true); mainShader.setBool("useTexture", false);
        for (auto& enemy : enemies) {
            for (auto& p : enemy.particles) {
                mainShader.setVec3("objectColor", p.color);
                mainShader.setFloat("alpha", (p.life / p.maxLife) * 0.8f);
                glm::mat4 m = glm::translate(glm::mat4(1.0f), p.position); m = glm::scale(m, glm::vec3(p.size));
                mainShader.setMat4("model", m); cubeModel.Draw();
            }
        }
        mainShader.setBool("isParticle", false);

        // --- HUD ---
        if (showDebug && !inMenu && !inMainMenu && !isGameOver && !isGameWon) {
            ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.35f);
            ImGui::Begin("Debug", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "--- DEBUG HUD (F3) ---");
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("Baterka: %s", flashlightOn ? "ON" : "OFF");
            ImGui::Text("Rezim: %s", simpleFlashlight ? "BILE (STABILNI)" : "HOROR (BLIKANI)");
            ImGui::Text("Artefakty: %zu", bunnies.size());
            ImGui::End();
        }

        // --- MENU ---
        if (inMainMenu || inMenu || isGameOver || isGameWon) {
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(io.DisplaySize);
            ImVec4 bg = isGameOver ? ImVec4(0.3f, 0, 0, 0.75f) : (isGameWon ? ImVec4(0, 0.3f, 0, 0.75f) : ImVec4(0, 0, 0, 0.75f));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, bg);

            const char* title = isGameWon ? "VYHRAL JSI!" : (isGameOver ? "ZEMREL JSI" : (inMainMenu ? "HAUNTED MAZE" : "PAUZA"));
            ImGui::Begin("MenuWindow", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

            ImGui::SetWindowFontScale(3.5f);
            float tw = ImGui::CalcTextSize(title).x;
            ImGui::SetCursorPosX((io.DisplaySize.x - tw) * 0.5f);
            ImGui::SetCursorPosY(io.DisplaySize.y * 0.15f);
            ImGui::Text(title);

            ImGui::SetWindowFontScale(1.3f);
            float cx = (io.DisplaySize.x - 350.0f) * 0.5f;
            ImGui::SetCursorPosX(cx);
            ImGui::SetCursorPosY(io.DisplaySize.y * 0.4f);

            if (ImGui::Button(isGameOver || isGameWon || inMainMenu ? "START / RESTART" : "POKRACOVAT", ImVec2(350, 60))) {
                if (isGameOver || isGameWon || inMainMenu) resetGame(audio);
                inMainMenu = inMenu = isGameOver = isGameWon = false;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                firstMouse = true;
            }

            ImGui::SetCursorPosX(cx);
            ImGui::Separator();

            ImGui::SetCursorPosX(cx);
            ImGui::Checkbox(" Jednoducha svitilna (Bila/Neblika)", &simpleFlashlight);

            ImGui::SetCursorPosX(cx);
            if (ImGui::Checkbox(" VSync", &vsyncEnabled)) glfwSwapInterval(vsyncEnabled ? 1 : 0);

            ImGui::SetCursorPosX(cx);
            if (ImGui::Checkbox(" Antialiasing (MSAA 4x)", &msaaEnabled)) {
                if (msaaEnabled) glEnable(GL_MULTISAMPLE);
                else             glDisable(GL_MULTISAMPLE);
            }

            ImGui::SetCursorPosX(cx);
            if (ImGui::Checkbox(" Fullscreen", &isFullscreen)) {
                if (isFullscreen) {
                    glfwGetWindowPos(window, &windowedX, &windowedY); glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
                    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate);
                }
                else {
                    glfwSetWindowMonitor(window, NULL, windowedX, windowedY, windowedWidth, windowedHeight, 0);
                }
            }

            ImGui::SetCursorPosY(io.DisplaySize.y * 0.85f);
            ImGui::SetCursorPosX(cx);
            if (ImGui::Button("EXIT", ImVec2(350, 50))) glfwSetWindowShouldClose(window, true);

            ImGui::End();
            ImGui::PopStyleColor();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    footstepSound.reset(); ambientSound.reset();
    ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplGlfw_Shutdown(); ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}