/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// TruthParticleLinksCnv_p1.h 
// Header file for class TruthParticleLinksCnv_p1
//  a ParticleLinksCnv_p1 instantiation)
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef TruthParticleLinksCnv_p1_H 
#define TruthParticleLinksCnv_p1_H 

#include "ParticleEventTPCnv/ParticleLinksCnv_p1.h"
#include "McParticleEvent/TruthParticleContainer.h"

class TruthParticleLinksCnv_p1:public ParticleLinksCnv_p1<TruthParticleContainer>{
    TruthParticleLinksCnv_p1();
    ~TruthParticleLinksCnv_p1(){;}
};

#endif
