/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// PhotonLinksCnv_p1.h 
// Header file for class PhotonLinksCnv_p1
//  a ParticleLinksCnv_p1 instantiation)
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef PhotonLinksCnv_p1_H 
#define PhotonLinksCnv_p1_H 

#include "ParticleEventTPCnv/ParticleLinksCnv_p1.h"
#include "egammaEvent/PhotonContainer.h"

class PhotonLinksCnv_p1:public ParticleLinksCnv_p1<PhotonContainer>{
    PhotonLinksCnv_p1();
    ~PhotonLinksCnv_p1(){;}
};

#endif
