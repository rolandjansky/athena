/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArMinBiasAverageSC.h"


//const float LArMinBiasAverageSC::errorcode=ILArMinBiasAverage::ERRORCODE;

LArMinBiasAverageSC::LArMinBiasAverageSC()
  : LArCondSuperCellBase ("LArMinBiasAverageSC")
{}

LArMinBiasAverageSC::~LArMinBiasAverageSC() {}


LArMinBiasAverageSC::LArMinBiasAverageSC(const CondAttrListCollection* attrList)
  : LArCondSuperCellBase ("LArMinBiasAverageSC")
{
  if (initializeBase().isFailure()) return;
 
  readBlob(attrList,"MinBiasAverage",msg());

  if (m_pValues.size()!=1) {
    ATH_MSG_ERROR( "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." );
  }

  return;
}


const float& LArMinBiasAverageSC::minBiasAverage(const HWIdentifier& hwid) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->getDataByHash(hash, 0);
}

