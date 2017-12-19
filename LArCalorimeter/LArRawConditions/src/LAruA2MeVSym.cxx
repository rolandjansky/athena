/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LAruA2MeVSym.h"
#include "LArRawConditions/LAruA2MeVComplete.h"
#include "LArRawConditions/LAruA2MeVMC.h"
#include "LArRawConditions/LArMCSym.h"


LAruA2MeVSym::LAruA2MeVSym(const LArMCSym* mcSym,  const LAruA2MeVMC* uA2MeVComplete) :
  m_mcSym(mcSym),
  m_uA2MeVComplete(static_cast<const LAruA2MeVComplete*>(uA2MeVComplete)) //Cast to base-class
{}

LAruA2MeVSym::~LAruA2MeVSym() {}
 
const float&  LAruA2MeVSym::UA2MEV(const HWIdentifier&  hwid) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOnl(hwid);
  return m_uA2MeVComplete->UA2MEV(symhwid);
}
  
const float& LAruA2MeVSym::UA2MEV(const Identifier&  id) const {
  HWIdentifier symhwid=m_mcSym->ZPhiSymOfl(id);
  return m_uA2MeVComplete->UA2MEV(symhwid);
}
