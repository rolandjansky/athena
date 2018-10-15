/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArNoiseSym.h"
#include "LArRawConditions/LArNoiseMC.h"
#include "LArRawConditions/LArMCSym.h"


LArNoiseSym::LArNoiseSym(const LArMCSym* mcSym,  const LArNoiseMC* noiseComplete) :
  m_mcSym(mcSym),
  m_noiseComplete(noiseComplete) //Implicit cast to base-class
{}

LArNoiseSym::~LArNoiseSym() {}
 
const float&  LArNoiseSym::noise(const HWIdentifier&  hwid, int gain) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  return m_noiseComplete->get(symhwid,gain).m_Noise;
}
  
