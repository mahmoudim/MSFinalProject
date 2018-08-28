//
// Created by mehran on 2/27/18.
//
#include "../main/stdafx.h"

#define graphPath "../graph.g"
#define datasetPath "../dataset"
#define idsPath "../ids"
#define rawdataPath "/run/media/mehran/New Volume/Users/Mehran/Documents/MainProject/new start/papers_text/%s\.txt"


int main(int argc, char *argv[]) {


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

    F->GetNIdV(lis);

    FILE *f = fopen(datasetPath, "w");

    FILE *ids = fopen(idsPath, "w");


    for (int i = 0; i < lis.Len(); i++) {
        int j = lis[i];

        char fname[100];
        sprintf(fname,rawdataPath , listi[j].c_str());
        std::ifstream doc(fname);
        std::string str((std::istreambuf_iterator<char>(doc)),
                        std::istreambuf_iterator<char>());

        std::replace(str.begin(), str.end(), '\n', ' ');
        std::replace(str.begin(), str.end(), '\r', ' ');
        int count = 0, spaces = 0;

        std::replace_if(str.begin(), str.end(), [&count, &spaces](char x) {
            if (x < 0) {
                spaces++;
                return true;
            }
            if (isalpha(x)) {
                count++;
                return false;
            }
            switch (x) {
                case '(':
                    count++;
                    return false;
                case ')':
                    count++;
                    return false;
                case ';':
                    count++;
                    return false;
                case ',':
                    count++;
                    return false;
                case '\'':
                    count++;
                    return false;
                case '"':
                    count++;
                    return false;
                case '.':
                    count++;
                    return false;
                case '\\':
                    count++;
                    return false;
                case '/':
                    count++;
                    return false;
                case '[':
                    count++;
                    return false;
                case ']':
                    count++;
                    return false;
                case '?':
                    count++;
                    return false;
                case '!':
                    count++;
                    return false;
                case ':':
                    count++;
                    return false;
                case ' ':
                    spaces++;
                    count++;
                    return false;
                default:
                    spaces++;
                    return true;
            }
        }, ' ');

        if (count - spaces > 100) {
            fprintf(f, "%s\n", str.c_str());
            fprintf(ids, "%s\n", listi[j].c_str());
        }
    }
    fclose(f);

}
