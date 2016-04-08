/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// ElectronLinksCnv_p1.h 
// Header file for class ElectronLinksCnv_p1
//  a ParticleLinksCnv_p1 instantiation)
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef ElectronLinksCnv_p1_H 
#define ElectronLinksCnv_p1_H 

#include "ParticleEventTPCnv/ParticleLinksCnv_p1.h"
#include "egammaEvent/ElectronContainer.h"

class ElectronLinksCnv_p1:public ParticleLinksCnv_p1<ElectronContainer>{
    ElectronLinksCnv_p1();
    ~ElectronLinksCnv_p1(){;}
};

#endif
