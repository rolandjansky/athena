/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArHVScaleCorrFlat.h"


LArHVScaleCorrFlat::LArHVScaleCorrFlat()
  : LArCondFlatBase("LArHVScaleCorrFlat")
{}

LArHVScaleCorrFlat::~LArHVScaleCorrFlat() 
{}


LArHVScaleCorrFlat::LArHVScaleCorrFlat(const CondAttrListCollection* attrList)
  : LArCondFlatBase("LArHVScaleCorrFlat")
{
  if (initializeBase().isFailure()) return;
  
  this->readBlob(attrList,"HVScaleCorr",msg());

  if (m_pValues.size()!=1) {
    ATH_MSG_ERROR( "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." );
 }

  return;
}


const float& LArHVScaleCorrFlat::HVScaleCorr(const HWIdentifier& chid) const {
  return getDataByHash(m_onlineHelper->channel_Hash(chid),0);

}

