///////////////////////// -*- c++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleBaseLinksCnv_p1.h 
// Header file for class ParticleBaseLinksCnv_p1
// (example of a ParticleLinksCnv_p1 instantiation)
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_PARTICLEBASELINKSCNV_P1_H
#define PARTICLEEVENTTPCNV_PARTICLEBASELINKSCNV_P1_H 1

#include "ParticleEventTPCnv/ParticleLinksCnv_p1.h"
#include "ParticleEvent/ParticleBaseLinks.h"

class ParticleBaseLinksCnv_p1: public ParticleLinksCnv_p1<ParticleBaseContainer>{
 public:
    ParticleBaseLinksCnv_p1();
    ~ParticleBaseLinksCnv_p1(){;}
	
};


#endif // !PARTICLEEVENTTPCNV_PARTICLEBASELINKSCNV_P1_H

