/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMinBiasAverageSym.h"
#include "LArRawConditions/LArMinBiasAverageMC.h"
#include "LArRawConditions/LArMCSym.h"


LArMinBiasAverageSym::LArMinBiasAverageSym(const LArMCSym* mcSym,  const LArMinBiasAverageMC* MinBiasAverageComplete) :
  m_mcSym(mcSym),
  m_MinBiasAverageComplete(MinBiasAverageComplete) //Implicit cast to base-class
{}

LArMinBiasAverageSym::~LArMinBiasAverageSym() {}

const float& LArMinBiasAverageSym::minBiasAverage(const HWIdentifier& hwid) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  return m_MinBiasAverageComplete->get(symhwid).m_MinBiasAverage;
}
