/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArRawConditions/LArRampSym.h"
#include "LArRawConditions/LArRampComplete.h"
#include "LArRawConditions/LArRampMC.h"
#include "LArRawConditions/LArMCSym.h"


LArRampSym::LArRampSym(const LArMCSym* mcSym,  const LArRampMC* rampComplete) :
  m_mcSym(mcSym),
  m_rampComplete(static_cast<const LArRampComplete*>(rampComplete)) //Cast to base-class
{}

LArRampSym::~LArRampSym() {}
 
ILArRamp::RampRef_t LArRampSym::ADC2DAC(const HWIdentifier&  hwid, int gain) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  return m_rampComplete->ADC2DAC(symhwid,gain);
}
  
ILArRamp::RampRef_t LArRampSym::ADC2DAC(const Identifier&  id, int gain) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOfl(id);
  return m_rampComplete->ADC2DAC(symhwid,gain);
}
