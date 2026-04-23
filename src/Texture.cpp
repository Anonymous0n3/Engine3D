// Texture.cpp
#include "Texture.h"
#include <stb_image.h>
#include <iostream>
#include <cmath>
#include <algorithm>

/**
 * @brief Konstruktor třídy Texture.
 *
 * Provádí načtení obrázku z disku pomocí knihovny stb_image. Následně vyhodnotí
 * počet barevných kanálů a podle toho zvolí interní a datový formát (GL_RED, GL_RGB, GL_RGBA).
 * Využívá moderní OpenGL Direct State Access (DSA) pro vytvoření textury, alokaci
 * neměnné paměti (immutable storage), nahrání dat na GPU a vygenerování mipmap.
 * Nakonec nastaví parametry pro filtrování a opakování textury.
 *
 * @param imagePath Cesta k souboru s obrázkem, ze kterého se má textura vytvořit.
 */
Texture::Texture(const char* imagePath) : path(imagePath) {
    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);

    if (data) {
        GLenum internalFormat = 0;
        GLenum dataFormat = 0;

        if (nrChannels == 1) {
            internalFormat = GL_R8;
            dataFormat = GL_RED;
        }
        else if (nrChannels == 3) {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }
        else if (nrChannels == 4) {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &ID);

        GLsizei mipLevels = static_cast<GLsizei>(std::floor(std::log2(std::max(width, height)))) + 1;

        glTextureStorage2D(ID, mipLevels, internalFormat, width, height);

        glTextureSubImage2D(ID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

        glGenerateTextureMipmap(ID);

        glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cerr << "Texture failed to load at path: " << imagePath << std::endl;
        stbi_image_free(data);
    }
}

/**
 * @brief Destruktor třídy Texture.
 *
 * Zavolá OpenGL funkci glDeleteTextures pro bezpečné uvolnění textury z paměti grafické karty.
 */
Texture::~Texture() {
    glDeleteTextures(1, &ID);
}

/**
 * @brief Aktivuje a naváže texturu na určený texturovací slot.
 *
 * Využívá moderní DSA funkci glBindTextureUnit, která nahrazuje starší přístup
 * vyžadující volání glActiveTexture a glBindTexture zvlášť.
 *
 * @param slot Číslo texturovací jednotky, na kterou se má textura navázat (výchozí hodnota je 0).
 */
void Texture::Bind(GLuint slot) const {
    glBindTextureUnit(slot, ID);
}