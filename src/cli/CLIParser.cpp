#include "CLIParser.hpp"
#include <iostream>
#include <string_view>

AppConfig CLIParser::parse(int argc, char* argv[]) 
{
    AppConfig config;

    if (argc <= 1) {
        config.showHelp = true;
        return config;
    }

    for (int i = 1; i < argc; ++i) 
    {
        std::string_view arg = argv[i];

        if (arg == "-h" || arg == "--help") 
        {
            config.showHelp = true;
            return config;
        } 
        else if (arg == "-i" || arg == "--input") 
        {
            if (i + 1 < argc) 
            {
                config.inputFile = argv[++i];
            } else 
            {
                std::cerr << "Erro: Faltou especificar o arquivo após " << arg << "\n";
                return config;
            }
        } 
        else if (arg == "-o" || arg == "--output") 
        {
            if (i + 1 < argc) 
            {
                config.outputFile = argv[++i];
            } else 
            {
                std::cerr << "Erro: Faltou especificar o arquivo após " << arg << "\n";
                return config;
            }
        }
    }

    // A configuração só é válida se tivermos um arquivo de entrada
    if (!config.inputFile.empty()) 
    {
        config.isValid = true;
    } else 
    {
        std::cerr << "Erro: Arquivo de entrada não especificado.\n";
    }

    return config;
}

void CLIParser::printHelp() const 
{
    std::cout << "Uso: obj-splitter [opcoes]\n\n"
              << "Opcoes:\n"
              << "  -i, --input <arquivo>   Caminho para o arquivo .obj original (Obrigatório)\n"
              << "  -o, --output <arquivo>  Caminho para salvar o novo .obj (Opcional, padrao: output_separated.obj)\n"
              << "  -h, --help              Mostra esta mensagem de ajuda\n\n"
              << "Exemplo:\n"
              << "  ./obj-splitter -i cena_inteira.obj -o cena_separada.obj\n";
}