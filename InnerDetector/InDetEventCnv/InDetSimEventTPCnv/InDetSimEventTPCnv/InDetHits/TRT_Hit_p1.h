/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_HIT_P1_H
#define TRT_HIT_P1_H

#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"

class TRT_Hit_p1 {
 public:
  int hitID; // To identify the hit
  HepMcParticleLink_p1 m_partLink; // link to the particle generating the hit
  int particleEncoding;         // PDG id 
  float kineticEnergy;          // kin energy of the particle
  float energyDeposit;          // energy deposit by the hit
  float preStepX;
  float preStepY;
  float preStepZ;
  float postStepX;
  float postStepY;
  float postStepZ;
  float globalTime;
};
#endif
