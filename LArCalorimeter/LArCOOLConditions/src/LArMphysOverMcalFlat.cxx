/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArMphysOverMcalFlat.h"


LArMphysOverMcalFlat::LArMphysOverMcalFlat()
  : LArCondFlatBase("LArMphysOverMcalFlat")
{}

LArMphysOverMcalFlat::~LArMphysOverMcalFlat() {}


LArMphysOverMcalFlat::LArMphysOverMcalFlat(const CondAttrListCollection* attrList)
  : LArCondFlatBase("LArMphysOverMcalFlat")
{
  if (initializeBase().isFailure()) return;
  
  this->readBlob(attrList,"MphysOverMcal",msg());

  return;
}

const float& LArMphysOverMcalFlat::MphysOverMcal(const HWIdentifier& chid, int gain) const {
  return getDataByHash(m_onlineHelper->channel_Hash(chid),gain);
}

