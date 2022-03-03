/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArDAC2uASC.h"

//const float LArDAC2uASC::errorcode=ILArDAC2uA::ERRORCODE;

LArDAC2uASC::LArDAC2uASC()
  : LArCondSuperCellBase ("LArDAC2uASC")
{}

LArDAC2uASC::~LArDAC2uASC() {}


LArDAC2uASC::LArDAC2uASC(const CondAttrListCollection* attrList)
  : LArCondSuperCellBase ("LArDAC2uASC")
{
  if (initializeBase().isFailure()) return;
 
  readBlob(attrList,"DAC2uA",msg());

  if (m_pValues.size()!=1) {
    ATH_MSG_ERROR( "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." );
  }

  return;
}


const float& LArDAC2uASC::DAC2UA(const HWIdentifier& hwid) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->getDataByHash(hash, 0);
}

