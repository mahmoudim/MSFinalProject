#include "stdafx.h"

#define graphPath "../graph.g"
#define SymGraphPath "SymGraph.g"


int main(int argc, char *argv[]) {

    float alpha = 0.5, betha = 0.5, proneTreshold = 0.025;

    int numthreads=4;
#pragma region getargs

    if(argc>=2) {
        std::string Salpha = std::string(argv[1]);
        alpha = std::stof(Salpha);
        printf("alpha: %f\n",alpha);
    }
    if(argc>=3) {
        std::string Sbetha = std::string(argv[2]);
        betha = std::stof(Sbetha);
        printf("betha: %f\n",betha);
    }
    if(argc>=4) {
        std::string Sprone = std::string(argv[3]);
        proneTreshold=std::stof(Sprone);
        printf("proneTreshold: %f\n",proneTreshold);
    }
    if(argc>=5) {
        std::string Snumthreads = std::string(argv[4]);
        numthreads=std::stoi(Snumthreads);
    }

#pragma endregion

    Eigen::setNbThreads(numthreads);
    omp_set_num_threads(numthreads);

    typedef PNGraph PGraph;
    PGraph G = PGraph::TObj::New();
    PNGraph F;


    std::map<std::string, int> list;
    std::map<int, std::string> listi;

    FILE *file = fopen(graphPath, "r");


    char source[10], dest[10];


    int count = 0;
    while (fscanf(file, "%s ==> %s\n", source, dest) != EOF) {
        std::string source1(source), dest1(dest);
        if (list.find(source1) == list.end()) {
            list.insert(std::pair<std::string, int>(source1, count));
            listi.insert(std::pair<int, std::string>(count, source1));
            G->AddNode(count++);
        }

        if (list.find(dest1) == list.end()) {
            list.insert(std::pair<std::string, int>(dest1, count));
            listi.insert(std::pair<int, std::string>(count, dest1));
            G->AddNode(count++);
        }
        G->AddEdge(list[source1], list[dest1]);
    }
    F = TSnap::GetMxWcc(G);

    TIntV lis;

    //degree discounted

    Eigen::SparseMatrix<double> g = SymSnap::DegreeDiscounted(F, alpha, betha, proneTreshold);

    SymSnap::PrintSym(g,listi,SymGraphPath);

    return 0;
}
