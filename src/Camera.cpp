// Camera.cpp
#include "Camera.h"

/**
 * @brief Konstruktor třídy Camera.
 * * Inicializuje základní atributy kamery, jako je pozice, orientace a rychlost.
 * Na závěr zavolá updateCameraVectors pro prvotní výpočet směrových vektorů.
 * * @param position Počáteční pozice v prostoru.
 * @param up Vektor určující směr "nahoru" ve světě.
 * @param yaw Počáteční rotace kolem osy Y.
 * @param pitch Počáteční rotace kolem osy X.
 */
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), Roll(ROLL) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

/**
 * @brief Generuje matici pohledu (View Matrix) pro transformaci scény.
 * * @return glm::mat4 Matice pohledu vypočítaná pomocí funkce lookAt.
 */
glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}

/**
 * @brief Zpracovává vstup z klávesnice pro pohyb a naklánění kamery.
 * * Metoda zajišťuje posun v 6 stupních volnosti (pohyb všemi směry)
 * a rotaci typu Roll (naklánění do stran).
 * * @param direction Identifikátor směru pohybu.
 * @param deltaTime Časový rozdíl mezi snímky pro plynulý pohyb.
 */
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;

    if (direction == FORWARD)  Position += Front * velocity;
    if (direction == BACKWARD) Position -= Front * velocity;
    if (direction == LEFT)     Position -= Right * velocity;
    if (direction == RIGHT)    Position += Right * velocity;
    if (direction == UP)       Position += Up * velocity;
    if (direction == DOWN)     Position -= Up * velocity;

    float rollVelocity = MovementSpeed * 10.0f * deltaTime;
    if (direction == ROLL_LEFT)  Roll -= rollVelocity;
    if (direction == ROLL_RIGHT) Roll += rollVelocity;

    updateCameraVectors();
}

/**
 * @brief Zpracovává pohyb myši pro otáčení kamery (Yaw a Pitch).
 * * @param xoffset Horizontální posun myši.
 * @param yoffset Vertikální posun myši.
 * @param constrainPitch Pokud je true, omezí rotaci nahoru/dolů na 89 stupňů (prevence převrácení).
 */
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        if (Pitch > 89.0f)  Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
    }

    updateCameraVectors();
}

/**
 * @brief Zpracovává kolečko myši pro změnu zorného pole (Zoom).
 * * @param yoffset Hodnota posunu kolečka myši.
 */
void Camera::ProcessMouseScroll(float yoffset) {
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)  Zoom = 1.0f;
    if (Zoom > 90.0f) Zoom = 90.0f;
}

/**
 * @brief Přepočítává směrové vektory kamery na základě Eulerových úhlů.
 * * Tato metoda vypočítá nový vektor Front pomocí goniometrických funkcí
 * a následně odvodí vektory Right a Up pomocí vektorových součinů.
 * Na závěr aplikuje rotaci Roll kolem osy Front.
 */
void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    glm::vec3 baseRight = glm::normalize(glm::cross(Front, WorldUp));
    glm::vec3 baseUp = glm::normalize(glm::cross(baseRight, Front));

    Right = glm::rotate(baseRight, glm::radians(Roll), Front);
    Up = glm::rotate(baseUp, glm::radians(Roll), Front);
}