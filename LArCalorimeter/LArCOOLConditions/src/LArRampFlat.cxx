/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArRampFlat.h" 
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"
#include "LArCabling/LArCablingService.h"

LArRampFlat::LArRampFlat() {}

LArRampFlat::~LArRampFlat()  {}


LArRampFlat::LArRampFlat(const CondAttrListCollection* attrList) {

 
  StatusCode sc=initializeBase("LArRampFlat");
  if (sc.isFailure()) return;
 
  readBlob(attrList,*m_log);
}

LArRampFlat::RampRef_t LArRampFlat::ADC2DAC(const HWIdentifier& CellID, int gain) const 
{ 
  return ADC2DACByHash(m_onlineHelper->channel_Hash(CellID),gain);  
}

LArRampFlat::RampRef_t LArRampFlat::ADC2DAC(const Identifier&  CellID, int gain) const
{
  const HWIdentifier OnId = m_larCablingSvc->createSignalChannelID(CellID);
  return this->ADC2DAC(OnId, gain);
}

