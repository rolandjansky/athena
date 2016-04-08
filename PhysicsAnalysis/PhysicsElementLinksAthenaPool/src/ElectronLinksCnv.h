/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// ElectronLinksCnv.h 
// Header file for class ElectronLinksCnv
// a ParticleLinksCnv instantiation
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef ElectronLinksCnv_H 
#define ElectronLinksCnv_H 

#include "ParticleEventAthenaPool/ParticleLinksCnv.h"
#include "PhysicsElementLinks/ElectronLinks.h"

class ElectronLinksCnv:public ParticleLinksCnv<ElectronContainer>{
 public:
    ElectronLinksCnv(ISvcLocator* svcloc);
    ~ElectronLinksCnv(){;}
};

#endif
