/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// MuonLinksCnv_p1.h 
// Header file for class MuonLinksCnv_p1
// (example of a ParticleLinksCnv_p1 instantiation)
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef MuonLinksCnv_p1_H 
#define MuonLinksCnv_p1_H 

#include "ParticleEventTPCnv/ParticleLinksCnv_p1.h"
#include "muonEvent/MuonContainer.h"

class MuonLinksCnv_p1:public ParticleLinksCnv_p1<Analysis::MuonContainer>{
    MuonLinksCnv_p1();
    ~MuonLinksCnv_p1(){;}
};

#endif
