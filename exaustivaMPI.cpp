
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <omp.h>
#include <mpi.h>
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

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    GrafoStruct grafoPacote;
    if (world_rank == 0) {
        grafoPacote = LerGrafo("grafo.txt");
    }

    // Broadcast do número de vértices para todos os processos
    MPI_Bcast(&grafoPacote.numVertices, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Inicializar listas de adjacência para todos os processos
    grafoPacote.adjList.resize(grafoPacote.numVertices);

    // ... [Código para distribuir a lista de adjacências entre processos] ...

    int partSize = grafoPacote.numVertices / world_size;
    int start = world_rank * partSize;
    int end = (world_rank == world_size - 1) ? grafoPacote.numVertices : (world_rank + 1) * partSize;

    vector<int> cand(grafoPacote.numVertices);
    iota(cand.begin(), cand.end(), 0);

    vector<int> clique, CM;
    CM = EncontrarCliqueMaxima(grafoPacote.adjList, cand, clique, CM);

    // Processo raiz coleta cliques máximos de todos os processos
    if (world_rank == 0) {
        vector<int> globalCM = CM;
        for (int i = 1; i < world_size; ++i) {
            int tempCMSize;
            MPI_Status status;

            // Primeiro, recebe o tamanho do vetor CM do processo i
            MPI_Recv(&tempCMSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);

            // Agora, recebe o vetor CM do processo i
            vector<int> tempCM(tempCMSize);
            MPI_Recv(&tempCM[0], tempCMSize, MPI_INT, i, 0, MPI_COMM_WORLD, &status);

            if (tempCM.size() > globalCM.size()) {
                globalCM = tempCM;
            }
        }

        // Exibir clique máximo global
        cout << "Clique máximo global: ";
        for (int v : globalCM) {
            cout << v + 1 << " ";
        }
        cout << endl;
    } else {
        int CMSize = CM.size();

        // Primeiro, envia o tamanho do vetor CM
        MPI_Send(&CMSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        // Agora, envia o vetor CM
        MPI_Send(&CM[0], CMSize, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
