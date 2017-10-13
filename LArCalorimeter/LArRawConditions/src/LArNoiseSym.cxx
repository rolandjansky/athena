/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArNoiseSym.h"
#include "LArRawConditions/LArNoiseComplete.h"
#include "LArRawConditions/LArNoiseMC.h"
#include "LArRawConditions/LArMCSym.h"


LArNoiseSym::LArNoiseSym(const LArMCSym* mcSym,  const LArNoiseMC* noiseComplete) :
  m_mcSym(mcSym),
  m_noiseComplete(static_cast<const LArNoiseComplete*>(noiseComplete)) //Cast to base-class
{}

LArNoiseSym::~LArNoiseSym() {}
 
const float&  LArNoiseSym::noise(const HWIdentifier&  hwid, int gain) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  return m_noiseComplete->noise(symhwid,gain);
}
  
const float& LArNoiseSym::noise(const Identifier&  id, int gain) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOfl(id);
  return m_noiseComplete->noise(symhwid,gain);
}
