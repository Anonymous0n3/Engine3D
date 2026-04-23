// Performance.h
#pragma once
#include <GLFW/glfw3.h>
#include <string>

/**
 * @class PerformanceTracker
 * @brief Třída pro sledování výkonu aplikace a časování.
 *
 * Měří čas, který uplynul mezi jednotlivými snímky (Delta Time), a počítá
 * aktuální počet snímků za sekundu (FPS).
 */
class PerformanceTracker {
public:
    /**
     * @brief Čas uplynulý od posledního vykresleného snímku (v sekundách).
     *
     * Slouží k zajištění nezávislosti rychlosti hry na snímkové frekvenci
     * (např. pro plynulý pohyb nezávisle na výkonu počítače).
     */
    float deltaTime;

    /**
     * @brief Aktuální počet snímků za sekundu (FPS).
     */
    int framesPerSecond;

    /**
     * @brief Konstruktor třídy PerformanceTracker.
     *
     * Inicializuje interní časovače a vynuluje počítadla snímků.
     */
    PerformanceTracker();

    /**
     * @brief Aktualizuje časovače a výpočet FPS.
     *
     * Vypočítá aktuální deltaTime a pokud uplynula celá vteřina, aktualizuje
     * hodnotu framesPerSecond. Může také aktualizovat titulek okna s informací o FPS.
     *
     * @param window Ukazatel na GLFW okno (využívá se například k zobrazení FPS v titulku).
     */
    void Update(GLFWwindow* window);

private:
    /**
     * @brief Čas (v sekundách), kdy byl zpracován předchozí snímek.
     */
    float lastFrame;

    /**
     * @brief Čas posledního updatu počítadla FPS (pro měření jednosekundových intervalů).
     */
    float lastTime;

    /**
     * @brief Počet snímků vykreslených v aktuálním jednosekundovém intervalu.
     */
    int nbFrames;
};