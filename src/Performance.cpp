// Performance.cpp
#include "Performance.h"

/**
 * @brief Konstruktor třídy PerformanceTracker.
 *
 * Inicializuje všechny interní časovače a počítadla snímků na nulové hodnoty,
 * aby bylo připraveno čisté prostředí pro začátek měření.
 */
PerformanceTracker::PerformanceTracker() {
    deltaTime = 0.0f;
    lastFrame = 0.0f;
    lastTime = 0.0f;
    nbFrames = 0;
    framesPerSecond = 0;
}

/**
 * @brief Aktualizuje logiku pro sledování výkonu a času.
 *
 * Získá aktuální čas pomocí knihovny GLFW a vypočítá hodnotu deltaTime,
 * která reprezentuje čas uplynulý od předchozího snímku. Zároveň zvyšuje
 * počítadlo snímků a v intervalu jedné sekundy aktualizuje hodnotu FPS a
 * dobu trvání jednoho snímku v milisekundách. Tyto údaje pak propíše přímo
 * do titulku aplikace.
 *
 * @param window Ukazatel na instanci GLFW okna, u kterého se má aktualizovat text v titulku.
 */
void PerformanceTracker::Update(GLFWwindow* window) {
    float currentFrame = static_cast<float>(glfwGetTime());

    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    nbFrames++;

    if (currentFrame - lastTime >= 1.0f) {
        framesPerSecond = nbFrames;

        std::string title = "Engine3D | FPS: " + std::to_string(framesPerSecond) +
            " | ms/frame: " + std::to_string(1000.0 / double(nbFrames));

        glfwSetWindowTitle(window, title.c_str());

        nbFrames = 0;
        lastTime += 1.0f;
    }
}