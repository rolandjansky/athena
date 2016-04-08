/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// TrackLinksCnv.h 
// Header file for class TrackLinksCnv
// a ParticleLinksCnv instantiation
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef TrackLinksCnv_H 
#define TrackLinksCnv_H 

#include "ParticleEventAthenaPool/ParticleLinksCnv.h"
#include "PhysicsElementLinks/TrackLinks.h"

class TrackLinksCnv:public ParticleLinksCnv<Rec::TrackParticleContainer>{
 public:
    TrackLinksCnv(ISvcLocator* svcloc);
    ~TrackLinksCnv(){;}
};

#endif
