/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArHVScaleCorrSC.h"

//const float LArHVScaleCorrSC::errorcode=ILArHVScaleCorr::ERRORCODE;

LArHVScaleCorrSC::LArHVScaleCorrSC() {}

LArHVScaleCorrSC::~LArHVScaleCorrSC() {}


LArHVScaleCorrSC::LArHVScaleCorrSC(const CondAttrListCollection* attrList) {
  StatusCode sc=initializeBase("LArHVScaleCorrSC");
  if (sc.isFailure()) return;
 
   readBlob(attrList,"HVScaleCorr",*m_log);

  if (m_pValues.size()!=1) {
    (*m_log) << MSG::ERROR << "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." << endmsg;
  }

  return;
}


const float& LArHVScaleCorrSC::HVScaleCorr(const HWIdentifier& hwid) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->getDataByHash(hash, 0);
}

// retrieving LArHVScaleCorr using offline ID  
const float& LArHVScaleCorrSC::HVScaleCorr(const Identifier& id) const  {
  const HWIdentifier hwid=m_scCablingTool->createSignalChannelID(id);
  return  LArHVScaleCorrSC::HVScaleCorr(hwid);
}
