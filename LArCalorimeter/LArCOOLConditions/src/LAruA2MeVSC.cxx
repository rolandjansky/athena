/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LAruA2MeVSC.h"


LAruA2MeVSC::LAruA2MeVSC() {}

LAruA2MeVSC::~LAruA2MeVSC() {}

//const float LAruA2MeVSC::errorcode=ILAruA2MeV::ERRORCODE;

LAruA2MeVSC::LAruA2MeVSC(const CondAttrListCollection* attrList) {
  StatusCode sc=initializeBase("LAruA2MeVSC");
  if (sc.isFailure()) return;
 
  readBlob(attrList,"uA2MeV",*m_log);

  if (m_pValues.size()!=1) {
    (*m_log) << MSG::ERROR << "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." << endmsg;
  }

  return;
}


const float& LAruA2MeVSC::UA2MEV(const HWIdentifier& hwid) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
   return this->getDataByHash(hash, 0);
}

