/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LAruA2MeVSC.h"


LAruA2MeVSC::LAruA2MeVSC()
  : LArCondSuperCellBase ("LAruA2MeVSC")
{}

LAruA2MeVSC::~LAruA2MeVSC() {}

//const float LAruA2MeVSC::errorcode=ILAruA2MeV::ERRORCODE;

LAruA2MeVSC::LAruA2MeVSC(const CondAttrListCollection* attrList)
  : LArCondSuperCellBase ("LAruA2MeVSC")
{
  if (initializeBase().isFailure()) return;
 
  readBlob(attrList,"uA2MeV",msg());

  if (m_pValues.size()!=1) {
    ATH_MSG_ERROR( "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." );
  }

  return;
}


const float& LAruA2MeVSC::UA2MEV(const HWIdentifier& hwid) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
   return this->getDataByHash(hash, 0);
}

