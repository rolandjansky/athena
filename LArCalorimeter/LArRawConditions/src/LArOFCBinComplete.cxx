/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArOFCBinComplete.h"

LArOFCBinComplete::LArOFCBinComplete () {}

LArOFCBinComplete::~LArOFCBinComplete() {}

void LArOFCBinComplete::set(const HWIdentifier& chid, const int& gain, 
		       const int& bin) {
  this->setPdata(chid,LArOFCBinP(bin),gain);
  return;
}
