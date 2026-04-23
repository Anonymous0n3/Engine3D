// stb_image.cpp

/**
 * @file stb_image.cpp
 * @brief Zdrojový soubor pro inicializaci a kompilaci implementace knihovny stb_image.
 *
 * Knihovna stb_image je distribuována jako tzv. "single-header library".
 * Aby správně fungovala a linker nenašel vícenásobné definice, je nutné
 * definovat makro STB_IMAGE_IMPLEMENTATION přesně v jednom zdrojovém (.cpp)
 * souboru celého projektu ještě před vložením samotné hlavičky.
 * Tento soubor plní přesně tuto roli a generuje zkompilovaný kód pro načítání textur.
 */

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>