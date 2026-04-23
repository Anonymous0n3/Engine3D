// miniaudio.cpp

/**
 * @file miniaudio.cpp
 * @brief Zdrojový soubor pro inicializaci a kompilaci implementace knihovny miniaudio.
 * * Knihovna miniaudio je navržena jako "single-header library". To znamená, že veškerá
 * logika je obsažena v hlavičkovém souboru, ale aby byla zkompilována, musí být
 * v jednom zdrojovém souboru projektu definováno makro MINIAUDIO_IMPLEMENTATION.
 * Tento soubor slouží výhradně k tomuto účelu a vytváří spustitelný kód pro
 * audio engine, správu zvuků a nízkoúrovňovou komunikaci se zvukovým hardwarem.
 */

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"