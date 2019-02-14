/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArShapeSC.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"

LArShapeSC::LArShapeSC()   {}

LArShapeSC::~LArShapeSC() {}


LArShapeSC::LArShapeSC(const CondAttrListCollection* attrList) {
  StatusCode sc=initializeBase("LArShapeSC");
  if (sc.isFailure()) return;

  readBlob(attrList,*m_log);
}




LArShapeSC::ShapeRef_t LArShapeSC::Shape(const HWIdentifier& hwid, int  /*gain*/, int, int) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->ShapeByHash(hash,0);
}

LArShapeSC::ShapeRef_t LArShapeSC::ShapeDer(const HWIdentifier& hwid, int  /*gain*/, int, int) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->ShapeDerByHash(hash,0);
}

float LArShapeSC::timeOffset(const HWIdentifier& hwid , int) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return m_pTimeOffset[0][hash];
}


unsigned LArShapeSC::nTimeBins(const HWIdentifier&, int) const {
  return 1;
}


float LArShapeSC::timeBinWidth(const HWIdentifier&, int ) const {
  return (25./24.);
}
