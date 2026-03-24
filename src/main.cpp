#include <iostream>
#include "cli/CLIParser.hpp"
#include "io/MeshIO.hpp"
#include "algorithm/MeshSplitter.hpp"

int main(int argc, char* argv[]) 
{
    CLIParser cli;
    AppConfig config = cli.parse(argc, argv);

    if (config.showHelp || !config.isValid) 
    {
        cli.printHelp();
        return config.showHelp ? 0 : 1;
    }

    // Carregando o modelo (I/O)
    std::cout << "[1/3] Carregando e otimizando a malha original...\n";
    Assimp::Importer importer;
    MeshIO meshIO;
    const aiScene* originalScene = meshIO.load(config.inputFile, importer);

    if (!originalScene) 
    {
        return 1; // O erro já foi impresso pelo MeshIO
    }

    // Processando e separando os objetos (Algoritmo)
    std::cout << "[2/3] Mapeando conectividade e separando os objetos...\n";
    MeshSplitter splitter;
    aiScene* newScene = splitter.splitScene(originalScene);

    if (!newScene || newScene->mNumMeshes == 0) 
    {
        std::cerr << "Erro: Nenhuma malha valida foi gerada apos a separacao.\n";
        delete newScene;
        return 1;
    }

    // Exportando o resultado
    std::cout << "[3/3] Exportando novo arquivo...\n";
    if (!meshIO.save(config.outputFile, newScene)) 
    {
        delete newScene;
        return 1;
    }

    delete newScene;

    std::cout << "\nSucesso! Arquivo salvo em: " << config.outputFile << "\n";
    return 0;
}