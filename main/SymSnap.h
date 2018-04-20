#pragma once
#ifndef SymSnap_H
#define SymSnap_H

#include "stdafx.h"

namespace SymSnap {
	Eigen::SparseMatrix<double> DegreeDiscounted(PNGraph G,float alpha,float betha, float treshold);
	Eigen::Matrix2d DegreeDiscountedProPosed(PNGraph G,float alpha,float betha);
	double getDirectedModularity(PNGraph G, int *Clusters,int count);
    void PrintSym(Eigen::SparseMatrix<double>,std::map<int, std::string>,const char *);
    void PrintSym(Eigen::Matrix2d,std::map<int, std::string>,const char *,double);
}
#endif 
