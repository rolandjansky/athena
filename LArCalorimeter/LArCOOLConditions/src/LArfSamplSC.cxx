/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArfSamplSC.h"


//const float LArfSamplSC::errorcode=ILArfSampl::ERRORCODE;

LArfSamplSC::LArfSamplSC():m_null(0.) {}

LArfSamplSC::~LArfSamplSC() {}


LArfSamplSC::LArfSamplSC(const CondAttrListCollection* attrList):m_null(0.) {
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
const float& LArfSamplSC::FSAMPL(const Identifier& /*id*/) const  {
  (*m_log) << MSG::WARNING << "LArfSamplSC::FSAMPL not implemented for CellId !!!" << endmsg;
  return m_null; 
}
