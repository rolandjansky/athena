/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMinBiasSym.h"
#include "LArRawConditions/LArMinBiasComplete.h"
#include "LArRawConditions/LArMinBiasMC.h"
#include "LArRawConditions/LArMCSym.h"


LArMinBiasSym::LArMinBiasSym(const LArMCSym* mcSym,  const LArMinBiasMC* MinBiasComplete) :
  m_mcSym(mcSym),
  m_MinBiasComplete(static_cast<const LArMinBiasComplete*>(MinBiasComplete)) //Cast to base-class
{}

LArMinBiasSym::~LArMinBiasSym() {}
 
const float&  LArMinBiasSym::minBiasRMS(const HWIdentifier&  hwid) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  return m_MinBiasComplete->minBiasRMS(symhwid);
}
  
const float& LArMinBiasSym::minBiasRMS(const Identifier&  id) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOfl(id);
  return m_MinBiasComplete->minBiasRMS(symhwid);
}
