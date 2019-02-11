/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArHVScaleCorrFlat.h"


LArHVScaleCorrFlat::LArHVScaleCorrFlat():
  LArSingleFloatBlob()
  {}

LArHVScaleCorrFlat::~LArHVScaleCorrFlat() 
{}


LArHVScaleCorrFlat::LArHVScaleCorrFlat(const CondAttrListCollection* attrList) {
  
  StatusCode sc=initializeBase("LArHVScaleCorrFlat");
  if (sc.isFailure()) return;
  
  this->readBlob(attrList,"HVScaleCorr",*m_log);

  if (m_pValues.size()!=1) {
    (*m_log) << MSG::ERROR << "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." << endmsg;
 }

  return;
}


const float& LArHVScaleCorrFlat::HVScaleCorr(const HWIdentifier& chid) const {
  return getDataByHash(m_onlineHelper->channel_Hash(chid),0);

}

