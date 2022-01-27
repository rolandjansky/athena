/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArDAC2uAFlat.h"


LArDAC2uAFlat::LArDAC2uAFlat()
  : LArCondFlatBase("LArDAC2uAFlat")
{}

LArDAC2uAFlat::~LArDAC2uAFlat() {}


LArDAC2uAFlat::LArDAC2uAFlat(const CondAttrListCollection* attrList)
  : LArCondFlatBase("LArDAC2uAFlat")
{
  if (initializeBase().isFailure()) return;
 
  readBlob(attrList,"DAC2uA",msg());

  if (m_pValues.size()!=1) {
    ATH_MSG_ERROR( "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." );
  }

  return;
}


const float& LArDAC2uAFlat::DAC2UA(const HWIdentifier& chid) const {
  return getDataByHash(m_onlineHelper->channel_Hash(chid),0);
}

