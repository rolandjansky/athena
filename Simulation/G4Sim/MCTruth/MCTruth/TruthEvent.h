/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthEvent_H
#define TruthEvent_H

#include "AtlasHepMC/GenEvent.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef HepMC::GenEvent TruthEvent;
typedef HepMC::GenVertex TruthVertex;
typedef HepMC::GenParticle TruthParticle;

CLASS_DEF( TruthEvent , 97924139 , 1 )

#endif
