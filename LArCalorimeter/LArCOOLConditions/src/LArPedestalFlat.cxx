/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArPedestalFlat.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "LArCabling/LArCablingService.h"

LArPedestalFlat::LArPedestalFlat()  {}


LArPedestalFlat::LArPedestalFlat(const CondAttrListCollection* attrList) {
 
  StatusCode sc=initializeBase("LArPedestalFlat");
  if (sc.isFailure()) return;
 
  readBlob(attrList,*m_log);
  return;
}


float LArPedestalFlat::pedestal(const Identifier&  CellID, int gain) const {
  const HWIdentifier OnId = m_larCablingSvc->createSignalChannelID(CellID);
  return this->pedestal(OnId,gain);
}

float LArPedestalFlat::pedestalRMS(const Identifier&  CellID, int gain) const {
  const HWIdentifier OnId = m_larCablingSvc->createSignalChannelID(CellID);
  return this->pedestalRMS(OnId,gain);
}


float  LArPedestalFlat::pedestal(const HWIdentifier&  CellID, int gain) const {
  return this->pedestalByHash(m_onlineHelper->channel_Hash(CellID),gain);
}
  
float  LArPedestalFlat::pedestalRMS(const HWIdentifier&  CellID, int gain) const {
  return this->pedestalRMSByHash(m_onlineHelper->channel_Hash(CellID),gain);
}
