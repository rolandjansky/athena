/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArAutoCorrSC.h" 
#include "AthenaPoolUtilities/CondAttrListCollection.h"

LArAutoCorrSC::LArAutoCorrSC()
  : LArCondSuperCellBase ("LArAutoCorrSC")
{}

LArAutoCorrSC::~LArAutoCorrSC()  {}


LArAutoCorrSC::LArAutoCorrSC(const CondAttrListCollection* attrList)
  : LArCondSuperCellBase ("LArAutoCorrSC")
{
  if (initializeBase().isFailure()) return;
 
  if (!attrList) return;

  readBlob(attrList,msg());

}

ILArAutoCorr::AutoCorrRef_t LArAutoCorrSC::autoCorr(const HWIdentifier& hwid, int /*gain*/) const { 
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->autocorrByHash(hash,0);
}

