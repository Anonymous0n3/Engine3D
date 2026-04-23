// Model.cpp
#include "Model.h"
#include <iostream>

/**
 * @brief Konstruktor třídy Model.
 *
 * Automaticky zahájí proces načtení 3D modelu z předané cesty voláním interní
 * metody loadModel.
 *
 * @param path Cesta k souboru s modelem (např. .obj, .fbx).
 */
Model::Model(const std::string& path) {
    loadModel(path);
}

/**
 * @brief Vykreslí celý model.
 *
 * Projde všechny sítě (meshes) uložené v tomto modelu a u každé z nich zavolá
 * příslušnou vykreslovací metodu Draw.
 */
void Model::Draw() const {
    for (const Mesh& mesh : meshes) {
        mesh.Draw();
    }
}

/**
 * @brief Provede samotné načtení modelu z disku.
 *
 * Využívá knihovnu Assimp pro import souboru, aplikuje na něj základní
 * post-processingové operace (triangulace, spojení duplicitních vrcholů, generování normál)
 * a zpracuje kořenový uzel scény. Po úspěšném načtení dat do RAM provede
 * inicializaci OpenGL bufferů pro všechny vygenerované sítě nahráním dat na GPU.
 *
 * @param path Cesta k souboru s modelem.
 */
void Model::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenSmoothNormals
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);

    for (Mesh& mesh : meshes) {
        mesh.setupMesh();
    }
}

/**
 * @brief Zpracovává hierarchii uzlů v Assimp scéně.
 *
 * Extrahují se sítě přidělené danému uzlu a po jejich zpracování se funkce
 * rekurzivně volá na všechny potomky (children) tohoto uzlu.
 *
 * @param node Ukazatel na právě zpracovávaný Assimp uzel.
 * @param scene Ukazatel na celkovou importovanou Assimp scénu.
 */
void Model::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

/**
 * @brief Konvertuje strukturu sítě z Assimpu do formátu naší aplikace.
 *
 * Přečte veškerá potřebná data ze struktury aiMesh (pozice, normály a texturové
 * souřadnice) a uspořádá je do kolekcí vrcholů (vertices) a indexů (indices).
 *
 * @param mesh Ukazatel na importovanou Assimp síť.
 * @param scene Ukazatel na celkovou Assimp scénu (používá se např. pro přístup k materiálům).
 * @return Nově vytvořená instance třídy Mesh naplněná daty.
 */
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        if (mesh->HasNormals()) {
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        else {
            vertex.Normal = glm::vec3(0.0f);
        }

        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    return Mesh(vertices, indices);
}