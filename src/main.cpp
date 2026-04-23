// main.cpp - Hlavní vstupní bod aplikace Haunted Maze
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

/**
 * @section Nastavení a Globální Instance
 */
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(2.0f, 0.0f, 2.0f));
PerformanceTracker perfTracker;

// Vstupní data myši
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

/**
 * @section Stavové Proměnné Hry
 */
bool inMainMenu = true;
bool inMenu = false;
bool isGameOver = false;
bool isGameWon = false;
bool showDebug = true;
bool flashlightOn = true;
bool isFullscreen = false;
bool vsyncEnabled = true;
bool msaaEnabled = true;
int windowedX, windowedY, windowedWidth, windowedHeight;

/**
 * @section Fyzika a Pohyb
 */
float verticalVelocity = 0.0f;
const float GRAVITY = -18.0f;
const float JUMP_FORCE = 7.0f;
bool isGrounded = true;

bool isWalking = false;
std::unique_ptr<Sound3D> footstepSound;

/**
 * @section Definice Bludiště
 * 1 = Zeď, 0 = Volná cesta
 */
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

/**
 * Kontrola kolizí s bludištěm (AABB vs Maze Grid)
 */
bool isColliding(glm::vec3 pos) {
    float objectRadius = 0.3f;
    for (int row = 0; row < MAZE_SIZE; row++) {
        for (int col = 0; col < MAZE_SIZE; col++) {
            if (maze[row][col] == 1) {
                float wallX = col * 2.0f; float wallZ = row * 2.0f;
                if (pos.x + objectRadius > wallX - 1.0f && pos.x - objectRadius < wallX + 1.0f &&
                    pos.z + objectRadius > wallZ - 1.0f && pos.z - objectRadius < wallZ + 1.0f) return true;
            }
        }
    }
    return false;
}

/**
 * OpenGL Debug Callback - Výpis chyb a varování z ovladače
 */
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam) {
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
    std::cout << "[OpenGL Debug] " << message << std::endl;
}

#ifndef RESOURCE_DIR
#define RESOURCE_DIR ""
#endif

/**
 * Pomocná funkce pro lokalizaci souborů assetů
 */
std::string getAssetPath(const std::string& relativePath) {
    std::string path = std::string(RESOURCE_DIR) + relativePath;
    if (std::filesystem::exists(path)) return path;
    if (std::filesystem::exists(relativePath)) return relativePath;
    return relativePath;
}

/**
 * @section Herní Entity a Částicový Systém
 */

struct Particle {
    glm::vec3 position, velocity, color;
    float life, maxLife, size;
};

// Sběratelský předmět (králík)
struct BunnyArtefact {
    glm::vec3 position;
    glm::vec3 color;
    std::unique_ptr<Sound3D> sound;

    BunnyArtefact(glm::vec3 pos, glm::vec3 col, AudioEngine& engine, std::string soundPath)
        : position(pos), color(col)
    {
        sound = std::make_unique<Sound3D>(engine, soundPath, true);
        sound->SetPosition(pos);
        sound->SetVolume(0.4f);
        sound->Play();
    }
};
std::vector<BunnyArtefact> bunnies;

// Nepřítel hlídkující v bludišti
struct PatrolEnemy {
    glm::vec3 currentPos;
    glm::vec3 velocity;
    float baseY;
    float timeOffset;
    std::unique_ptr<Sound3D> sound;
    std::vector<Particle> particles;

    PatrolEnemy(glm::vec3 start, glm::vec3 vel, float offset, AudioEngine& engine, std::string soundPath)
        : currentPos(start), velocity(vel), baseY(start.y), timeOffset(offset)
    {
        sound = std::make_unique<Sound3D>(engine, soundPath, true);
        sound->SetPosition(currentPos);
        sound->SetVolume(0.7f);
        sound->Play();
    }

