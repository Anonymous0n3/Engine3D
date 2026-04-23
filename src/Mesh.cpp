// Mesh.cpp
#include "Mesh.h"
#include <utility>

/**
 * @brief Konstruktor třídy Mesh.
 * * Inicializuje vnitřní kontejnery pro vrcholy a indexy a nastaví OpenGL ID na nulu.
 * Samotné nahrání dat na GPU (setupMesh) se provádí až po stabilizaci dat v paměti.
 * * @param vertices Vektor vrcholů tvořících síť.
 * @param indices Vektor indexů určujících pořadí vykreslování.
 */
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
    this->vertices = vertices;
    this->indices = indices;

    VAO = 0;
    VBO = 0;
    EBO = 0;
}

/**
 * @brief Přesouvací konstruktor (Move constructor).
 * * Přebírá vlastnictví prostředků (paměť v RAM i ID v GPU) z jiného objektu.
 * Původnímu objektu nastaví ID na 0, aby jeho destruktor neuvolnil prostředky v GPU.
 * * @param other Instance, ze které jsou data přesouvána.
 */
Mesh::Mesh(Mesh&& other) noexcept
    : vertices(std::move(other.vertices)),
    indices(std::move(other.indices)),
    VAO(other.VAO), VBO(other.VBO), EBO(other.EBO) {

    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
}

/**
 * @brief Přesouvací operátor přiřazení (Move assignment).
 * * Pokud se nejedná o přiřazení sobě samému, uvolní stávající OpenGL prostředky,
 * převezme nová data a původní objekt vynuluje.
 * * @param other Instance, ze které jsou data přesouvána.
 * @return Reference na tento objekt.
 */
Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }

        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;

        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }
    return *this;
}

/**
 * @brief Destruktor třídy Mesh.
 * * Uvolní alokované OpenGL buffery (VAO, VBO, EBO) z paměti GPU,
 * pokud objekt stále vlastní platná ID.
 */
Mesh::~Mesh() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

/**
 * @brief Inicializuje OpenGL buffery pomocí Direct State Access (DSA).
 * * Vytvoří VAO, VBO a EBO, alokuje neměnné úložiště (immutable storage) pro data
 * a nakonfiguruje rozvržení atributů vrcholů (pozice, normály, texturové souřadnice)
 * bez nutnosti globálního bindování objektů.
 */
void Mesh::setupMesh() {
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);

    glNamedBufferStorage(VBO, vertices.size() * sizeof(Vertex), vertices.data(), 0);
    glNamedBufferStorage(EBO, indices.size() * sizeof(unsigned int), indices.data(), 0);

    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
    glVertexArrayElementBuffer(VAO, EBO);

    glEnableVertexArrayAttrib(VAO, 0);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
    glVertexArrayAttribBinding(VAO, 0, 0);

    glEnableVertexArrayAttrib(VAO, 1);
    glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
    glVertexArrayAttribBinding(VAO, 1, 0);

    glEnableVertexArrayAttrib(VAO, 2);
    glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, TexCoords));
    glVertexArrayAttribBinding(VAO, 2, 0);
}

/**
 * @brief Vykreslí síť pomocí aktuálně aktivního shaderu.
 * * Naváže VAO a provede indexované vykreslení trojúhelníků. Pokud objekt
 * nevlastní platné VAO (např. po přesunu), metoda neprovede žádnou operaci.
 */
void Mesh::Draw() const {
    if (VAO == 0) return;

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}