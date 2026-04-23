// Mesh.h
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"

/**
 * @struct Vertex
 * @brief Struktura reprezentující jeden vrchol (vertex) ve 3D prostoru.
 *
 * Obsahuje všechna potřebná data pro vykreslení vrcholu, včetně jeho pozice,
 * normály pro výpočet osvětlení a texturových souřadnic.
 */
struct Vertex {
    /**
     * @brief Pozice vrcholu ve 3D prostoru (x, y, z).
     */
    glm::vec3 Position;

    /**
     * @brief Normálový vektor vrcholu určující směr "ven" z povrchu (využívá se u osvětlení).
     */
    glm::vec3 Normal;

    /**
     * @brief UV souřadnice pro mapování 2D textury na povrch modelu (u, v).
     */
    glm::vec2 TexCoords;
};

/**
 * @class Mesh
 * @brief Třída reprezentující 3D síť (mesh) tvořenou vrcholy a indexy.
 *
 * Spravuje data o vrcholech a indexech, generuje potřebné OpenGL buffery (VAO, VBO, EBO)
 * a poskytuje metodu pro samotné vykreslení sítě.
 */
class Mesh {
public:
    /**
     * @brief Kolekce všech vrcholů, ze kterých se síť skládá.
     */
    std::vector<Vertex> vertices;

    /**
     * @brief Kolekce indexů určujících, v jakém pořadí z vrcholů sestavit trojúhelníky.
     */
    std::vector<unsigned int> indices;

    /**
     * @brief Konstruktor třídy Mesh.
     *
     * Vytvoří síť z předaných vektorů vrcholů a indexů.
     *
     * @param vertices Vektor struktur Vertex.
     * @param indices Vektor indexů pro element buffer.
     */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    /**
     * @brief Destruktor třídy Mesh.
     *
     * Bezpečně uvolní z paměti grafické karty všechny spravované OpenGL buffery.
     */
    ~Mesh();

    /**
     * @brief Zakázaný kopírovací konstruktor.
     *
     * Zabraňuje kopírování objektu, čímž se předchází vícenásobnému smazání
     * stejných OpenGL bufferů (double-free).
     */
    Mesh(const Mesh&) = delete;

    /**
     * @brief Zakázaný operátor přiřazení kopií.
     */
    Mesh& operator=(const Mesh&) = delete;

    /**
     * @brief Přesouvací konstruktor (Move constructor).
     *
     * Umožňuje efektivně předat vlastnictví OpenGL bufferů a dat do nového objektu.
     *
     * @param other Původní instance Mesh, ze které se data přesouvají.
     */
    Mesh(Mesh&& other) noexcept;

    /**
     * @brief Přesouvací operátor přiřazení (Move assignment).
     *
     * @param other Původní instance Mesh, ze které se data přesouvají.
     * @return Reference na aktuální objekt po přesunu dat.
     */
    Mesh& operator=(Mesh&& other) noexcept;

    /**
     * @brief Připraví a inicializuje OpenGL buffery.
     *
     * Vygeneruje VAO, VBO a EBO, nahraje do nich data vrcholů a indexů a
     * nakonfiguruje ukazatele na atributy vrcholů (position, normal, texCoords).
     */
    void setupMesh();

    /**
     * @brief Vykreslí celou 3D síť.
     *
     * Naváže příslušné Vertex Array Object (VAO) a zavolá OpenGL příkaz pro
     * vykreslení elementů (glDrawElements). Před zavoláním této metody by měl
     * být aktivován požadovaný shader program.
     */
    void Draw() const;

private:
    /**
     * @brief Identifikátory OpenGL objektů: Vertex Array Object (VAO),
     * Vertex Buffer Object (VBO) a Element Buffer Object (EBO).
     */
    GLuint VAO, VBO, EBO;
};