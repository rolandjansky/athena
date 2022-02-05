/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArMinBiasSC.h"


//const float LArMinBiasSC::errorcode=ILArMinBias::ERRORCODE;

LArMinBiasSC::LArMinBiasSC()
  : LArCondSuperCellBase ("LArMinBiasSC")
{}

LArMinBiasSC::~LArMinBiasSC() {}


LArMinBiasSC::LArMinBiasSC(const CondAttrListCollection* attrList)
  : LArCondSuperCellBase ("LArMinBiasSC")
{
  if (initializeBase().isFailure()) return;
 
  readBlob(attrList,"MinBias",msg());

  if (m_pValues.size()!=1) {
    ATH_MSG_ERROR( "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." );
  }

  return;
}


const float& LArMinBiasSC::minBiasRMS(const HWIdentifier& hwid) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->getDataByHash(hash, 0);
}

