/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArAutoCorrSym.h"
#include "LArRawConditions/LArAutoCorrMC.h"
#include "LArRawConditions/LArMCSym.h"

LArAutoCorrSym::LArAutoCorrSym(const LArMCSym* mcSym,  const LArAutoCorrMC* acComplete) :
  m_mcSym(mcSym),
  m_acComplete(acComplete) //Implicit cast to base-class
{}

LArAutoCorrSym::~LArAutoCorrSym() {}
 
ILArAutoCorr::AutoCorrRef_t LArAutoCorrSym::autoCorr(const HWIdentifier&  hwid, int gain) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  return m_acComplete->get(symhwid,gain).m_vAutoCorr;
}
  
