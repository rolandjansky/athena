/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LAruA2MeVFlat.h"


LAruA2MeVFlat::LAruA2MeVFlat()
  : LArCondFlatBase("LAruA2MeVFlat")
{}

LAruA2MeVFlat::~LAruA2MeVFlat() {}


LAruA2MeVFlat::LAruA2MeVFlat(const CondAttrListCollection* attrList)
  : LArCondFlatBase("LAruA2MeVFlat")
{
  if (initializeBase().isFailure()) return;
 
  readBlob(attrList,"uA2MeV",msg());

  if (m_pValues.size()!=1) {
    ATH_MSG_ERROR( "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." );
  }

  return;
}


const float& LAruA2MeVFlat::UA2MEV(const HWIdentifier& chid) const {
  return getDataByHash(m_onlineHelper->channel_Hash(chid),0);
}

