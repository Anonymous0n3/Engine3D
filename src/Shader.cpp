// Shader.cpp
#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * @brief Konstruktor třídy Shader.
 *
 * Otevře zadané soubory s kódem shaderů, přečte jejich obsah a následně
 * zkompiluje vertex a fragment shader. Poté je slinkuje do výsledného
 * shader programu a uvolní původní zkompilované části, protože již nejsou potřeba.
 *
 * @param vertexPath Cesta k souboru s kódem vertex shaderu.
 * @param fragmentPath Cesta k souboru s kódem fragment shaderu.
 */
Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

/**
 * @brief Destruktor třídy Shader.
 *
 * Zavolá OpenGL funkci glDeleteProgram pro odstranění shader programu z paměti grafické karty.
 */
Shader::~Shader() {
    glDeleteProgram(ID);
}

/**
 * @brief Aktivuje tento shader program.
 *
 * Slouží k nastavení tohoto shaderu jako aktuálního pro následující vykreslovací příkazy (draw calls).
 */
void Shader::use() const {
    glUseProgram(ID);
}

/**
 * @brief Nastaví uniformní proměnnou typu boolean s využitím Direct State Access (DSA).
 *
 * Umožňuje nastavit hodnotu proměnné přímo, aniž by bylo nutné shader program předem aktivovat.
 *
 * @param name Název uniformní proměnné v kódu shaderu.
 * @param value Hodnota pro nastavení (true/false).
 */
void Shader::setBool(const std::string& name, bool value) const {
    glProgramUniform1i(ID, glGetUniformLocation(ID, name.c_str()), (int)value);
}

/**
 * @brief Nastaví uniformní proměnnou typu integer s využitím DSA.
 *
 * @param name Název uniformní proměnné v kódu shaderu.
 * @param value Celočíselná hodnota pro nastavení.
 */
void Shader::setInt(const std::string& name, int value) const {
    glProgramUniform1i(ID, glGetUniformLocation(ID, name.c_str()), value);
}

/**
 * @brief Nastaví uniformní proměnnou typu float s využitím DSA.
 *
 * @param name Název uniformní proměnné v kódu shaderu.
 * @param value Hodnota s plovoucí desetinnou čárkou pro nastavení.
 */
void Shader::setFloat(const std::string& name, float value) const {
    glProgramUniform1f(ID, glGetUniformLocation(ID, name.c_str()), value);
}

/**
 * @brief Nastaví uniformní proměnnou typu mat4 s využitím DSA.
 *
 * @param name Název uniformní proměnné v kódu shaderu.
 * @param mat Matice 4x4 (typu glm::mat4) pro nastavení.
 */
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glProgramUniformMatrix4fv(ID, glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

/**
 * @brief Nastaví uniformní proměnnou typu vec3 s využitím DSA.
 *
 * @param name Název uniformní proměnné v kódu shaderu.
 * @param vec Vektor o třech prvcích (typu glm::vec3) pro nastavení.
 */
void Shader::setVec3(const std::string& name, const glm::vec3& vec) const {
    glProgramUniform3fv(ID, glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
}

/**
 * @brief Interní metoda pro kontrolu chyb při kompilaci a linkování.
 *
 * Dotáže se OpenGL na stav kompilace (nebo linkování programu) a v případě selhání
 * vypíše informační log do standardního chybového výstupu.
 *
 * @param shader OpenGL identifikátor kompilovaného shaderu nebo linkovaného programu.
 * @param type Řetězec určující typ prováděné kontroly (např. "VERTEX", "FRAGMENT", "PROGRAM").
 */
void Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}