#pragma once
#ifndef SymSnap_H
#define SymSnap_H

#include "stdafx.h"
#ifndef SymSnap
namespace SymSnap {
    static double ERROR=-1;

	inline double & reader(int x,int y,double * data,long xMax)
	{
        if(x>=xMax||y>=xMax)
            return  ERROR;
		return data[x*xMax + y];
	}

	struct DegreDiscountedRes{
		Eigen::SparseMatrix<double> *res;
		std::map<int, int> *idsrev;
		DegreDiscountedRes(Eigen::SparseMatrix<double> *res,std::map<int, int> *idsrev):res(res),idsrev(idsrev){}
        ~DegreDiscountedRes(){
            delete (res);
        }

	};
	DegreDiscountedRes * DegreeDiscounted(PNGraph G,float alpha,float betha, float treshold);
	DegreDiscountedRes * DegreeDiscountedProposed(PNGraph &G,float alpha,float betha, float treshold,std::map<int, std::string>,std::map<std::string,int>,double * data,long xMax);
	DegreDiscountedRes * DegreeDiscountedProposedParalel(PNGraph &G,float alpha,float betha);
	DegreDiscountedRes * ConbineProposedParalel(DegreDiscountedRes * ,float g,std::map<int, std::string>,std::map<std::string,int>,double *,long);
    double getDirectedModularity(PNGraph G, int *Clusters,int count);
	void PrintSym(DegreDiscountedRes *,std::map<int, std::string>,const char *);
}
#endif 
#endif
