/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArDAC2uAFlat.h"


LArDAC2uAFlat::LArDAC2uAFlat() {}

LArDAC2uAFlat::~LArDAC2uAFlat() {}


LArDAC2uAFlat::LArDAC2uAFlat(const CondAttrListCollection* attrList) {
  StatusCode sc=initializeBase("LArDAC2uAFlat");
  if (sc.isFailure()) return;
 
   readBlob(attrList,"DAC2uA",*m_log);

  if (m_pValues.size()!=1) {
    (*m_log) << MSG::ERROR << "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." << endmsg;
  }

  return;
}


const float& LArDAC2uAFlat::DAC2UA(const HWIdentifier& chid) const {
  return getDataByHash(m_onlineHelper->channel_Hash(chid),0);
}

  // retrieving LArDAC2uA using offline ID  
const float& LArDAC2uAFlat::DAC2UA(const Identifier& id) const  {
  const HWIdentifier OnId = m_larCablingSvc->createSignalChannelID(id);
  return DAC2UA(OnId);
}
