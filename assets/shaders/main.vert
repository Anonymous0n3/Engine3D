// main.vert
#version 460 core

/**
 * @brief Vstupní atribut vrcholu: Pozice vrcholu v lokálním prostoru objektu.
 */
layout (location = 0) in vec3 aPos;

/**
 * @brief Vstupní atribut vrcholu: Normálový vektor povrchu.
 */
layout (location = 1) in vec3 aNormal;

/**
 * @brief Vstupní atribut vrcholu: 2D texturové souřadnice (UV).
 */
layout (location = 2) in vec2 aTexCoords;

/**
 * @brief Výstupní proměnná pro fragment shader: Pozice vrcholu ve světovém prostoru.
 * Využívá se především pro výpočty vzdálenosti a směru u osvětlení.
 */
out vec3 FragPos;

/**
 * @brief Výstupní proměnná pro fragment shader: Normálový vektor transformovaný do světového prostoru.
 */
out vec3 Normal;

/**
 * @brief Výstupní proměnná pro fragment shader: Texturové souřadnice předávané k interpolaci.
 */
out vec2 TexCoords;

/**
 * @brief Modelová matice (Model matrix) pro transformaci souřadnic z lokálního prostoru modelu do světového prostoru.
 */
uniform mat4 model;

/**
 * @brief Matice pohledu (View matrix) reprezentující pozici a orientaci kamery.
 */
uniform mat4 view;

/**
 * @brief Projekční matice (Projection matrix) definující perspektivu a zorný komolý jehlan (frustum).
 */
uniform mat4 projection;

/**
 * @brief Hlavní funkce vertex shaderu.
 * * Přepočítává lokální souřadnice vrcholů a normál do světového a následně klipového (clip space) prostoru.
 * Aplikuje takzvanou normálovou matici (transpose inverse model matice) k zachování správného směru 
 * normál i při nestejnoměrném škálování objektu. Zapisuje finální pozici vrcholu do zabudované proměnné gl_Position.
 */
void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}