    void Update(float dt, float globalTime) {
        // Logika pohybu a odrazů od zdí
        glm::vec3 nextPos = currentPos;
        nextPos.x += velocity.x * dt;
        nextPos.z += velocity.z * dt;

        if (isColliding(nextPos)) velocity = -velocity;
        else { currentPos.x = nextPos.x; currentPos.z = nextPos.z; }

        currentPos.y = baseY + cos(globalTime * 2.5f + timeOffset) * 0.3f;

        if (sound) sound->SetPosition(currentPos);

        // Emitování částic (vizuální efekt ohně/energie)
        int emitCount = 1 + (rand() % 2);
        for (int i = 0; i < emitCount; ++i) {
            Particle p;
            float offsetX = ((rand() % 100) / 100.0f - 0.5f) * 0.3f;
            float offsetZ = ((rand() % 100) / 100.0f - 0.5f) * 0.3f;
            p.position = currentPos + glm::vec3(offsetX, 0.1f, offsetZ);

            float velX = ((rand() % 100) / 100.0f - 0.5f) * 0.6f;
            float velY = 0.5f + ((rand() % 100) / 100.0f) * 1.5f;
            float velZ = ((rand() % 100) / 100.0f - 0.5f) * 0.6f;
            p.velocity = glm::vec3(velX, velY, velZ);

            p.maxLife = 0.3f + ((rand() % 100) / 100.0f) * 0.5f;
            p.life = p.maxLife;
            p.size = 0.01f + ((rand() % 100) / 100.0f) * 0.03f;
            p.color = glm::vec3(1.0f, 0.3f + ((rand() % 100) / 100.0f) * 0.4f, 0.0f);
            particles.push_back(p);
        }

        // Update částic s kolizemi
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
};
std::vector<PatrolEnemy> enemies;

/**
 * Inicializace / Reset herního světa
 */
void resetGame(AudioEngine& audio) {
    camera = Camera(glm::vec3(2.0f, 0.0f, 2.0f));
    bunnies.clear();
    enemies.clear();

    // Spawn králíčků (náhodné pozice mimo spawn hráče)
    int numBunnies = 3 + (rand() % 8);
    for (int i = 0; i < numBunnies; i++) {
        int r, c;
        do { r = rand() % MAZE_SIZE; c = rand() % MAZE_SIZE; } while (maze[r][c] == 1 || (r <= 2 && c <= 2));

        glm::vec3 color((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f);
        if (color.r < 0.2f && color.g < 0.2f && color.b < 0.2f) color = glm::vec3(0.8f, 0.8f, 0.8f);

        bunnies.emplace_back(glm::vec3(c * 2.0f, -0.5f, r * 2.0f), color, audio, getAssetPath("assets/audio/artefact.mp3"));
    }

    // Spawn nepřátel
    int numGuards = 2 + (rand() % 4);
    for (int i = 0; i < numGuards; i++) {
        int r, c;
        do { r = rand() % MAZE_SIZE; c = rand() % MAZE_SIZE; } while (maze[r][c] == 1 || (r <= 4 && c <= 4));

        glm::vec3 vel(0.0f);
        if (rand() % 2 == 0) vel.x = (rand() % 2 == 0) ? 2.5f : -2.5f;
        else vel.z = (rand() % 2 == 0) ? 2.5f : -2.5f;

        float offset = (rand() % 100) / 10.0f;
        enemies.emplace_back(glm::vec3(c * 2.0f, 0.0f, r * 2.0f), vel, offset, audio, getAssetPath("assets/audio/ghost.mp3"));
    }
}

/**
 * @section Callbacks pro Vstup
 */
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
    if (key == GLFW_KEY_F && action == GLFW_PRESS && !inMenu && !inMainMenu && !isGameOver && !isGameWon) {
        flashlightOn = !flashlightOn;
    }
}

/**
 * Zpracování uživatelského vstupu a herní fyziky
 */
void processInput(GLFWwindow* window) {
    float dt = perfTracker.deltaTime;
    glm::vec3 oldPos = camera.Position;

    // Pohyb
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, dt);

    // Skok
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded) { verticalVelocity = JUMP_FORCE; isGrounded = false; }

    // Gravitace a vertikální pohyb
    verticalVelocity += GRAVITY * dt;
    camera.Position.y += verticalVelocity * dt;

    if (camera.Position.y >= 0.8f) { camera.Position.y = 0.8f; if (verticalVelocity > 0.0f) verticalVelocity = 0.0f; }
    if (camera.Position.y <= 0.0f) { camera.Position.y = 0.0f; verticalVelocity = 0.0f; isGrounded = true; }

    // Kolize (rozklad na osy pro plynulé klouzání po zdi)
    glm::vec3 newPos = camera.Position;
    camera.Position = oldPos;
    camera.Position.x = newPos.x;
    if (isColliding(camera.Position)) camera.Position.x = oldPos.x;
    camera.Position.z = newPos.z;
    if (isColliding(camera.Position)) camera.Position.z = oldPos.z;
    camera.Position.y = newPos.y;

    // Logika zvuků kroků
    float movedDistance = glm::length(glm::vec2(camera.Position.x - oldPos.x, camera.Position.z - oldPos.z));
    bool wasWalking = isWalking;
    isWalking = (movedDistance > 0.001f && isGrounded);

    if (footstepSound) {
        if (isWalking && !wasWalking) footstepSound->Play();
        else if (!isWalking && wasWalking) footstepSound->Stop();
        if (isWalking) footstepSound->SetPosition(camera.Position);
    }

    // Sbírání králíčků
    for (auto it = bunnies.begin(); it != bunnies.end(); ) {
        if (glm::distance(camera.Position, it->position) < 1.2f) { it->sound->Stop(); it = bunnies.erase(it); }
        else ++it;
    }
}

