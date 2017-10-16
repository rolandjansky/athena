/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIHIT_P2_H
#define SIHIT_P2_H

#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"

class SiHit_p2 {
 public:
  SiHit_p2()  {};
  float m_stX, m_stY, m_stZ;
  float m_enX, m_enY, m_enZ;
  float m_energyLoss; // deposited energy
  float m_meanTime; // time of energy deposition
  HepMcParticleLink_p2 m_partLink;
  unsigned int m_ID;
};
#endif
