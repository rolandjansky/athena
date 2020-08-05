/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_GENVERTEXFWD_H
#define ATLASHEPMC_GENVERTEXFWD_H
#ifdef HEPMC3
#include "HepMC3/GenVertex_fwd.h"
namespace HepMC {
typedef HepMC3::GenVertexPtr GenVertexPtr;
typedef HepMC3::ConstGenVertexPtr ConstGenVertexPtr;
}
#else
namespace HepMC {
class GenVertex;
typedef HepMC::GenVertex* GenVertexPtr;
typedef const HepMC::GenVertex* ConstGenVertexPtr;
}
#endif
#endif
