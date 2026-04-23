// Config.h
#pragma once
#include "json.hpp"
#include <fstream>
#include <string>

using json = nlohmann::json;

struct Config {
    bool vsync = true;
    bool msaa = true;
    bool fullscreen = false;
    bool simpleFlashlight = false;
    float masterVolume = 1.0f;
    int windowWidth = 1280;
    int windowHeight = 720;

    void load(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            // File doesn't exist yet, just use defaults
            return;
        }
        try {
            json j;
            file >> j;
            vsync = j.value("vsync", vsync);
            msaa = j.value("msaa", msaa);
            fullscreen = j.value("fullscreen", fullscreen);
            simpleFlashlight = j.value("simpleFlashlight", simpleFlashlight);
            masterVolume = j.value("masterVolume", masterVolume);
            windowWidth = j.value("windowWidth", windowWidth);
            windowHeight = j.value("windowHeight", windowHeight);
        }
        catch (const json::exception& e) {
            // Corrupt file — silently fall back to defaults
            std::cerr << "Config load error: " << e.what() << std::endl;
        }
    }

    void save(const std::string& path) const {
        json j;
        j["vsync"] = vsync;
        j["msaa"] = msaa;
        j["fullscreen"] = fullscreen;
        j["simpleFlashlight"] = simpleFlashlight;
        j["masterVolume"] = masterVolume;
        j["windowWidth"] = windowWidth;
        j["windowHeight"] = windowHeight;

        std::ofstream file(path);
        if (file.is_open())
            file << j.dump(4); // 4 = indent spaces, makes it human readable
    }
};