#pragma once
#ifndef SymSnap_H
#define SymSnap_H

#include "stdafx.h"

namespace SymSnap {
	Eigen::SparseMatrix<double> DegreeDiscounted(PNGraph G,float alpha,float betha, float treshold);
	double getDirectedModularity(PNGraph G, std::vector<std::vector<int> > clusters);
}
#endif 
