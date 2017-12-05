/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_HIT_P2_H
#define TRT_HIT_P2_H

#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"

class TRT_Hit_p2 {
 public:
  TRT_Hit_p2()  {};
  int hitID; // To identify the hit
  HepMcParticleLink_p2 m_partLink; // link to the particle generating the hit
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
