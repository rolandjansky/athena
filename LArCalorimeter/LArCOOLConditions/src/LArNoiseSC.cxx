/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArNoiseSC.h"


//const float LArNoiseSC::errorcode=ILArNoise::ERRORCODE;

LArNoiseSC::LArNoiseSC()
  : LArCondSuperCellBase ("LArNoiseSC")
{}

LArNoiseSC::~LArNoiseSC() {}


LArNoiseSC::LArNoiseSC(const CondAttrListCollection* attrList)
  : LArCondSuperCellBase ("LArNoiseSC")
{
  if (initializeBase().isFailure()) return;
 
  readBlob(attrList,"Noise",msg());

  return;
}


const float& LArNoiseSC::noise(const HWIdentifier& hwid, int /*gain*/) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->getDataByHash(hash, 0);
}

