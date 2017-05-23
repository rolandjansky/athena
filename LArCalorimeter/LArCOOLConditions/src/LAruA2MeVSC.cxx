/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// retrieving LAruA2MeV using offline ID  
const float& LAruA2MeVSC::UA2MEV(const Identifier& cellID) const  {
  const HWIdentifier hwid=m_scCablingTool->createSignalChannelID(cellID);
  return LAruA2MeVSC::UA2MEV(hwid); 
}
