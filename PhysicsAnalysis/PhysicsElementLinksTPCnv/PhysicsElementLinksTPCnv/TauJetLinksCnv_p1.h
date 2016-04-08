/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// TauJetLinksCnv_p1.h 
// Header file for class TauJetLinksCnv_p1
// (example of a ParticleLinksCnv_p1 instantiation)
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef TauJetLinksCnv_p1_H 
#define TauJetLinksCnv_p1_H 

#include "ParticleEventTPCnv/ParticleLinksCnv_p1.h"
#include "tauEvent/TauJetContainer.h"

class TauJetLinksCnv_p1:public ParticleLinksCnv_p1<Analysis::TauJetContainer>{
    TauJetLinksCnv_p1();
  ~TauJetLinksCnv_p1(){;}
};

#endif
