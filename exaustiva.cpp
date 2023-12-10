
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <omp.h>
using namespace std;

struct GrafoStruct {
    vector<vector<int>> adjList;
    int numVertices;
};

GrafoStruct LerGrafo(const std::string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    int numVertices, numArestas;
    arquivo >> numVertices >> numArestas;

    GrafoStruct grafoReturn;
    grafoReturn.numVertices = numVertices;
    grafoReturn.adjList.resize(numVertices);

    for (int i = 0; i < numArestas; ++i) {
        int u, v;
        arquivo >> u >> v;
        u--; v--; // Ajustar índices para base 0
        grafoReturn.adjList[u].push_back(v);
        grafoReturn.adjList[v].push_back(u);
    }

    arquivo.close();
    return grafoReturn;
}

bool isClique(const vector<int>& clique, const vector<vector<int>>& adjList) {
    for (size_t i = 0; i < clique.size(); i++) {
        for (size_t j = i + 1; j < clique.size(); j++) {
            if (find(adjList[clique[i]].begin(), adjList[clique[i]].end(), clique[j]) == adjList[clique[i]].end())
                return false;
        }
    }
    return true;
}

vector<int> EncontrarCliqueMaxima(const vector<vector<int>>& adjList, vector<int>& cand, vector<int>& clique, vector<int>& CM) {
    if (cand.empty()) {
        if (clique.size() > CM.size()) {
            CM = clique;
        }
        return CM;
    }

    // Poda: Se o tamanho do maior clique possível com os candidatos restantes + clique atual é menor que o tamanho do clique máximo encontrado até agora, retorna.
    if (clique.size() + cand.size() <= CM.size()) return CM;

    for (size_t i = 0; i < cand.size(); i++) {
        vector<int> newClique = clique;
        newClique.push_back(cand[i]);

        if (isClique(newClique, adjList)) {
            vector<int> newCand;
            for (size_t j = i + 1; j < cand.size(); j++) {
                if (find(adjList[cand[i]].begin(), adjList[cand[i]].end(), cand[j]) != adjList[cand[i]].end()) {
                    newCand.push_back(cand[j]);
                }
            }
            CM = EncontrarCliqueMaxima(adjList, newCand, newClique, CM);
        }
    }
    return CM;
}

int main() {
    double tempo = omp_get_wtime();

    GrafoStruct grafoPacote = LerGrafo("grafo.txt");
    vector<vector<int>> adjList = grafoPacote.adjList;
    int numVertices = grafoPacote.numVertices;

    vector<int> cand(numVertices);
    iota(cand.begin(), cand.end(), 0); // Inicializa com os índices dos vértices

    vector<int> clique, CM;
    CM = EncontrarCliqueMaxima(adjList, cand, clique, CM);

    tempo = omp_get_wtime() - tempo;

    // Abra um arquivo de saída
    std::ofstream arquivo_saida("saida.txt");

    // Verifique se o arquivo foi aberto com sucesso
    if (arquivo_saida.is_open()) {
        arquivo_saida << tempo << endl;
        //arquivo_saida << "Tamanho: " << CM.size() << std::endl;
        //arquivo_saida << "Clique Máxima: " << std::endl;

        // Escreva no arquivo em vez de imprimir no console
        for (size_t v : CM) {
            arquivo_saida << v + 1 << endl;
        }

        // Feche o arquivo quando terminar
        arquivo_saida.close();
    } else {
        // Se o arquivo não pôde ser aberto, imprima uma mensagem de erro
        std::cerr << "Erro ao abrir o arquivo de saída." << std::endl;
    }

    return 0;
}
