/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMphysOverMcalSym.h"
#include "LArRawConditions/LArMphysOverMcalMC.h"
#include "LArRawConditions/LArMCSym.h"


LArMphysOverMcalSym::LArMphysOverMcalSym(const LArMCSym* mcSym,  const LArMphysOverMcalMC* MphysOverMcalComplete) :
  m_mcSym(mcSym),
  m_MphysOverMcalComplete(MphysOverMcalComplete) //Implicit cast to base-class
{}

LArMphysOverMcalSym::~LArMphysOverMcalSym() {}
 
const float& LArMphysOverMcalSym::MphysOverMcal(const HWIdentifier&  hwid, int gain) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  return m_MphysOverMcalComplete->get(symhwid,gain).m_data;
}
  

