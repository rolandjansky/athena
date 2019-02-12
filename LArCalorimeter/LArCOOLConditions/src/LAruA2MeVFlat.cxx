/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LAruA2MeVFlat.h"


LAruA2MeVFlat::LAruA2MeVFlat() {}

LAruA2MeVFlat::~LAruA2MeVFlat() {}


LAruA2MeVFlat::LAruA2MeVFlat(const CondAttrListCollection* attrList) {
  StatusCode sc=initializeBase("LAruA2MeVFlat");
  if (sc.isFailure()) return;
 
  readBlob(attrList,"uA2MeV",*m_log);

  if (m_pValues.size()!=1) {
    (*m_log) << MSG::ERROR << "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." << endmsg;
  }

  return;
}


const float& LAruA2MeVFlat::UA2MEV(const HWIdentifier& chid) const {
  return getDataByHash(m_onlineHelper->channel_Hash(chid),0);
}

