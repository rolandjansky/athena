/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArRampFlat.h" 
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"

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

