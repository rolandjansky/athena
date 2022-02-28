/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArMphysOverMcalSC.h"


//const float LArMphysOverMcalSC::m_errorcode=ILArMphysOverMcal::ERRORCODE;

LArMphysOverMcalSC::LArMphysOverMcalSC()
  : LArCondSuperCellBase ("LArMphysOverMcalSC")
{}

LArMphysOverMcalSC::~LArMphysOverMcalSC() {}


LArMphysOverMcalSC::LArMphysOverMcalSC(const CondAttrListCollection* attrList)
  : LArCondSuperCellBase ("LArMphysOverMcalSC")
{
  if (initializeBase().isFailure()) return;
  readBlob(attrList,"MphysOverMcal",msg());

  return;
}


const float& LArMphysOverMcalSC::MphysOverMcal(const HWIdentifier& hwid, int /*gain*/) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
   return this->getDataByHash(hash,0);
}

