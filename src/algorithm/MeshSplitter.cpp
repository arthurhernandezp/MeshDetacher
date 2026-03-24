#include "MeshSplitter.hpp"
#include <queue>
#include <unordered_map>
#include <iostream>
#include <cstdint>

std::vector<aiMesh*> MeshSplitter::splitMesh(const aiMesh* mesh) 
{
    std::vector<aiMesh*> separatedMeshes;
    if (!mesh || mesh->mNumFaces == 0) return separatedMeshes;

    std::vector<std::vector<unsigned int>> vertexToFaces(mesh->mNumVertices);
    for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
        const aiFace& face = mesh->mFaces[f];
        for (unsigned int i = 0; i < face.mNumIndices; ++i) {
            vertexToFaces[face.mIndices[i]].push_back(f);
        }
    }

    // --- BFS ---
    std::vector<uint8_t> visitedFaces(mesh->mNumFaces, 0);
    std::vector<std::vector<unsigned int>> islands; 

    for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
        if (visitedFaces[f]) continue; // Essa face ja foi encontrada em

        std::vector<unsigned int> currentIsland;
        std::queue<unsigned int> q;
        
        q.push(f);
        visitedFaces[f] = true;

        while (!q.empty()) {
            unsigned int currentFace = q.front();
            q.pop();
            currentIsland.push_back(currentFace);

            const aiFace& face = mesh->mFaces[currentFace];
            for (unsigned int i = 0; i < face.mNumIndices; ++i) 
            {
                unsigned int v = face.mIndices[i];
                
                // Pega todas as faces coladas nesse vértice
                for (unsigned int adjFace : vertexToFaces[v]) 
                {
                    if (!visitedFaces[adjFace]) 
                    {
                        visitedFaces[adjFace] = true;
                        q.push(adjFace);
                    }
                }
            }
        }
        islands.push_back(currentIsland);
    }

    std::cout << "Encontrados " << islands.size() << " objetos separados na malha.\n";

    for (const auto& islandFaces : islands) 
    {
        aiMesh* newMesh = new aiMesh();
        newMesh->mMaterialIndex = mesh->mMaterialIndex;

        // Mapa para recalcular os índices dos vértices do zero (0, 1, 2...)
        std::unordered_map<unsigned int, unsigned int> oldToNewVertex;
        std::vector<unsigned int> newVerticesList; 

        // Descobre quais vértices a nova malha precisa
        for (unsigned int faceIdx : islandFaces) {
            const aiFace& oldFace = mesh->mFaces[faceIdx];
            for (unsigned int i = 0; i < oldFace.mNumIndices; ++i) {
                unsigned int oldV = oldFace.mIndices[i];
                if (oldToNewVertex.find(oldV) == oldToNewVertex.end()) {
                    oldToNewVertex[oldV] = newVerticesList.size();
                    newVerticesList.push_back(oldV);
                }
            }
        }

        // Aloca a memória na Assimp para os vértices do novo objeto
        newMesh->mNumVertices = newVerticesList.size();
        newMesh->mVertices = new aiVector3D[newMesh->mNumVertices];
        if (mesh->HasNormals()) {
            newMesh->mNormals = new aiVector3D[newMesh->mNumVertices];
        }

        // Copia os dados reais (x, y, z)
        for (size_t i = 0; i < newVerticesList.size(); ++i) {
            unsigned int oldV = newVerticesList[i];
            newMesh->mVertices[i] = mesh->mVertices[oldV];
            if (mesh->HasNormals()) newMesh->mNormals[i] = mesh->mNormals[oldV];
        }

        // Reconstrói as faces com os índices recalculados
        newMesh->mNumFaces = islandFaces.size();
        newMesh->mFaces = new aiFace[newMesh->mNumFaces];
        for (size_t i = 0; i < islandFaces.size(); ++i) {
            const aiFace& oldFace = mesh->mFaces[islandFaces[i]];
            aiFace& newFace = newMesh->mFaces[i];
            
            newFace.mNumIndices = oldFace.mNumIndices;
            newFace.mIndices = new unsigned int[newFace.mNumIndices];
            
            for (unsigned int j = 0; j < oldFace.mNumIndices; ++j) 
            {
                newFace.mIndices[j] = oldToNewVertex[oldFace.mIndices[j]]; 
            }
        }

        separatedMeshes.push_back(newMesh);
    }

    return separatedMeshes;
}

// Orquestra a criação de uma nova cena baseada na antiga
aiScene* MeshSplitter::splitScene(const aiScene* originalScene) 
{
    if (!originalScene || !originalScene->mRootNode) return nullptr;

    aiScene* newScene = new aiScene();
    
    // Coletamos todos os pedaços de todas as malhas originais
    std::vector<aiMesh*> allNewMeshes;
    for (unsigned int i = 0; i < originalScene->mNumMeshes; ++i) {
        std::vector<aiMesh*> subMeshes = splitMesh(originalScene->mMeshes[i]);
        allNewMeshes.insert(allNewMeshes.end(), subMeshes.begin(), subMeshes.end());
    }

    // Atribui as novas malhas à cena
    newScene->mNumMeshes = allNewMeshes.size();
    newScene->mMeshes = new aiMesh*[newScene->mNumMeshes];
    for (size_t i = 0; i < allNewMeshes.size(); ++i) 
    {
        // Dando um nome único para aparecer certinho no arquivo .obj final
        allNewMeshes[i]->mName = aiString("Objeto_" + std::to_string(i + 1));
        newScene->mMeshes[i] = allNewMeshes[i];
    }

    // A Assimp exige que a cena tenha um nó raiz (Root Node) para ser exportada
    newScene->mRootNode = new aiNode();
    newScene->mRootNode->mName = aiString("Root");
    newScene->mRootNode->mNumMeshes = newScene->mNumMeshes;
    newScene->mRootNode->mMeshes = new unsigned int[newScene->mNumMeshes];
    for (unsigned int i = 0; i < newScene->mNumMeshes; ++i) {
        newScene->mRootNode->mMeshes[i] = i;
    }

    // Copia os materiais originais 
    if (originalScene->mNumMaterials > 0) {
        newScene->mNumMaterials = originalScene->mNumMaterials;
        newScene->mMaterials = new aiMaterial*[newScene->mNumMaterials];
        for (unsigned int i = 0; i < newScene->mNumMaterials; ++i) {
            aiMaterial* mat = new aiMaterial();
            originalScene->mMaterials[i]->CopyPropertyList(mat, originalScene->mMaterials[i]);
            newScene->mMaterials[i] = mat;
        }
    }

    return newScene;
}