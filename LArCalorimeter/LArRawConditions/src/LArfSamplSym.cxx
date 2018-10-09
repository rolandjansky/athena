/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "LArRawConditions/LArfSamplSym.h"
#include "LArRawConditions/LArfSamplComplete.h"
#include "LArRawConditions/LArfSamplMC.h"
#include "LArRawConditions/LArMCSym.h"


LArfSamplSym::LArfSamplSym(const LArMCSym* mcSym,  const LArfSamplMC* fSamplComplete) :
  m_mcSym(mcSym),
  m_fSamplComplete(static_cast<const LArfSamplComplete*>(fSamplComplete)) //Cast to base-class
{}

LArfSamplSym::~LArfSamplSym() {}

const float& LArfSamplSym::FSAMPL(const HWIdentifier&  hwid) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  return m_fSamplComplete->FSAMPL(symhwid);
}
  
const float& LArfSamplSym::FSAMPL(const Identifier&  id) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOfl(id);
  return m_fSamplComplete->FSAMPL(symhwid);
}
