/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// TruthParticleLinksCnv.h 
// Header file for class TruthParticleLinksCnv
// a ParticleLinksCnv instantiation
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef TruthParticleLinksCnv_H 
#define TruthParticleLinksCnv_H 

#include "ParticleEventAthenaPool/ParticleLinksCnv.h"
#include "PhysicsElementLinks/TruthParticleLinks.h"

class TruthParticleLinksCnv:public ParticleLinksCnv<TruthParticleContainer>{
 public:
    TruthParticleLinksCnv(ISvcLocator* svcloc);
    ~TruthParticleLinksCnv(){;}
};

#endif
