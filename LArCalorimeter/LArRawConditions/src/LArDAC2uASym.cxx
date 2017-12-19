/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArDAC2uASym.h"
#include "LArRawConditions/LArDAC2uAComplete.h"
#include "LArRawConditions/LArDAC2uAMC.h"
#include "LArRawConditions/LArMCSym.h"


LArDAC2uASym::LArDAC2uASym(const LArMCSym* mcSym,  const LArDAC2uAMC* DAC2uAComplete) :
  m_mcSym(mcSym),
  m_DAC2uAComplete(static_cast<const LArDAC2uAComplete*>(DAC2uAComplete)) //Cast to base-class
{}

LArDAC2uASym::~LArDAC2uASym() {}
 
const float&  LArDAC2uASym::DAC2UA(const HWIdentifier&  hwid) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  return m_DAC2uAComplete->DAC2UA(symhwid);
}
  
const float& LArDAC2uASym::DAC2UA(const Identifier&  id) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOfl(id);
  return m_DAC2uAComplete->DAC2UA(symhwid);
}
