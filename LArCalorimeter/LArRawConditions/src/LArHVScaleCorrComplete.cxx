/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArHVScaleCorrComplete.h"

void LArHVScaleCorrComplete::set(const HWIdentifier& CellID, const float corr) {
  this->setPdata(CellID,corr);
}

const float& LArHVScaleCorrComplete::HVScaleCorr(const Identifier&) const {
  std::cout << "LArHVScaleCorrComplete access by offline id not implemented!" << std::endl;
  std::abort();
  const static float errcode=LArElecCalib::ERRORCODE;
  return errcode;
}
