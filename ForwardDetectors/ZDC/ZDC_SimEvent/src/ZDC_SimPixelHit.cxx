/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZDC_SimEvent/ZDC_SimPixelHit.h"

const ZDC_SimPixelHit& ZDC_SimPixelHit::operator = (const ZDC_SimPixelHit& simhit) {
  
  Side           = simhit.Side;
  ModNo          = simhit.ModNo;
  PixNo          = simhit.PixNo;
  Nphotons       = simhit.Nphotons;
  Edep           = simhit.Edep;
  
  return *this;
}

int ZDC_SimPixelHit::operator == (const ZDC_SimPixelHit& simhit) const { return (this==&simhit) ? 1 : 0; }

bool ZDC_SimPixelHit::operator < (const ZDC_SimPixelHit&    rhs) const { 
  
  if (Side  < rhs.Side)  return true;
  if (ModNo < rhs.ModNo) return true;
  if (PixNo < rhs.PixNo) return true;

  return false;
}
