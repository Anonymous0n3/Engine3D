// Shader.h
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

/**
 * @class Shader
 * @brief Třída pro načítání, kompilaci a správu OpenGL shader programů.
 *
 * Poskytuje rozhraní pro vytvoření shader programu ze zdrojových souborů (vertex a fragment)
 * a metody pro bezpečné nastavování uniformních proměnných (s využitím Direct State Access).
 */
class Shader {
public:
    /**
     * @brief OpenGL identifikátor (handle) pro tento shader program.
     */
    GLuint ID;

    /**
     * @brief Konstruktor třídy Shader.
     *
     * Načte zdrojové kódy ze zadaných cest k souborům, zkompiluje jednotlivé shadery
     * a slinkuje je do výsledného shader programu.
     *
     * @param vertexPath Cesta k souboru se zdrojovým kódem vertex shaderu.
     * @param fragmentPath Cesta k souboru se zdrojovým kódem fragment shaderu.
     */
    Shader(const char* vertexPath, const char* fragmentPath);

    /**
     * @brief Destruktor třídy Shader.
     *
     * Smaže shader program z paměti grafické karty.
     */
    ~Shader();

    /**
     * @brief Aktivuje tento shader program.
     *
     * Tuto metodu je nutné zavolat před samotným příkazem pro vykreslování (draw call).
     */
    void use() const;

    /**
     * @brief Nastaví uniformní proměnnou typu boolean.
     *
     * @param name Název uniformní proměnné v shaderu.
     * @param value Hodnota, která se má nastavit (true/false).
     */
    void setBool(const std::string& name, bool value) const;

    /**
     * @brief Nastaví uniformní proměnnou typu integer.
     *
     * @param name Název uniformní proměnné v shaderu.
     * @param value Celočíselná hodnota, která se má nastavit.
     */
    void setInt(const std::string& name, int value) const;

    /**
     * @brief Nastaví uniformní proměnnou typu float.
     *
     * @param name Název uniformní proměnné v shaderu.
     * @param value Hodnota s plovoucí desetinnou čárkou, která se má nastavit.
     */
    void setFloat(const std::string& name, float value) const;

    /**
     * @brief Nastaví uniformní proměnnou typu mat4 (matice 4x4).
     *
     * @param name Název uniformní proměnné v shaderu.
     * @param mat Matice (typu glm::mat4), která se má nastavit.
     */
    void setMat4(const std::string& name, const glm::mat4& mat) const;

    /**
     * @brief Nastaví uniformní proměnnou typu vec3 (vektor 3 hodnot).
     *
     * @param name Název uniformní proměnné v shaderu.
     * @param vec Vektor (typu glm::vec3), který se má nastavit.
     */
    void setVec3(const std::string& name, const glm::vec3& vec) const;

private:
    /**
     * @brief Pomocná funkce pro kontrolu chyb po kompilaci nebo linkování shaderů.
     *
     * Vypíše chybové hlášení s informačním logem z OpenGL, pokud dojde k chybě.
     *
     * @param shader OpenGL identifikátor kompilovaného shaderu nebo linkovaného programu.
     * @param type Řetězec určující typ operace ("VERTEX", "FRAGMENT" pro kompilaci, "PROGRAM" pro linkování).
     */
    void checkCompileErrors(GLuint shader, std::string type);
};