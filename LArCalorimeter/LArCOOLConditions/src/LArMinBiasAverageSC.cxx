/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArMinBiasAverageSC.h"


//const float LArMinBiasAverageSC::errorcode=ILArMinBiasAverage::ERRORCODE;

LArMinBiasAverageSC::LArMinBiasAverageSC() {}

LArMinBiasAverageSC::~LArMinBiasAverageSC() {}


LArMinBiasAverageSC::LArMinBiasAverageSC(const CondAttrListCollection* attrList) {
  StatusCode sc=initializeBase("LArMinBiasAverageSC");
  if (sc.isFailure()) return;
 
   readBlob(attrList,"MinBiasAverage",*m_log);

  if (m_pValues.size()!=1) {
    (*m_log) << MSG::ERROR << "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." << endmsg;
  }

  return;
}


const float& LArMinBiasAverageSC::minBiasAverage(const HWIdentifier& hwid) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->getDataByHash(hash, 0);
}

