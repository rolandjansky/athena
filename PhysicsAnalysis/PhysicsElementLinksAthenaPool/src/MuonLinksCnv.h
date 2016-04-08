/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// MuonLinksCnv.h 
// Header file for class MuonLinksCnv
//  a ParticleLinksCnv instantiation
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef MuonLinksCnv_H 
#define MuonLinksCnv_H 

#include "ParticleEventAthenaPool/ParticleLinksCnv.h"
#include "PhysicsElementLinks/MuonLinks.h"

class MuonLinksCnv:public ParticleLinksCnv<Analysis::MuonContainer>{
 public:
    MuonLinksCnv(ISvcLocator* svcloc);
    ~MuonLinksCnv(){;}
};

#endif
