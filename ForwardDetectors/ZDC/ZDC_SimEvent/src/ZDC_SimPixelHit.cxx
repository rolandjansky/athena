/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZDC_SimEvent/ZDC_SimPixelHit.h"

int ZDC_SimPixelHit::operator == (const ZDC_SimPixelHit& simhit) const { return (this==&simhit) ? 1 : 0; }

bool ZDC_SimPixelHit::operator < (const ZDC_SimPixelHit&    rhs) const { 
  
  if (m_Side  < rhs.m_Side)  return true;
  if (m_ModNo < rhs.m_ModNo) return true;
  if (m_PixNo < rhs.m_PixNo) return true;

  return false;
}
