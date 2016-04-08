////////////////////////////////////// -*- C++ -*- ////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleBaseLinksCnv.h 
// Header file for class ParticleBaseLinksCnv
// (example of a ParticleLinksCnv instantiation)
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_PARTICLEBASELINKSCNV_H
#define PARTICLEEVENTATHENAPOOL_PARTICLEBASELINKSCNV_H 1
#include "ParticleEventAthenaPool/ParticleLinksCnv.h"
#include "ParticleEvent/ParticleBaseLinks.h"

class ParticleBaseLinksCnv:public ParticleLinksCnv<ParticleBaseContainer>{
    ParticleBaseLinksCnv(ISvcLocator* svcloc);
  ~ParticleBaseLinksCnv(){;}
};

#endif // !PARTICLEEVENTATHENAPOOL_PARTICLEBASELINKSCNV_H

