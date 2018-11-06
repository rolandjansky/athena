/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "LArRawConditions/LArfSamplSym.h"
#include "LArRawConditions/LArMCSym.h"


LArfSamplSym::LArfSamplSym(const LArMCSym* mcSym,  const LArfSamplMC* fSamplComplete) :
  m_mcSym(mcSym),
  m_fSamplCont(fSamplComplete) //Implicit cast to base-class
{}

LArfSamplSym::~LArfSamplSym() {}

const float& LArfSamplSym::FSAMPL(const HWIdentifier&  hwid) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  return m_fSamplCont->get(symhwid).m_fSampl;
}
  
const float& LArfSamplSym::FSAMPL(const Identifier&  id) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOfl(id);
  return m_fSamplCont->get(symhwid).m_fSampl;
}
