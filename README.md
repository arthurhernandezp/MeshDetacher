# MeshDetacher

Um utilitário de linha de comando (CLI) em C++  para processar arquivos `.obj` e separar malhas 3D desconectadas que estão agrupadas em um único objeto.

##  Funcionalidades

* **Separação Automática:** Detecta e isola partes da malha que não compartilham vértices (componentes conexas).
* **Otimização de Geometria:** Funde vértices duplicados na mesma coordenada espacial, reduzindo o tamanho do arquivo sem alterar o número de triângulos.
* **Saída Estruturada:** Exporta um novo `.obj` onde cada componente conexa é separada por tags de grupo (`g Objeto_1`, `g Objeto_2`, etc.).
* **Stand-alone:** Utiliza a biblioteca **Assimp** compilada estaticamente via submódulo. Não requer instalação de DLLs ou dependências no sistema operacional.

## Como Funciona (O Algoritmo)


1. **Leitura e Limpeza:** A malha é carregada via Assimp, ignorando normais e UVs para focar puramente na estrutura espacial. Vértices sobrepostos são soldados (`aiProcess_JoinIdenticalVertices`).
2. **Mapeamento (Adjacency List):** É criado um mapa que relaciona cada vértice a todas as faces (triângulos) que o utilizam.
3. **Contágio (Busca em Largura / BFS):** O algoritmo escolhe um triângulo aleatório e, usando uma fila (queue) e escolhe todos os triângulos vizinhos que compartilham os mesmos vértices. Ao não encontrar mais vizinhos o algoritmo para, assim a componente conexa foi encontrada.
4. **Reconstrução:** O processo se repete para as faces não visitadas. No final, os índices globais do `.obj` são recalculados localmente para cada componente conexa.

## 🛠️ Como Compilar

Este projeto utiliza CMake e requer um compilador com suporte a **C++17**.

Como a biblioteca Assimp está incluída como um submódulo do Git, é crucial clonar o repositório com a flag `--recursive`:

```bash
# 1. Clone o repositório com os submódulos
git clone --recursive <url-do-repositorio>
cd MeshDetacher

# 2. Crie a pasta de build
mkdir build
cd build

# 3. Gere os arquivos do CMake e compile (Release mode recomendado para velocidade)
cmake ..
cmake --build . --config Release
```

## Como Usar
Via terminal, chame o executável passando o arquivo de entrada e o de saída:

```Bash
./MeshDetacher -i modelo_original.obj -o resultado.obj
```

**Opções disponíveis:**
* `-i, --input <arquivo>`: Caminho para o `.obj` original (Obrigatório).
* `-o, --output <arquivo>`: Caminho para salvar o novo `.obj` (Opcional, padrão: `output_separated.obj`).
* `-h, --help`: Mostra o menu de ajuda.

## 📝 Licença

Este projeto é de código aberto