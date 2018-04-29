#include "stdafx.h"

#define graphPath "graph.g"
#define docSim "simHelinger.csv"
#define confFile "conf.csv"
#define docSimiDS "new_ids.txt"
#define SymGraphPath "/SymGraph.g"


int main(int argc, char *argv[]) {
    int numthreads=4;
#pragma region getargs

    if(argc>=2) {
        std::string Snumthreads = std::string(argv[1]);
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
    std::map<std::string,int> listIds;

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

    //read Similarity

    FILE *ids=fopen(docSimiDS,"r");
    char name[10];
    while(fscanf(ids,"%s\n",name)!=EOF)
    {
        listIds[std::string(name)]=listIds.size();
    }


    csv::Parser reader = csv::Parser(docSim);
    csv::Parser confreader = csv::Parser(confFile);

    printf("loading finished!\n");

    //degree discounted
    double alpha,betha,proneTreshold;
    try {
        for (int i = 0;; ) {
            alpha = confreader[0][i];
            i++;
            try {
                for (int j = 0;; ) {
                    betha = confreader[1][j];
                    j++;
                    try {
                        for (int k = 0;; ) {
                            proneTreshold = confreader[2][k];
                            k++;
                            printf("%.4f-%.4f-%.4f",alpha,betha,proneTreshold);
                            Eigen::SparseMatrix<double> g = SymSnap::DegreeDiscountedProposed(F, alpha, betha,proneTreshold,reader, listi,listIds);
                            //Eigen::SparseMatrix<double> g = SymSnap::DegreeDiscounted(F, alpha, betha, proneTreshold);
                            char dir[20];
                            sprintf(dir,"%.4f-%.4f-%.4f",alpha,betha,proneTreshold);
                            system((std::string("mkdir ")+dir).c_str());
                            SymSnap::PrintSym(g, listi, (std::string(dir)+SymGraphPath).c_str());
                        }
                    } catch (csv::Error &e) {

                    }
                }
            }  catch (csv::Error &e) {

            }
        }
    }  catch (csv::Error &e) {

    }
    return 0;
}
