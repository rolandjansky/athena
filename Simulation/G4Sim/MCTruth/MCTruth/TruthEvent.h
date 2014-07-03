/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthEvent_H
#define TruthEvent_H

#include "HepMC/GenEvent.h"

#ifndef __CINT__
#include "CLIDSvc/CLASS_DEF.h"
#endif

typedef HepMC::GenEvent TruthEvent;
typedef HepMC::GenVertex TruthVertex;
typedef HepMC::GenParticle TruthParticle;

#ifndef __CINT__
CLASS_DEF( TruthEvent , 97924139 , 1 )
#endif

#endif
