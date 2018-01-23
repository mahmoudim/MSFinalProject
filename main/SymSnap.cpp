#include "SymSnap.h"

std::map<int, int> ids, idsrev;

Eigen::SparseMatrix<double> SymSnap::DegreeDiscounted(PNGraph G,float alpha, float betha,float treshold) {
	TIntPrV in,out;TIntPr val;
	
	TSnap::GetNodeInDegV(G,in);
	TSnap::GetNodeOutDegV(G,out);
	int count = G->GetNodes();
	
	Eigen::SparseMatrix<double> adj(count, count);
	for (int i = 0; i < count; i++) {
		adj.insert(i, i) = 1;
	}
	Eigen::SparseMatrix<double> di(count, count), doo(count, count);
	for (int i = 0; i < count; val = in[i++]) {
		TInt a, b;
		val.GetVal(a, b);
		if(ids.find(a)==ids.end())
			ids[a] = ids.size();
		idsrev[ids[a]] = a;
		di.insert(ids[a], ids[a]) = pow((double)b.Val+ 1.0,-1*betha);
	}
	for (int i = 0; i < count; val = out[i++]) {
		TInt a, b;
		val.GetVal(a, b);
		if (ids.find(a) == ids.end())
			ids[a] = ids.size();
		idsrev[ids[a]] = a;
		doo.insert(ids[a], ids[a]) = pow((double)b.Val + 1.0, -1 * alpha);
	}

	
	for (TNGraph::TEdgeI  s = G->BegEI(); s!=G->EndEI(); s++) {
		if(s.GetSrcNId()!= s.GetDstNId())
			adj.insert(ids[s.GetSrcNId()], ids[s.GetDstNId()]) = 1;
	}
	
	Eigen::Transpose<Eigen::SparseMatrix<double>> adj_trans = adj.transpose();

	Eigen::SparseMatrix<double> bd =(doo)*(adj)*(di)*(adj_trans)*(doo);

	Eigen::SparseMatrix<double> cd = (di)*(adj)*(doo)*(adj_trans)*(di);

	Eigen::SparseMatrix<double> u = (bd + cd).pruned(treshold);

	/*PWgtNet graph = TWgtNet::New();

	for (int k = 0; k < u.outerSize(); ++k) {
		for (Eigen::SparseMatrix<double>::InnerIterator it(u, k); it; ++it)
		{
			if (!graph->IsNode(idsrev[it.row()])) {
				graph->AddNode(idsrev[it.row()]);
			}
			if (!graph->IsNode(idsrev[it.col()])) {
				graph->AddNode(idsrev[it.col()]);
			}
			graph->AddEdge(idsrev[it.row()], idsrev[it.col()], it.value());
		}
	}
	graph->AddBiDirEdges(1);
	*/
	return u;
}
double SymSnap::getDirectedModularity(PNGraph G, std::vector<std::vector<int>> clusters)
{
	int *Clusters = new int[G->GetNodes()];
	for (int i = 0; i < clusters.size(); i++)
	{
		for (int j = 0; j < clusters[i].size(); j++)
		{
			//printf("%d ", clusters[i][j]);
			Clusters[clusters[i][j]] = i;
		}
		//printf("\n");
	}
	printf(" ok \n");
	int *IN_d = new int[clusters.size()];
	int *OUT_d = new int[clusters.size()];
	int *NUM_M = new int[clusters.size()];
	printf(" ok \n");
	for (TNGraph::TEdgeI s = G->BegEI(); s != G->EndEI(); s++) {
		if (Clusters[s.GetSrcNId()] == Clusters[s.GetDstNId()])
			NUM_M[Clusters[ids[s.GetSrcNId()]]] += 1;
		OUT_d[Clusters[ids[s.GetSrcNId()]]] += 1;
		IN_d[Clusters[ids[s.GetDstNId()]]] += 1;
	}
	double res=0,num_edges=G->GetEdges();
	for (int i = 0; i < clusters.size(); i++)
		res += (((double)NUM_M[i]) / (num_edges))-(((double)OUT_d[i] * IN_d[i]) / (num_edges*num_edges));
	return res;
}
;