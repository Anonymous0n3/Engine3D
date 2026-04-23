// Camera.h
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

/**
 * @enum Camera_Movement
 * @brief Výčet možných směrů pohybu kamery.
 *
 * Slouží k abstrakci vstupů (např. z klávesnice) od konkrétního okenního systému.
 */
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    ROLL_LEFT,
    ROLL_RIGHT
};

/**
 * @brief Výchozí hodnota úhlu Yaw (otočení vlevo/vpravo).
 */
const float YAW = -90.0f;

/**
 * @brief Výchozí hodnota úhlu Pitch (otočení nahoru/dolů).
 */
const float PITCH = 0.0f;

/**
 * @brief Výchozí hodnota úhlu Roll (naklonění do stran).
 */
const float ROLL = 0.0f;

/**
 * @brief Výchozí rychlost pohybu kamery.
 */
const float SPEED = 5.0f;

/**
 * @brief Výchozí citlivost myši pro otáčení kamery.
 */
const float SENSITIVITY = 0.1f;

/**
 * @brief Výchozí hodnota přiblížení (Field of View / Zoom).
 */
const float ZOOM = 45.0f;

/**
 * @class Camera
 * @brief Třída reprezentující volnou 3D kameru (tzv. fly/FPS kamera).
 *
 * Spravuje orientaci kamery, její pozici ve 3D prostoru a převádí vstupy
 * od uživatele na pohyb. Výsledkem je View matice, kterou lze použít
 * ve vertex shaderu k transformaci scény.
 */
class Camera {
public:
    /**
     * @brief Pozice kamery ve světovém prostoru.
     */
    glm::vec3 Position;

    /**
     * @brief Směrový vektor určující, kam kamera hledí.
     */
    glm::vec3 Front;

    /**
     * @brief Vektor určující směr "nahoru" relativně vůči orientaci kamery.
     */
    glm::vec3 Up;

    /**
     * @brief Vektor určující směr "doprava" relativně vůči orientaci kamery.
     */
    glm::vec3 Right;

    /**
     * @brief Globální vektor určující směr "nahoru" ve světovém prostoru (obvykle 0, 1, 0).
     */
    glm::vec3 WorldUp;

    /**
     * @brief Úhel rotace kamery do stran (kolem osy Y).
     */
    float Yaw;

    /**
     * @brief Úhel rotace kamery nahoru/dolů (kolem osy X).
     */
    float Pitch;

    /**
     * @brief Úhel naklonění kamery (kolem osy Z).
     */
    float Roll;

    /**
     * @brief Rychlost, jakou se kamera pohybuje prostředím.
     */
    float MovementSpeed;

    /**
     * @brief Citlivost reakce kamery na pohyb myši.
     */
    float MouseSensitivity;

    /**
     * @brief Úroveň přiblížení (využívá se k výpočtu zorného úhlu - FOV).
     */
    float Zoom;

    /**
     * @brief Konstruktor třídy Camera.
     *
     * Inicializuje kameru na zadané pozici s výchozími úhly a nastaví
     * směrové vektory.
     *
     * @param position Počáteční pozice kamery (výchozí je střed souřadnic).
     * @param up Vektor určující směr nahoru ve světovém prostoru (výchozí je osa Y).
     * @param yaw Počáteční úhel Yaw.
     * @param pitch Počáteční úhel Pitch.
     */
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    /**
     * @brief Vypočítá a vrátí View matici (matici pohledu).
     *
     * Využívá aktuální pozici, směrový vektor (Front) a vektor Up
     * pro sestavení matice pomocí funkce glm::lookAt.
     *
     * @return 4x4 View matice.
     */
    glm::mat4 GetViewMatrix() const;

    /**
     * @brief Zpracuje vstup z klávesnice a posune kameru odpovídajícím směrem.
     *
     * @param direction Směr pohybu zadaný výčtem Camera_Movement.
     * @param deltaTime Čas uplynulý od posledního snímku (zajišťuje plynulý pohyb nezávislý na FPS).
     */
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    /**
     * @brief Zpracuje vstup z myši a otočí kameru.
     *
     * @param xoffset Posun myši na ose X.
     * @param yoffset Posun myši na ose Y.
     * @param constrainPitch Určuje, zda se má omezit vertikální úhel (pitch), aby se kamera nepřetočila vzhůru nohama (výchozí je true).
     */
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    /**
     * @brief Zpracuje vstup z kolečka myši a upraví úroveň přiblížení (zoom).
     *
     * @param yoffset Hodnota posunu kolečka myši (obvykle 1 nebo -1).
     */
    void ProcessMouseScroll(float yoffset);

private:
    /**
     * @brief Přepočítá všechny směrové vektory kamery (Front, Right, Up).
     *
     * Tato funkce se volá interně po každé změně úhlů (Eulerových úhlů),
     * aby se aktualizovala prostorová orientace kamery.
     */
    void updateCameraVectors();
};