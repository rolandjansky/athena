/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMinBiasSym.h"
#include "LArRawConditions/LArMinBiasMC.h"
#include "LArRawConditions/LArMCSym.h"


LArMinBiasSym::LArMinBiasSym(const LArMCSym* mcSym,  const LArMinBiasMC* MinBiasComplete) :
  m_mcSym(mcSym),
  m_MinBiasComplete(MinBiasComplete) //Implicit cast to base-class
{}

LArMinBiasSym::~LArMinBiasSym() {}
 
const float& LArMinBiasSym::minBiasRMS(const HWIdentifier&  hwid) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  return m_MinBiasComplete->get(symhwid).m_MinBiasRMS;
}
  

