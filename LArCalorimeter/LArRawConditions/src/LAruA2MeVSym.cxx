/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LAruA2MeVSym.h"
#include "LArRawConditions/LAruA2MeVMC.h"
#include "LArRawConditions/LArMCSym.h"


LAruA2MeVSym::LAruA2MeVSym(const LArMCSym* mcSym,  const LAruA2MeVMC* uA2MeVComplete) :
  m_mcSym(mcSym),
  m_uA2MeVComplete(uA2MeVComplete) //Implicit cast to base-class
{}

LAruA2MeVSym::~LAruA2MeVSym() {}
 
const float& LAruA2MeVSym::UA2MEV(const HWIdentifier&  hwid) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  return m_uA2MeVComplete->get(symhwid).m_data;
}
