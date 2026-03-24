cmake_minimum_required(VERSION 3.15)
project(MeshDetacher VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define os arquivos fonte
set(SOURCES
    src/main.cpp
    src/cli/CLIParser.cpp
    src/io/ObjLoader.cpp
    src/processing/MeshProcessor.cpp
)

# Cria o executável
add_executable(${PROJECT_NAME} ${SOURCES})

# Adiciona o diretório src aos includes para facilitar os imports
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)