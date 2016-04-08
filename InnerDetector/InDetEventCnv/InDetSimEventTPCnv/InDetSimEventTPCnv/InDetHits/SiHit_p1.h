/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIHIT_P1_H
#define SIHIT_P1_H

#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"

class SiHit_p1 {
 public:
  float m_stX, m_stY, m_stZ;
  float m_enX, m_enY, m_enZ;
  float m_energyLoss; // deposited energy
  float m_meanTime; // time of energy deposition
  HepMcParticleLink_p1 m_partLink;
  unsigned int m_ID;
};
#endif
