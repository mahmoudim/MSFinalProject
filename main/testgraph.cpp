#include "stdafx.h"

#define graphPath "../graph.g"
#define SnapMXWCCPath "../graph.snap"


int main(int argc, char* argv[]) {

  typedef PNGraph PGraph; 
  printf("Creating graph:\n");
  PGraph G = PGraph::TObj::New();
  PNGraph F;
  

  std::map<std::string, int> list;
  std::map<int, std::string> listi;

  TFIn fin(SnapMXWCCPath);
  try {
		F=TNGraph::Load(fin);
  }
	catch (PExcept e) {
		printf("%s", e->GetMsgStr().GetCStr());
	  FILE *file = fopen(graphPath, "r");

	  char source[10], dest[10];


	  int count = 0;
	  while (fscanf(file, "%s ==> %s\n", source, dest) != EOF)
	  {
		  std::string source(source), dest(dest);
		  if (list.find(source) == list.end())
		  {
			  list.insert(std::pair<std::string, int>(source, count));
			  listi.insert(std::pair<int, std::string>(count, source));
			  G->AddNode(count++);
		  }

		  if (list.find(dest) == list.end())
		  {
			  list.insert(std::pair<std::string, int>(dest, count));
			  listi.insert(std::pair<int, std::string>(count, dest));
			  G->AddNode(count++);
		  }
		  G->AddEdge(list[source], list[dest]);
	  }
	  F = TSnap::GetMxWcc(G);
	  
	  TFOut fout(SnapMXWCCPath);
	  F->Save(fout);
  }
  TIntV lis;

  float alpha = 0.5, betha = 0.5;

  //degree discounted 
  //Eigen::setNbThreads(6);
  omp_set_num_threads(4);
  Eigen::SparseMatrix<double> g = SymSnap::DegreeDiscounted(F, alpha, betha,0.25);
  //Eigen::MatrixXd m = Eigen::MatrixXd::Zero(100, 100);
  SpectralClustering* c = new SpectralClustering(g, 100);
  printf("ok\n");
  std::vector <std::vector<int>> a = c->clusterKmeans(10);//c->clusterRotate();
  printf("ok\n");
  printf("%lf",SymSnap::getDirectedModularity(F, a));
  
  TFltV v;

  

  F->GetNIdV(lis);

#pragma region saveDataset


  //FILE*f = fopen("white.list","w");

  FILE*f = fopen("dataset", "w");

  FILE*ids = fopen("ids", "w");

  for (int i = 0; i < lis.Len(); i++) {
	  int j = lis[i];
	  /*fprintf(f, "%s\n", listi[j].c_str());
	  char str[80];
	  sprintf(str, "copy papers_text\\%s\.txt white_papers_text\\%s\.txt", listi[j].c_str(), listi[j].c_str());
	  system(str);*/
	  char fname[80];
	  sprintf(fname, "newUrl\\%s\.txt", listi[j].c_str());
	  std::ifstream doc(fname);
	  std::string str((std::istreambuf_iterator<char>(doc)),
		  std::istreambuf_iterator<char>());
	  
	  std::replace(str.begin(), str.end(), '\n', ' ');
	  std::replace(str.begin(), str.end(), '\r', ' ');
	  int count = 0,spaces=0;

	  std::replace_if(str.begin(), str.end(), [&count,&spaces](char x) {
		  if (x < 0)
		  {
			  spaces++;
			  return true;
		  }
		  if (isalpha(x))
		  {
			  count++;
			  return false;
		  }
		  switch (x)
		  {
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
	  
	  if (count-spaces > 100) {
		  fprintf(f, "%s\n", str.c_str());
		  fprintf(ids, "%s\n", listi[j].c_str());
	  }
	  else {
		  char str[80];
		  sprintf(str, "copy papers_text\\%s\.txt black\\%s\.txt", listi[j].c_str(), listi[j].c_str());
		  system(str);
	  }
  }
  fclose(f);

#pragma endregion

  return 0;
}