#include "stdafx.h"

#define graphPath "graph.g"
#define docSim "simHelinger.csv"
#define confFile "conf.csv"
#define docSimiDS "new_ids.txt"
#define SymGraphPath "/SymGraph.g"
struct operation{
    double alpha;
    double betha;
    double prune;
    std::vector<std::pair<double,double>> &tasks;
    PNGraph &G;
    csv::Parser &reader;
    std::map<int, std::string> &listi;
    std::map<std::string,int> &listIds;
    operation(double alpha, double betha, double prune, PNGraph &G, csv::Parser &reader,
                  std::map<int, std::string> &listi, std::map<std::string, int> &listIds)
            : G(G), listIds(listIds), listi(listi), reader(reader) ,tasks(* new std::vector<std::pair<double,double>>()){
        this->alpha=alpha;
        this->betha=betha;
        this->prune=prune;
    }
    operation(double alpha, double betha, std::vector<std::pair<double,double>> tasks, PNGraph &G, csv::Parser &reader,
              std::map<int, std::string> &listi, std::map<std::string, int> &listIds)
            : G(G), listIds(listIds), listi(listi), reader(reader),tasks(tasks) {
        this->alpha=alpha;
        this->betha=betha;
        this->prune=prune;
    }
};

void executeTask(int id,operation *op)
{
    printf("%.4f-%.4f-%.4f\n",op->alpha,op->betha,op->prune);
    SymSnap::DegreDiscountedRes * g = SymSnap::DegreeDiscountedProposedParalel(op->G, op->alpha,op->betha);

    for (int i = 0; i < op->tasks.size(); ++i) {
        std::pair<double ,double > t=op->tasks[i];
        SymSnap::DegreDiscountedRes * g1 = SymSnap::ConbineAndPruneProposedParalel(g,t.first,t.second, op->reader, op->listi, op->listIds);
        //Eigen::SparseMatrix<double> g = SymSnap::DegreeDiscounted(F, alpha, betha, proneTreshold);
        char dir[20];
        sprintf(dir, "%.4f-%.4f-%.4f-%.4f", op->alpha, op->betha, t.first,t.second);
        system((std::string("mkdir ") + dir).c_str());
        SymSnap::PrintSym(g1, op->listi, (std::string(dir) + SymGraphPath).c_str());
        system((std::string("gzip -f ") + (std::string(dir) + SymGraphPath)).c_str());
        delete(g1);
    }
    delete(op);
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

    ctpl::thread_pool p(numthreads);

    printf("loading finished!\n");
    std::vector<std::pair<double,double>> tasks;
    double g,proneTreshold;
    try {
        for (int i = 0;; ) {
            proneTreshold = confreader[2][i];
            i++;
            try {
                for (int j = 0;; ) {
                    g = confreader[3][j];
                    j++;
                    tasks.push_back(std::pair<double,double>(proneTreshold,g));
                }
            }  catch (csv::Error &e) {

            }
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
                    p.push(executeTask, new operation(alpha, betha, tasks, F, reader, listi,listIds));
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
