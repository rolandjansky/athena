/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// JetLinksCnv.h 
// Header file for class JetLinksCnv
// a ParticleLinksCnv instantiation)
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef JetLinksCnv_H 
#define JetLinksCnv_H 

#include "ParticleEventAthenaPool/ParticleLinksCnv.h"
#include "PhysicsElementLinks/JetLinks.h"

class JetLinksCnv:public ParticleLinksCnv<JetCollection>{
 public:
    JetLinksCnv(ISvcLocator* svcloc);
    ~JetLinksCnv(){;}
};

#endif
