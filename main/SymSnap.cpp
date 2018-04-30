#include "stdafx.h"


SymSnap::DegreDiscountedRes * SymSnap::DegreeDiscounted(PNGraph G,float alpha, float betha,float treshold) {
    std::map<int, int> ids, idsrev;
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

    Eigen::SparseMatrix<double> u = (bd + cd).pruned(treshold,1);

    return new  DegreDiscountedRes(u,idsrev);
};

SymSnap::DegreDiscountedRes * SymSnap::DegreeDiscountedProposed(PNGraph &G,float alpha, float betha,float treshold,csv::Parser &reader,std::map<int, std::string> listi,std::map<std::string,int> listIds) {
    std::map<int, int> ids, idsrev;
    TIntPrV in,out;TIntPr val;
    TSnap::GetNodeInDegV(G,in);
    TSnap::GetNodeOutDegV(G,out);
    int count = G->GetNodes();


    Eigen::SparseMatrix<double> *bd;
    Eigen::SparseMatrix<double> *cd;

    {

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

        bd = new Eigen::SparseMatrix<double> ((doo) * (adj) * (di) * (adj_trans) * (doo));

        cd = new Eigen::SparseMatrix<double> ((di) * (adj) * (doo) * (adj_trans) * (di));
    }

    Eigen::SparseMatrix<double> *u = new Eigen::SparseMatrix<double> (*bd + *cd);
    delete(bd);
    delete(cd);

    double max=0;
    for (int k=0; k<u->outerSize(); ++k)
        for (Eigen::SparseMatrix<double>::InnerIterator it(*u,k); it; ++it)
        {
            if(it.value()>max)
                max=it.value();

        }
    Eigen::SparseMatrix<double> *d=new Eigen::SparseMatrix<double> ((*u/(max)));
    delete(u);

    for (int k=0; k<d->outerSize(); ++k)
        for (Eigen::SparseMatrix<double>::InnerIterator it(*d,k); it; ++it)
        {
            long double  data;
            try {
                data=reader[listIds[listi[idsrev[it.row()]]]][listIds[listi[idsrev[it.col()]]]];
                it.valueRef() =(it.value()+data)/2;
            } catch (csv::Error &e) {

            }
        }
    return new  DegreDiscountedRes( d->pruned(treshold,1),idsrev);

}

double SymSnap::getDirectedModularity(PNGraph G, int *Clusters,int count)
{
	double *IN_d = new double[count];
	double  *OUT_d = new double [count];
	double   *NUM_M = new double [count];
	int i=0;
	for (TNGraph::TEdgeI s = G->BegEI(); s != G->EndEI(); s++) {
		if (Clusters[s.GetSrcNId()] == Clusters[s.GetDstNId()])
			NUM_M[Clusters[s.GetSrcNId()]] += 1;
		OUT_d[Clusters[s.GetSrcNId()]] += 1;
		IN_d[Clusters[s.GetDstNId()]] += 1;
	}
	double res=0,num_edges=G->GetEdges();
	for (int i = 0; i < count; i++) {
			res += ((NUM_M[i]) / (num_edges)) - ((OUT_d[i] * IN_d[i]) / (num_edges * num_edges));

	}
	return res;
}
void SymSnap::PrintSym(DegreDiscountedRes * res, std::map<int, std::string> listi, const char *path) {
	FILE *symfile = fopen(path, "w");
    Eigen::SparseMatrix<double> &g(res->res);
	for (int k = 0; k < g.outerSize(); ++k) {
		for (Eigen::SparseMatrix<double>::InnerIterator it(g, k); it; ++it)
		{
            fprintf(symfile,"%ld,%ld,%f\n",it.row()+1,it.col()+1,it.value());
		}
	}
	fclose(symfile);
    FILE *index = fopen((std::string(path)+".index").c_str(), "w");
    fprintf(index,"%ld\n",g.outerSize());
    for(int i =0 ; i<g.outerSize(); i++)
    {
        fprintf(index,"%d,%s\n",i+1,listi[res->idsrev[i]].c_str());
    }
    fclose(index);
}
;