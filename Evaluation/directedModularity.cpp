#include "../main/stdafx.h"

#define graphPath "../graph.g"
#define namesgraphPath "SymGraph.g.index"
#define ComgraphPath "preds.names"


int main(int argc, char *argv[]) {

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

    FILE *comfile = fopen(ComgraphPath, "r");
    FILE *namesgraphfile = fopen(namesgraphPath, "r");

    std::map<int,std::string> names;
    int * clusters= new int[G->GetNodes()+1];
    memset(clusters,0,sizeof(int)*(G->GetNodes()+1));
    int nodeId,commid,commax=0;
    char  nodename[10];

    while (fscanf(namesgraphfile, "%d,%s\n", &nodeId, nodename) != EOF) {
        names[nodeId]=std::string(nodename);
    }

    while (fscanf(comfile, "%d %d\n", &nodeId, &commid) != EOF) {
        clusters[list[names[nodeId]]]=commid;
        commax=std::max(commax,commid);
    }

    printf("%f",SymSnap::getDirectedModularity(F,clusters,commax+1));

    return 0;
}
