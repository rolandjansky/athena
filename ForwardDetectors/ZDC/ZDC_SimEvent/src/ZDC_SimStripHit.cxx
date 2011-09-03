/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZDC_SimEvent/ZDC_SimStripHit.h"

const ZDC_SimStripHit& ZDC_SimStripHit::operator = (const ZDC_SimStripHit& simhit) {
  
  Side           = simhit.Side;
  ModNo          = simhit.ModNo;
  Nphotons       = simhit.Nphotons;
  Edep           = simhit.Edep;
  
  return *this;
}

int ZDC_SimStripHit::operator == (const ZDC_SimStripHit& simhit) const { return (this==&simhit) ? 1 : 0; }

bool ZDC_SimStripHit::operator < (const ZDC_SimStripHit&    rhs) const { 

  if (Side  < rhs.Side)  return true;
  if (ModNo < rhs.ModNo) return true;

  return false;
}
