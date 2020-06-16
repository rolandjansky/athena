/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de
*/
#ifndef ATLASHEPMC_GENVERTEX_H
#define ATLASHEPMC_GENVERTEX_H
#include "HepMC/GenVertex.h"
namespace HepMC {
typedef HepMC::GenVertex* GenVertexPtr;
typedef HepMC::GenVertex* const  ConstGenVertexPtr;
inline GenVertexPtr newGenVertexPtr(const HepMC::FourVector &pos = HepMC::FourVector(0.0,0.0,0.0,0.0), const int i=0) {
    return new HepMC::GenVertex(pos,i);
}
inline int barcode(ConstGenVertexPtr p){ return p->barcode();}
}
#endif
