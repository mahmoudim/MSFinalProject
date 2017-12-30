#pragma once
#ifndef SymSnap_H
#define SymSnap_H

#include "Snap.h"

namespace SymSnap {
	TNEANet *DegreeDiscounted(PNGraph G,float alpha,float betha, float treshold);
}
#endif 
