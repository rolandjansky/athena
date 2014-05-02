/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArHVScaleCorrComplete.h"

const float& LArHVScaleCorrComplete::HVScaleCorr(const Identifier& id ) const {
  const HWIdentifier chid = larCablingSvc()->createSignalChannelID(id); 
  return (this->get(chid).m_data);
}


void LArHVScaleCorrComplete::set(const HWIdentifier& CellID, const float corr) {
  this->setPdata(CellID,corr);
}

