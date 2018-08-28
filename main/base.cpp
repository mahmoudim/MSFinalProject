#include "stdafx.h"

#define graphPath "graph.g"
#define confFile "conf.csv"
#define SymGraphPath "/SymGraph.g"
struct operation{
    double alpha;
    double betha;
    PNGraph &G;
    std::map<int, std::string> &listi;
    operation(double alpha, double betha, PNGraph &G,
              std::map<int, std::string> &listi)
            : G(G), listi(listi){
        this->alpha=alpha;
        this->betha=betha;
    }
};
std::vector<double> prunes;

void executeTask(int id,operation *op)
{
    printf("%.4f-%.4f\n",op->alpha,op->betha);
    SymSnap::DegreDiscountedRes * g = SymSnap::DegreeDiscountedProposedParalel(op->G, op->alpha,op->betha);
    for (int j = 0; j < prunes.size(); ++j) {
            char dir[40];
            sprintf(dir, "%.4f-%.4f-%.4f", op->alpha, op->betha, prunes[j]);
            FILE *tst = fopen((std::string(dir) + SymGraphPath+".gz").c_str(), "r");
            if (tst == NULL) {
                printf("%.4f-%.4f-%.4f-%.4f\n", op->alpha, op->betha, prunes[j]);
                system((std::string("mkdir ") + dir).c_str());
                SymSnap::DegreDiscountedRes *g2 = new SymSnap::DegreDiscountedRes(
                        new Eigen::SparseMatrix<double>(g->res->pruned(prunes[j], 1)), g->idsrev);
                SymSnap::PrintSym(g2, op->listi, (std::string(dir) + SymGraphPath).c_str());
                system((std::string("gzip -f ") + (std::string(dir) + SymGraphPath)).c_str());
                delete (g2);
            }
            else
                fclose(tst);
    }

    //delete(op);
    delete (g->idsrev);
    delete(g);
}

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


    csv::Parser confreader = csv::Parser(confFile);

    ctpl::thread_pool p(numthreads);

    printf("loading finished!\n");

    double g,proneTreshold;
    try {
        for (int i = 0;; ) {
            proneTreshold = confreader[2][i];
            i++;
            prunes.push_back(proneTreshold);
        }
    }  catch (csv::Error &e) {

    }
    //degree discounted
    double alpha,betha;
    try {
        for (int i = 0;; ) {
            alpha = confreader[0][i];
            i++;
            try {
                for (int j = 0;; ) {
                    betha = confreader[1][j];
                    j++;
                    p.push(executeTask, new operation(alpha, betha, F, listi));
                }
            }  catch (csv::Error &e) {

            }
        }
    }  catch (csv::Error &e) {

    }
    while (p.n_idle()!=p.size())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }
    return 0;
}
