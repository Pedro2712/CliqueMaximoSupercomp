
#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <bits/stdc++.h>
#include <algorithm>
#include <omp.h>
using namespace std;


struct GrafoStruct {
    vector<vector<int>> grafo;
    int numVertices;
};


GrafoStruct LerGrafo(const std::string& nomeArquivo) {
    std::ifstream arquivo(nomeArquivo);
    int numVertices;
    int numArestas;
    arquivo >> numVertices >> numArestas;

    GrafoStruct grafoReturn;
    grafoReturn.numVertices = numVertices;
    grafoReturn.grafo.resize(numVertices, std::vector<int>(numVertices, 0));

    for (int i = 0; i < numArestas; ++i) {
        int u, v;
        arquivo >> u >> v;
        grafoReturn.grafo[u - 1][v - 1] = 1;
        grafoReturn.grafo[v - 1][u - 1] = 1;  // O grafo é não direcionado
    }

    arquivo.close();

    return grafoReturn;
}


vector<int> EncontrarCliqueMaxima(vector<vector<int>>& grafo, int numVertices) {
    vector<int> cliqueMaxima;
    vector<int> candidatos;

    for (int i = 0; i < numVertices; ++i) {
        candidatos.push_back(i);
    }

    while (!candidatos.empty()) {
        int v = candidatos.back();
        candidatos.pop_back();

        bool podeAdicionar = true;

        for (int u : cliqueMaxima) {
            if (grafo[u][v] == 0) {
                podeAdicionar = false;
                break;
            }
        }

        if (podeAdicionar) {
            cliqueMaxima.push_back(v);
            vector<int> novosCandidatos;

            for (int u : candidatos) {
                bool adjacenteATodos = true;

                for (int c : cliqueMaxima) {
                    if (grafo[u][c] == 0) {
                        adjacenteATodos = false;
                        break;
                    }
                }

                if (adjacenteATodos) {
                    novosCandidatos.push_back(u);
                }
            }

            candidatos = novosCandidatos;
        }
    }

    return cliqueMaxima;
}


int main() {
    GrafoStruct grafoPacote;
    grafoPacote = LerGrafo("grafo.txt");
    int numVertices = grafoPacote.numVertices;
    vector<vector<int>> grafo = grafoPacote.grafo;

    vector<int> resultado = EncontrarCliqueMaxima(grafo, numVertices);

    // Imprime a clique máxima encontrada
    cout << "Tamanho: " << resultado.size() << endl;
    sort(resultado.begin(), resultado.end());
    cout << "Clique Máxima: " << endl;
    for (int v : resultado) {
        cout << v << " ";
    }
    cout << endl;

    return 0;
}
