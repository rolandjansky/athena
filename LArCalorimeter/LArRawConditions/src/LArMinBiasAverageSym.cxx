/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMinBiasAverageSym.h"
#include "LArRawConditions/LArMinBiasAverageComplete.h"
#include "LArRawConditions/LArMinBiasAverageMC.h"
#include "LArRawConditions/LArMCSym.h"


LArMinBiasAverageSym::LArMinBiasAverageSym(const LArMCSym* mcSym,  const LArMinBiasAverageMC* MinBiasAverageComplete) :
  m_mcSym(mcSym),
  m_MinBiasAverageComplete(static_cast<const LArMinBiasAverageComplete*>(MinBiasAverageComplete)) //Cast to base-class
{}

LArMinBiasAverageSym::~LArMinBiasAverageSym() {}
 
const float&  LArMinBiasAverageSym::minBiasAverage(const HWIdentifier&  hwid) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  return m_MinBiasAverageComplete->minBiasAverage(symhwid);
}
  

