#include "stdafx.h"


SymSnap::DegreDiscountedRes * SymSnap::DegreeDiscounted(PNGraph G,float alpha, float betha) {
    std::map<int, int> ids, *idsrev1=new std::map<int, int> () ,&idsrev=*idsrev1;
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
        TInt a, b;
        int poss=0;
        for (int i = 0; i < count; val = in[i++]) {
            val.GetVal(a, b);
            if(ids.find(a.Val)==ids.end())
                ids[a.Val] = poss++;

            idsrev[ids[a.Val]] = a.Val;
            di.insert(ids[a.Val], ids[a.Val]) = pow((double)b.Val+ 1.0,-1*betha);
        }
        for (int i = 0; i < count; val = out[i++]) {
            val.GetVal(a, b);
            if (ids.find(a.Val) == ids.end())
                ids[a.Val] = poss++;
            idsrev[ids[a.Val]] = a.Val;
            doo.insert(ids[a.Val], ids[a.Val]) = pow((double)b.Val + 1.0, -1 * alpha);
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

    return new  DegreDiscountedRes( u,idsrev1);
};

SymSnap::DegreDiscountedRes * SymSnap::DegreeDiscountedProposed(PNGraph &G,float alpha, float betha,float treshold,std::map<int, std::string> listi,std::map<std::string,int> listIds,double * data,long xMax) {
    std::map<int, int> ids, *idsrev1= new std::map<int, int>(),& idsrev= *idsrev1;
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
        TInt a, b;
        int poss=0;
        for (int i = 0; i < count; val = in[i++]) {
            val.GetVal(a, b);
            if(ids.find(a.Val)==ids.end())
                ids[a.Val] = poss++;

            idsrev[ids[a.Val]] = a.Val;
            di.insert(ids[a.Val], ids[a.Val]) = pow((double)b.Val+ 1.0,-1*betha);
        }
        for (int i = 0; i < count; val = out[i++]) {
            val.GetVal(a, b);
            if (ids.find(a.Val) == ids.end())
                ids[a.Val] = poss++;
            idsrev[ids[a.Val]] = a.Val;
            doo.insert(ids[a.Val], ids[a.Val]) = pow((double)b.Val + 1.0, -1 * alpha);
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
            long double  dat;

            dat=SymSnap::reader(listIds[listi[idsrev[it.row()]]],listIds[listi[idsrev[it.col()]]],data,xMax);
            if(dat>=0)
                it.valueRef() =(it.value()+dat)/2;

        }
    DegreDiscountedRes *ddd=new  DegreDiscountedRes( new Eigen::SparseMatrix<double>(d->pruned(treshold,1)),idsrev1);
    delete (d);
    return ddd;

}

SymSnap::DegreDiscountedRes * SymSnap::ConbineProposedParalel(DegreDiscountedRes *res, float g,
                                                                      std::map<int, std::string> listi,std::map<std::string,int> listIds,double * data,long xMax) {
    Eigen::SparseMatrix<double> *u = new Eigen::SparseMatrix<double> (*res->res);
    for (int k=0; k<u->outerSize(); ++k)
        for (Eigen::SparseMatrix<double>::InnerIterator it(*u,k); it; ++it)
        {
            long double  dat;

            dat=SymSnap::reader(listIds[listi[(*res->idsrev)[it.row()]]],listIds[listi[(*res->idsrev)[it.col()]]],data,xMax);
            if(dat>0&&dat<it.value())
                it.valueRef() =dat;

        }
    DegreDiscountedRes *ddd=new  DegreDiscountedRes( new Eigen::SparseMatrix<double>(*u),res->idsrev);
    delete u;
    return ddd;
}

SymSnap::DegreDiscountedRes * SymSnap::DegreeDiscountedProposedParalel(PNGraph &G, float alpha, float betha) {
    std::map<int, int> ids, *idsrev1=new std::map<int, int> () ,&idsrev=*idsrev1;
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
        TInt a, b;
        int poss=0;
        for (int i = 0; i < count; val = in[i++]) {
            val.GetVal(a, b);
            if(ids.find(a.Val)==ids.end())
                ids[a.Val] = poss++;

            idsrev[ids[a.Val]] = a.Val;
            di.insert(ids[a.Val], ids[a.Val]) = pow((double)b.Val+ 1.0,-1*betha);
        }
        for (int i = 0; i < count; val = out[i++]) {
            val.GetVal(a, b);
            if (ids.find(a.Val) == ids.end())
                ids[a.Val] = poss++;
            idsrev[ids[a.Val]] = a.Val;
            doo.insert(ids[a.Val], ids[a.Val]) = pow((double)b.Val + 1.0, -1 * alpha);
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
    return new  DegreDiscountedRes( d,idsrev1);
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
    Eigen::SparseMatrix<double> &g(*res->res);
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
        fprintf(index,"%d,%s\n",i+1,listi[(*res->idsrev)[i]].c_str());
    }
    fclose(index);
}
;