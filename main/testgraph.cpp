#include "stdafx.h"

#define SymGraphPath "SymGraph.g"


int main(int argc, char *argv[]) {


    int numthreads=4;
#pragma region getargs

    if(argc>=2) {
        std::string Salpha = std::string(argv[1]);
        alpha = std::stof(Salpha);
    }
    if(argc>=3) {
        std::string Sbetha = std::string(argv[2]);
        betha = std::stof(Sbetha);
    }
    if(argc>=4) {
        std::string Sprone = std::string(argv[3]);
        proneTreshold=std::stof(Sprone);
    }
    if(argc>=5) {
        std::string Snumthreads = std::string(argv[4]);
        numthreads=std::stoi(Snumthreads);
    }

#pragma endregion

    Eigen::setNbThreads(numthreads);
    omp_set_num_threads(numthreads);

    typedef PNGraph PGraph;
    printf("Creating graph:\n");
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
    printf("Creating MxWcc graph:\n");
    F = TSnap::GetMxWcc(G);

    TIntV lis;

    printf("Creating Sym graph:\n");
    //degree discounted

    Eigen::SparseMatrix<double> g = SymSnap::DegreeDiscounted(F, alpha, betha, proneTreshold);

    printf("Saving Sym graph:\n");
    SymSnap::PrintSym(g,listi,SymGraphPath);

    return 0;
}
