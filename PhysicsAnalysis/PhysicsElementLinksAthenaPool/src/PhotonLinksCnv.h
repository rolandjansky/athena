/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// PhotonLinksCnv.h 
// Header file for class PhotonLinksCnv
// a ParticleLinksCnv instantiation
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef PhotonLinksCnv_H 
#define PhotonLinksCnv_H 

#include "ParticleEventAthenaPool/ParticleLinksCnv.h"
#include "PhysicsElementLinks/PhotonLinks.h"

class PhotonLinksCnv:public ParticleLinksCnv<PhotonContainer>{
 public:
    PhotonLinksCnv(ISvcLocator* svcloc);
    ~PhotonLinksCnv(){;}
};

#endif
