#pragma once
#include <string>

struct AppConfig 
{
    std::string inputFile;
    std::string outputFile = "model.obj"; // Nome padrão caso o usuário não passe o -o
    bool isValid = false;
    bool showHelp = false;
};

class CLIParser {
public:
    AppConfig parse(int argc, char* argv[]);
    void printHelp() const;
};