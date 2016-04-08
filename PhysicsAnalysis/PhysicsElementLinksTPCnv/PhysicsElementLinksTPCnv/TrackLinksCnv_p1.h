/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// TrackLinksCnv_p1.h 
// Header file for class TrackLinksCnv_p1
// (example of a ParticleLinksCnv_p1 instantiation)
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef TrackLinksCnv_p1_H 
#define TrackLinksCnv_p1_H 

#include "ParticleEventTPCnv/ParticleLinksCnv_p1.h"
#include "Particle/TrackParticleContainer.h"

class TrackLinksCnv_p1:public ParticleLinksCnv_p1<Rec::TrackParticleContainer>{
    TrackLinksCnv_p1();
  ~TrackLinksCnv_p1(){;}
};

#endif
