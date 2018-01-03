#include "SymSnap.h"

PWgtNet SymSnap::DegreeDiscounted(PNGraph G,float alpha, float betha,float treshold) {
	TIntPrV in,out;TIntPr val;
	
	TSnap::GetNodeInDegV(G,in);
	TSnap::GetNodeOutDegV(G,out);
	int count = G->GetNodes();
	std::map<int,int> ids, idsrev;
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

	PWgtNet graph = TWgtNet::New();

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

	return graph;
};