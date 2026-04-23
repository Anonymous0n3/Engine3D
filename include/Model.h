// Model.h
#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include <vector>
#include <string>

/**
 * @class Model
 * @brief Třída pro načítání, správu a vykreslování 3D modelů.
 *
 * Využívá knihovnu Assimp (Open Asset Import Library) pro načtení modelu
 * z různých formátů (např. .obj, .fbx, .gltf) a převede jej do struktury
 * složené z jedné nebo více instancí třídy Mesh.
 */
class Model {
public:
    /**
     * @brief Kontejner obsahující všechny sítě (meshes), ze kterých se model skládá.
     */
    std::vector<Mesh> meshes;

    /**
     * @brief Cesta ke složce, ve které se nachází soubor s modelem.
     *
     * Tato cesta se využívá především k relativnímu dohledávání a načítání
     * přidružených textur modelu.
     */
    std::string directory;

    /**
     * @brief Konstruktor třídy Model.
     *
     * Zahájí proces načítání modelu ze zadané cesty k souboru.
     *
     * @param path Absolutní nebo relativní cesta k souboru s 3D modelem.
     */
    Model(const std::string& path);

    /**
     * @brief Vykreslí celý 3D model.
     *
     * Projde všechny sítě (meshes) uložené v tomto modelu a zavolá jejich
     * příslušné vykreslovací funkce.
     */
    void Draw() const;

private:
    /**
     * @brief Interní funkce pro načtení modelu pomocí knihovny Assimp.
     *
     * Inicializuje Assimp Importer, načte scénu ze souboru a aplikuje
     * základní post-processing (např. triangulaci).
     *
     * @param path Cesta k souboru s modelem.
     */
    void loadModel(const std::string& path);

    /**
     * @brief Rekurzivně zpracovává uzly (nodes) v hierarchii Assimp scény.
     *
     * Projde zadaný uzel, převede všechny jeho sítě (meshes) a následně
     * rekurzivně zavolá sama sebe na všechny jeho potomky.
     *
     * @param node Ukazatel na aktuálně zpracovávaný Assimp uzel.
     * @param scene Ukazatel na celkovou Assimp scénu obsahující všechna data.
     */
    void processNode(aiNode* node, const aiScene* scene);

    /**
     * @brief Převede datovou strukturu Assimp sítě (`aiMesh`) na naši třídu `Mesh`.
     *
     * Extrahuje vertexy (pozice, normály, texturové souřadnice), indexy
     * a materiály/textury z Assimp formátu a vytvoří z nich instanci třídy Mesh.
     *
     * @param mesh Ukazatel na Assimp síť, která má být převedena.
     * @param scene Ukazatel na celkovou Assimp scénu.
     * @return Zkompletovaná instance naší vlastní třídy Mesh.
     */
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};