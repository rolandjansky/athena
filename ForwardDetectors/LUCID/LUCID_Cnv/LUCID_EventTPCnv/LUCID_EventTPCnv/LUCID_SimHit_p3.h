/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_SIMHIT_P3_H
#define LUCID_SIMHIT_P3_H

#include <vector>
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p3.h"

class LUCID_SimHit_p3 {

 public:
  
  LUCID_SimHit_p3() {

    m_tubeID        = 0;
    m_track         = 0;
    m_pdgCode       = 0;
    m_genVolume     = 0;
    m_stepStartPosX = 0;
    m_stepStartPosY = 0;
    m_stepStartPosZ = 0;
    m_stepEndPosX   = 0;
    m_stepEndPosY   = 0;
    m_stepEndPosZ   = 0;
    m_preStepTime   = 0;
    m_postStepTime  = 0;
    m_wavelength    = 0;
    m_energy        = 0;
  };
  
  friend class LUCID_SimHitCnv_p3;
  
 private:
  
  HepMcParticleLink_p3 m_partLink;   
  
  short m_tubeID;
  int   m_track;
  int   m_pdgCode;
  int   m_genVolume;
  float m_stepStartPosX;
  float m_stepStartPosY;
  float m_stepStartPosZ;
  float m_stepEndPosX;
  float m_stepEndPosY;
  float m_stepEndPosZ;
  float m_preStepTime;
  float m_postStepTime;
  float m_wavelength;
  float m_energy;
};

#endif
