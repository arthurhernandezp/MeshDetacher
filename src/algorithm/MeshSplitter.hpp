#pragma once
#include <vector>
#include <assimp/scene.h>

class MeshSplitter 
{
public:
    // Ponto de entrada: pega a cena original e cria uma nova com os objetos divididos
    aiScene* splitScene(const aiScene* originalScene);

private:
    // A lógica de grafos que atua em uma única malha gigante e a quebra em várias menores
    std::vector<aiMesh*> splitMesh(const aiMesh* mesh);
};