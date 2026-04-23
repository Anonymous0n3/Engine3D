// Texture.h
#pragma once
#include <glad/glad.h>
#include <string>

/**
 * @class Texture
 * @brief Třída pro reprezentaci a správu OpenGL textury.
 *
 * Poskytuje rozhraní pro načtení obrazových dat, vytvoření textury v paměti
 * grafické karty a její následné navázání pro použití při vykreslování.
 */
class Texture {
public:
    /**
     * @brief OpenGL identifikátor (handle) textury.
     */
    GLuint ID;

    /**
     * @brief Cesta k souboru, ze kterého byla textura načtena.
     */
    std::string path;

    /**
     * @brief Konstruktor třídy Texture.
     *
     * Pokusí se načíst obrázek ze zadané cesty a vygeneruje z něj OpenGL texturu.
     *
     * @param imagePath Cesta k souboru s obrázkem.
     */
    Texture(const char* imagePath);

    /**
     * @brief Destruktor třídy Texture.
     *
     * Smaže texturu a uvolní prostředky alokované v paměti grafické karty.
     */
    ~Texture();

    /**
     * @brief Naváže (nabinduje) texturu do specifikovaného texturovacího slotu.
     *
     * Aktivuje texturovou jednotku a naváže na ni tuto texturu, aby s ní mohl
     * pracovat fragment shader během kreslení.
     *
     * @param slot Číslo texturovací jednotky (výchozí hodnota je 0).
     */
    void Bind(GLuint slot = 0) const;
};