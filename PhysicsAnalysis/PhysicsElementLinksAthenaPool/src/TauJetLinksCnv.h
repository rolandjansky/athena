/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// TauJetLinksCnv.h 
// Header file for class TauJetLinksCnv
// a ParticleLinksCnv instantiation
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef TauJetLinksCnv_H 
#define TauJetLinksCnv_H 

#include "ParticleEventAthenaPool/ParticleLinksCnv.h"
#include "PhysicsElementLinks/TauJetLinks.h"

class TauJetLinksCnv:public ParticleLinksCnv<Analysis::TauJetContainer>{
 public:
    TauJetLinksCnv(ISvcLocator* svcloc);
    ~TauJetLinksCnv(){;}
};

#endif
