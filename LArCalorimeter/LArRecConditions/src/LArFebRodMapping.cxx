/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecConditions/LArFebRodMapping.h"
#include "LArIdentifier/LArOnlineID.h"


LArFebRodMapping::LArFebRodMapping(const LArOnlineID* onlineId) :
  m_onlineId(onlineId),
  m_pFebHashtoROD(nullptr)

{}

HWIdentifier LArFebRodMapping::getReadoutModuleID(const HWIdentifier febId) const {
  const IdentifierHash fHash=m_onlineId->feb_Hash(febId);
  return getReadoutModuleIDFromHash(fHash);
}
