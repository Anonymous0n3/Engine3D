// Audio.h
#pragma once
#include "miniaudio.h"
#include <glm/glm.hpp>
#include <string>
#include <iostream>

/**
 * @class AudioEngine
 * @brief Třída reprezentující hlavní jádro pro zpracování zvuku.
 *
 * Zajišťuje inicializaci a ukončení běhu audio enginu pomocí knihovny miniaudio.
 * Rovněž se stará o aktualizaci pozice a orientace posluchače (listenera) ve 3D prostoru.
 */
class AudioEngine {
public:
    /**
     * @brief Instance miniaudio enginu.
     */
    ma_engine engine;

    /**
     * @brief Konstruktor třídy AudioEngine.
     *
     * Pokusí se inicializovat miniaudio engine. V případě selhání vypíše chybové
     * hlášení do standardního chybového výstupu.
     */
    AudioEngine() {
        ma_result result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS) {
            std::cerr << "Nepodarilo se inicializovat Audio Engine!" << std::endl;
        }
    }

    /**
     * @brief Destruktor třídy AudioEngine.
     *
     * Bezpečně uvolní zdroje alokované pro miniaudio engine.
     */
    ~AudioEngine() {
        ma_engine_uninit(&engine);
    }

    /**
     * @brief Aktualizuje pozici a orientaci posluchače ve 3D prostoru.
     *
     * @param position Aktuální pozice posluchače (např. pozice kamery).
     * @param front Vektor směru, kam posluchač hledí.
     * @param up Vektor určující směr "nahoru" vůči posluchači.
     */
    void UpdateListener(glm::vec3 position, glm::vec3 front, glm::vec3 up) {
        ma_engine_listener_set_position(&engine, 0, position.x, position.y, position.z);
        ma_engine_listener_set_direction(&engine, 0, front.x, front.y, front.z);
        ma_engine_listener_set_world_up(&engine, 0, up.x, up.y, up.z);
    }
};

/**
 * @class Sound3D
 * @brief Třída pro správu a přehrávání konkrétního 3D zvukového zdroje.
 *
 * Umožňuje načíst zvuk ze souboru, nastavit jeho pozici, hlasitost a chování
 * ve 3D prostoru (spatialization, útlum zvuku na základě vzdálenosti).
 */
class Sound3D {
public:
    /**
     * @brief Instance zvuku z knihovny miniaudio.
     */
    ma_sound sound;

    /**
     * @brief Konstruktor třídy Sound3D.
     *
     * Načte zvukový soubor, nastaví případné smyčkování a inicializuje fyzikální
     * vlastnosti pro 3D prostor (útlum zvuku a minimální vzdálenost).
     *
     * @param audio Reference na instanci AudioEngine, pod kterou zvuk spadá.
     * @param filepath Cesta ke zvukovému souboru.
     * @param looping Určuje, zda se má zvuk přehrávat ve smyčce (výchozí hodnota je true).
     */
    Sound3D(AudioEngine& audio, const std::string& filepath, bool looping = true) {
        ma_result result = ma_sound_init_from_file(&audio.engine, filepath.c_str(), 0, NULL, NULL, &sound);
        if (result != MA_SUCCESS) {
            std::cerr << "Nepodarilo se nacist zvuk: " << filepath << std::endl;
        }
        ma_sound_set_looping(&sound, looping ? MA_TRUE : MA_FALSE);

        ma_sound_set_spatialization_enabled(&sound, MA_TRUE);
        ma_sound_set_rolloff(&sound, 2.0f);
        ma_sound_set_min_distance(&sound, 1.0f);
    }

    /**
     * @brief Destruktor třídy Sound3D.
     *
     * Uvolní zdroje spojené s tímto zvukovým objektem.
     */
    ~Sound3D() {
        ma_sound_uninit(&sound);
    }

    /**
     * @brief Nastaví novou pozici zdroje zvuku ve 3D prostoru.
     *
     * @param pos Vektor obsahující nové souřadnice (x, y, z).
     */
    void SetPosition(glm::vec3 pos) {
        ma_sound_set_position(&sound, pos.x, pos.y, pos.z);
    }

    /**
     * @brief Nastaví hlasitost přehrávání zvuku.
     *
     * @param volume Úroveň hlasitosti (0.0f představuje ticho, 1.0f je 100% hlasitost,
     * hodnoty nad 1.0f zvuk uměle zesílí).
     */
    void SetVolume(float volume) {
        ma_sound_set_volume(&sound, volume);
    }

    void SetSpatialization(bool enabled) {
        ma_sound_set_spatialization_enabled(&sound, enabled ? MA_TRUE : MA_FALSE);
    }

    void SetMinDistance(float distance) {
        ma_sound_set_min_distance(&sound, distance);
    }

    /**
     * @brief Spustí přehrávání zvuku.
     */
    void Play() {
        ma_sound_start(&sound);
    }

    /**
     * @brief Zastaví přehrávání zvuku.
     */
    void Stop() {
        ma_sound_stop(&sound);
    }
};