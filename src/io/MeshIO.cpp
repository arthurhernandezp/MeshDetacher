#include "MeshIO.hpp"
#include <iostream>

const aiScene* MeshIO::load(const std::string& filepath, Assimp::Importer& importer) {
    // Assimp irá excluir os dados que não forem relacionados à Posição e Face
    importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, 
        aiComponent_NORMALS | aiComponent_TEXCOORDS | 
        aiComponent_COLORS | aiComponent_MATERIALS);

    const aiScene* scene = importer.ReadFile(filepath,
        aiProcess_RemoveComponent |       
        aiProcess_JoinIdenticalVertices |
        aiProcess_Triangulate |           
        aiProcess_SortByPType             
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cerr << "Erro no Assimp ao carregar: " << importer.GetErrorString() << "\n";
        return nullptr;
    }

    std::cout << "Arquivo carregado com sucesso: " << filepath << "\n";
    return scene;
}

bool MeshIO::save(const std::string& filepath, const aiScene* scene) 
{
    Assimp::Exporter exporter;
    
    aiReturn result = exporter.Export(scene, "obj", filepath);

    if (result != aiReturn_SUCCESS) 
    {
        std::cerr << "Erro no Assimp ao exportar: " << exporter.GetErrorString() << "\n";
        return false;
    }

    std::cout << "Arquivo salvo com sucesso: " << filepath << "\n";
    return true;
}