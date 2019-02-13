/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArMphysOverMcalSC.h"


//const float LArMphysOverMcalSC::m_errorcode=ILArMphysOverMcal::ERRORCODE;

LArMphysOverMcalSC::LArMphysOverMcalSC() {}

LArMphysOverMcalSC::~LArMphysOverMcalSC() {}


LArMphysOverMcalSC::LArMphysOverMcalSC(const CondAttrListCollection* attrList) {
  StatusCode sc=initializeBase("LArMphysOverMcalSC");
  if (sc.isFailure()) return;
  readBlob(attrList,"MphysOverMcal",*m_log);

  return;
}


const float& LArMphysOverMcalSC::MphysOverMcal(const HWIdentifier& hwid, int /*gain*/) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
   return this->getDataByHash(hash,0);
}

