/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArfSamplSC.h"


//const float LArfSamplSC::errorcode=ILArfSampl::ERRORCODE;

LArfSamplSC::LArfSamplSC() {}

LArfSamplSC::~LArfSamplSC() {}


LArfSamplSC::LArfSamplSC(const CondAttrListCollection* attrList) {
  StatusCode sc=initializeBase("LArfSamplSC");
  if (sc.isFailure()) return;
 
   readBlob(attrList,"fSampl",*m_log);

  if (m_pValues.size()!=1) {
    (*m_log) << MSG::ERROR << "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." << endmsg;
  }

  return;
}


const float& LArfSamplSC::FSAMPL(const HWIdentifier& hwid) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->getDataByHash(hash,0);
}

// retrieving LArfSampl using offline ID  
const float& LArfSamplSC::FSAMPL(const Identifier& id) const  {
  const HWIdentifier hwid=m_scCablingTool->createSignalChannelID(id);
  return LArfSamplSC::FSAMPL(hwid);
}