/**
 * @section Hlavní Funkce
 */
int main() {
    srand((unsigned int)time(NULL));

    // Inicializace GLFW a nastavení OpenGL kontextu
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

    // Zapnutí OpenGL debugování
    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
    }

    // Registrace callbacků
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Inicializace ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Nastavení globálních OpenGL stavů
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);

    // Načítání assetů
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
    if (ambientSound) { ambientSound->SetVolume(0.2f); ambientSound->Play(); }

    resetGame(audio);

    /**
     * @section Main Render Loop
     */
    while (!glfwWindowShouldClose(window)) {
        perfTracker.Update(window);
        float t = (float)glfwGetTime();
        float dt = perfTracker.deltaTime;

        if (ambientSound) ambientSound->SetPosition(camera.Position);

        // --- UPDATE LOGIKA ---
        if (!inMenu && !inMainMenu && !isGameOver && !isGameWon) {
            processInput(window);
            audio.UpdateListener(camera.Position, camera.Front, camera.Up);
            for (auto& enemy : enemies) {
                enemy.Update(dt, t);
                if (glm::distance(camera.Position, enemy.currentPos) < 0.8f) {
                    isGameOver = true;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
            }
            if (bunnies.empty()) {
                isGameWon = true;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
        else if (isWalking && footstepSound) {
            isWalking = false; footstepSound->Stop();
        }

        // --- RENDER SCÉNY ---
        glClearColor(0.005f, 0.005f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        mainShader.use();
        glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        mainShader.setMat4("projection", proj);
        mainShader.setMat4("view", camera.GetViewMatrix());
        mainShader.setBool("isParticle", false);

        // --- NASTAVENÍ OSVĚTLENÍ (Lights) ---
        mainShader.setVec3("viewPos", camera.Position);
        mainShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        mainShader.setVec3("dirLight.ambient", glm::vec3(0.02f, 0.02f, 0.04f));
        mainShader.setVec3("dirLight.diffuse", glm::vec3(0.05f, 0.05f, 0.1f));
        mainShader.setVec3("dirLight.specular", glm::vec3(0.1f, 0.1f, 0.1f));

        // Bodová světla od nepřátel
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

        // Svítilna hráče (Spotlight)
        bool isFlashlightActive = (!inMenu && !inMainMenu && !isGameOver && !isGameWon && flashlightOn);
        mainShader.setVec3("spotLight.position", camera.Position);
        mainShader.setVec3("spotLight.direction", camera.Front);
        mainShader.setVec3("spotLight.ambient", glm::vec3(0.0f));
        if (isFlashlightActive) {
            mainShader.setVec3("spotLight.diffuse", glm::vec3(1.0f, 0.9f, 0.8f));
            mainShader.setVec3("spotLight.specular", glm::vec3(1.0f));
        }
        else {
            mainShader.setVec3("spotLight.diffuse", glm::vec3(0.0f));
            mainShader.setVec3("spotLight.specular", glm::vec3(0.0f));
        }
        mainShader.setFloat("spotLight.constant", 1.0f);
        mainShader.setFloat("spotLight.linear", 0.045f);
        mainShader.setFloat("spotLight.quadratic", 0.0075f);
        mainShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        mainShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

        // --- VYKRESLOVÁNÍ GEOMETRIE ---

        // Zdi a podlahy bludiště
        mainShader.setBool("useTexture", true); mainShader.setFloat("alpha", 1.0f);
        mainShader.setFloat("shininess", 8.0f); mainShader.setFloat("specularStrength", 0.1f);
        for (int r = 0; r < MAZE_SIZE; r++) {
            for (int c = 0; c < MAZE_SIZE; c++) {
                glm::vec3 pos(c * 2.0f, 0.0f, r * 2.0f);
                if (maze[r][c] == 1) {
                    wallTex.Bind(0);
                    mainShader.setMat4("model", glm::translate(glm::mat4(1.0f), pos));
                    cubeModel.Draw();
                }
                else {
                    floorTex.Bind(0);
                    mainShader.setMat4("model", glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, -2, 0))); cubeModel.Draw();
                    mainShader.setMat4("model", glm::translate(glm::mat4(1.0f), pos + glm::vec3(0, 2, 0))); cubeModel.Draw();
                }
            }
        }

        // Artefakty (Králíčci)
        mainShader.setBool("useTexture", false); mainShader.setFloat("alpha", 0.3f);
        mainShader.setFloat("shininess", 256.0f); mainShader.setFloat("specularStrength", 4.0f);
        for (auto& bunny : bunnies) {
            mainShader.setVec3("objectColor", bunny.color);
            glm::mat4 m = glm::translate(glm::mat4(1.0f), bunny.position);
            m = glm::translate(m, glm::vec3(0.0f, sin(t * 3.0f) * 0.15f, 0.0f));
            m = glm::rotate(m, t * 1.2f, glm::vec3(0, 1, 0));
            m = glm::scale(m, glm::vec3(0.1f));
            mainShader.setMat4("model", m); bunnyModel.Draw();
        }

        // Nepřátelé (Lebky)
        mainShader.setBool("useTexture", true); mainShader.setVec3("objectColor", glm::vec3(1)); mainShader.setFloat("alpha", 0.8f);
        mainShader.setFloat("shininess", 32.0f); mainShader.setFloat("specularStrength", 0.5f);
        for (auto& enemy : enemies) {
            skullTex.Bind(0); glm::mat4 m = glm::translate(glm::mat4(1.0f), enemy.currentPos);
            float angle = atan2(enemy.velocity.x, enemy.velocity.z);
            m = glm::rotate(m, angle, glm::vec3(0, 1, 0)); m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1, 0, 0));
            m = glm::scale(m, glm::vec3(0.03f)); mainShader.setMat4("model", m); skullModel.Draw();
        }

        // Částicové efekty
        mainShader.setBool("isParticle", true);
        mainShader.setBool("useTexture", false);
        for (auto& enemy : enemies) {
            for (auto& p : enemy.particles) {
                mainShader.setVec3("objectColor", p.color);
                mainShader.setFloat("alpha", (p.life / p.maxLife) * 0.8f);
                glm::mat4 m = glm::translate(glm::mat4(1.0f), p.position);
                m = glm::scale(m, glm::vec3(p.size));
                mainShader.setMat4("model", m); cubeModel.Draw();
            }
        }
        mainShader.setBool("isParticle", false);

        // --- HUD A UI MENU ---
        if (showDebug && !inMenu && !inMainMenu && !isGameOver && !isGameWon) {
            ImGui::SetNextWindowPos(ImVec2(10, 10));
            ImGui::Begin("Debug", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("Pozice: %.1f, %.1f, %.1f", camera.Position.x, camera.Position.y, camera.Position.z);
            ImGui::Text("Direkcionalni Lampa (F): %s", flashlightOn ? "ZAP" : "VYP");
            ImGui::Text("Zbyva artefaktu: %zu", bunnies.size());
            ImGui::End();
        }

        // Lambda pro vykreslení fullscreen menu oken
        auto drawFullscreenMenu = [&](const char* title, ImVec4 color, const char* btnText, bool restart) {
            ImGui::SetNextWindowPos(ImVec2(0, 0)); ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.75f));
            ImGui::Begin(title, NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowFontScale(3.0f);
            float tw = ImGui::CalcTextSize(title).x;
            ImGui::SetCursorPosX((ImGui::GetIO().DisplaySize.x - tw) * 0.5f);
            ImGui::SetCursorPosY(ImGui::GetIO().DisplaySize.y * 0.2f);
            ImGui::TextColored(color, title);
            ImGui::SetWindowFontScale(1.5f);
            ImGui::SetCursorPosX((ImGui::GetIO().DisplaySize.x - 300) * 0.5f);
            ImGui::SetCursorPosY(ImGui::GetIO().DisplaySize.y * 0.5f);
            if (ImGui::Button(btnText, ImVec2(300, 60))) {
                if (restart) resetGame(audio);
                inMainMenu = isGameOver = isGameWon = inMenu = false;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                firstMouse = true;
            }
            ImGui::SetCursorPosX((ImGui::GetIO().DisplaySize.x - 300) * 0.5f);
            if (ImGui::Button("UKONCIT HRU", ImVec2(300, 60))) glfwSetWindowShouldClose(window, true);
            ImGui::End(); ImGui::PopStyleColor();
            };

        if (isGameWon) drawFullscreenMenu("VYHRAL JSI!", ImVec4(0, 1, 0, 1), "HRAT ZNOVU", true);
        else if (isGameOver) drawFullscreenMenu("ZEMREL JSI", ImVec4(1, 0, 0, 1), "ZKUSIT ZNOVU", true);
        else if (inMainMenu) drawFullscreenMenu("HAUNTED MAZE", ImVec4(1, 0.5, 0, 1), "NOVA HRA", true);
        else if (inMenu) drawFullscreenMenu("PAUZA", ImVec4(1, 1, 1, 1), "POKRACOVAT", false);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- CLEANUP ---
    footstepSound.reset(); ambientSound.reset();
    ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplGlfw_Shutdown(); ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}