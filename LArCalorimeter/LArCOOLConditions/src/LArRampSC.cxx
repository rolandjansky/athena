/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArRampSC.h" 
#include "AthenaPoolUtilities/CondAttrListCollection.h"

LArRampSC::LArRampSC()
  : LArCondSuperCellBase ("LArRampSC")
{}

LArRampSC::~LArRampSC()  {}


LArRampSC::LArRampSC(const CondAttrListCollection* attrList)
  : LArCondSuperCellBase ("LArRampSC")
{
  if (initializeBase().isFailure()) return;
 
  if (!attrList) return;

  readBlob(attrList,msg());

}

ILArRamp::RampRef_t LArRampSC::ADC2DAC(const HWIdentifier& hwid, int /*gain*/) const { 
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->ADC2DACByHash(hash, 0);
}

