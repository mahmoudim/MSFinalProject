#pragma once
#ifndef SymSnap_H
#define SymSnap_H

#include "stdafx.h"

namespace SymSnap {
	Eigen::SparseMatrix<double> DegreeDiscounted(PNGraph G,float alpha,float betha, float treshold);
    Eigen::SparseMatrix<double> DegreeDiscountedProposed(PNGraph G,float alpha,float betha, float treshold,csv::Parser,std::map<int, std::string>,std::map<std::string,int>);
    double getDirectedModularity(PNGraph G, int *Clusters,int count);
	void PrintSym(Eigen::SparseMatrix<double>,std::map<int, std::string>,const char *);
}
#endif 
