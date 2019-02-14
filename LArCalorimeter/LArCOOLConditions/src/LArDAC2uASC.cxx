/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArDAC2uASC.h"

//const float LArDAC2uASC::errorcode=ILArDAC2uA::ERRORCODE;

LArDAC2uASC::LArDAC2uASC() {}

LArDAC2uASC::~LArDAC2uASC() {}


LArDAC2uASC::LArDAC2uASC(const CondAttrListCollection* attrList) {
  StatusCode sc=initializeBase("LArDAC2uASC");
  if (sc.isFailure()) return;
 
   readBlob(attrList,"DAC2uA",*m_log);

  if (m_pValues.size()!=1) {
    (*m_log) << MSG::ERROR << "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." << endmsg;
  }

  return;
}


const float& LArDAC2uASC::DAC2UA(const HWIdentifier& hwid) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->getDataByHash(hash, 0);
}

