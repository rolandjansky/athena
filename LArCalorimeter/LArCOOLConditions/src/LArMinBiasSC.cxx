/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArMinBiasSC.h"


//const float LArMinBiasSC::errorcode=ILArMinBias::ERRORCODE;

LArMinBiasSC::LArMinBiasSC() {}

LArMinBiasSC::~LArMinBiasSC() {}


LArMinBiasSC::LArMinBiasSC(const CondAttrListCollection* attrList) {
  StatusCode sc=initializeBase("LArMinBiasSC");
  if (sc.isFailure()) return;
 
   readBlob(attrList,"MinBias",*m_log);

  if (m_pValues.size()!=1) {
    (*m_log) << MSG::ERROR << "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." << endmsg;
  }

  return;
}


const float& LArMinBiasSC::minBiasRMS(const HWIdentifier& hwid) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->getDataByHash(hash, 0);
}

