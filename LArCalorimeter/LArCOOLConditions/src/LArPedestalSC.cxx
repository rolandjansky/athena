/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArPedestalSC.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

LArPedestalSC::LArPedestalSC()
  : LArCondSuperCellBase ("LArPedestalSC")
{}

LArPedestalSC::LArPedestalSC(const CondAttrListCollection* attrList)
  : LArCondSuperCellBase ("LArPedestalSC")
{
  if (initializeBase().isFailure()) return;
 
  readBlob(attrList,msg());
}
 

float  LArPedestalSC::pedestal(const HWIdentifier& hwid, int /*gain*/) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->pedestalByHash(hash,0);
}
  
float  LArPedestalSC::pedestalRMS(const HWIdentifier& hwid, int /*gain*/) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->pedestalRMSByHash(hash,0);
}
 
