#pragma once
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class MeshIO 
{
public:
    const aiScene* load(const std::string& filepath, Assimp::Importer& importer);

    bool save(const std::string& filepath, const aiScene* scene);
};