/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArShapeFlat.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"
#include "LArCabling/LArCablingService.h"

LArShapeFlat::LArShapeFlat()   {}

LArShapeFlat::~LArShapeFlat() {}


LArShapeFlat::LArShapeFlat(const CondAttrListCollection* attrList) {
  StatusCode sc=initializeBase("LArShapeFlat");
  if (sc.isFailure()) return;

  readBlob(attrList,*m_log);
}




LArShapeFlat::ShapeRef_t LArShapeFlat::Shape(const Identifier&  CellID, int gain, int tbin, int mode) const {
  const HWIdentifier OnId = m_larCablingSvc->createSignalChannelID(CellID);
  return Shape(OnId,gain,tbin,mode);
}
LArShapeFlat::ShapeRef_t LArShapeFlat::ShapeDer(const Identifier&  CellID, int gain, int tbin,  int mode) const {
  const HWIdentifier OnId = m_larCablingSvc->createSignalChannelID(CellID);
  return ShapeDer(OnId,gain,tbin,mode);
}

LArShapeFlat::ShapeRef_t LArShapeFlat::Shape(const HWIdentifier&  onId, int gain, int tbin, int/* mode*/) const {
  if (tbin!=0) return ShapeRef_t(NULL,NULL);
  return this->ShapeByHash(m_onlineHelper->channel_Hash(onId),gain);  
}

LArShapeFlat::ShapeRef_t LArShapeFlat::ShapeDer(const HWIdentifier&  onId, int gain, int tbin, int /*mode*/) const {
  if (tbin!=0) return ShapeRef_t(NULL,NULL);
  return this->ShapeDerByHash(m_onlineHelper->channel_Hash(onId),gain);  
}

float LArShapeFlat::timeOffset(const HWIdentifier&  CellID, int gain) const {
  const IdentifierHash hs=m_onlineHelper->channel_Hash(CellID);
  return m_pTimeOffset[gain][hs];
}


float LArShapeFlat::timeOffset(const Identifier&  CellID, int gain) const {
  const HWIdentifier OnId = m_larCablingSvc->createSignalChannelID(CellID);
  return timeOffset(OnId,gain);
}
  

unsigned LArShapeFlat::nTimeBins(const HWIdentifier&, int) const {
  return 1;
}

unsigned LArShapeFlat::nTimeBins(const Identifier&, int) const {
  return 1;
}
 

float LArShapeFlat::timeBinWidth(const HWIdentifier&, int ) const {
  return (25./24.);
}

float LArShapeFlat::timeBinWidth(const Identifier&, int) const {
  return (25./24.);
}
