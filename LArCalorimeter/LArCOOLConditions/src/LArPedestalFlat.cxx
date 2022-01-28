/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArPedestalFlat.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"

LArPedestalFlat::LArPedestalFlat()
  : LArCondFlatBase("LArPedestalFlat")
{}


LArPedestalFlat::LArPedestalFlat(const CondAttrListCollection* attrList)
  : LArCondFlatBase("LArPedestalFlat")
{
  if (initializeBase().isFailure()) return;
 
  readBlob(attrList,msg());
  return;
}


float  LArPedestalFlat::pedestal(const HWIdentifier&  CellID, int gain) const {
  return this->pedestalByHash(m_onlineHelper->channel_Hash(CellID),gain);
}
  
float  LArPedestalFlat::pedestalRMS(const HWIdentifier&  CellID, int gain) const {
  return this->pedestalRMSByHash(m_onlineHelper->channel_Hash(CellID),gain);
}
