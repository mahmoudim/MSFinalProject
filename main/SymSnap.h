#pragma once
#ifndef SymSnap_H
#define SymSnap_H

#include "stdafx.h"

namespace SymSnap {
	struct DegreDiscountedRes{
		Eigen::SparseMatrix<double> &res;
		std::map<int, int> &idsrev;
		DegreDiscountedRes(Eigen::SparseMatrix<double> &res,std::map<int, int> &idsrev):res(res),idsrev(idsrev){}
	};
	DegreDiscountedRes * DegreeDiscounted(PNGraph G,float alpha,float betha, float treshold);
	DegreDiscountedRes * DegreeDiscountedProposed(PNGraph &G,float alpha,float betha, float treshold,csv::Parser&,std::map<int, std::string>,std::map<std::string,int>);
	DegreDiscountedRes * DegreeDiscountedProposedParalel(PNGraph &G,float alpha,float betha);
	DegreDiscountedRes * ConbineAndPruneProposedParalel(DegreDiscountedRes * , float treshold,float g,csv::Parser&,std::map<int, std::string>,std::map<std::string,int>);
    double getDirectedModularity(PNGraph G, int *Clusters,int count);
	void PrintSym(DegreDiscountedRes *,std::map<int, std::string>,const char *);
}
#endif 
