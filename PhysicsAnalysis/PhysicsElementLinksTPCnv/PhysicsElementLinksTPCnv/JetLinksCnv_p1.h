/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////// /////////////////////////////
// JetLinksCnv_p1.h 
// Header file for class JetLinksCnv_p1
// (example of a ParticleLinksCnv_p1 instantiation)
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef JetLinksCnv_p1_H 
#define JetLinksCnv_p1_H 

#include "ParticleEventTPCnv/ParticleLinksCnv_p1.h"
#include "JetEvent/JetCollection.h"

class JetLinksCnv_p1:public ParticleLinksCnv_p1<JetCollection>{
    JetLinksCnv_p1();
  ~JetLinksCnv_p1(){;}
};

#endif
