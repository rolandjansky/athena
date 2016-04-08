/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZDC_SimEvent/ZDC_SimStripHit.h"

int ZDC_SimStripHit::operator == (const ZDC_SimStripHit& simhit) const { return (this==&simhit) ? 1 : 0; }

bool ZDC_SimStripHit::operator < (const ZDC_SimStripHit&    rhs) const { 

  if (m_Side  < rhs.m_Side)  return true;
  if (m_ModNo < rhs.m_ModNo) return true;

  return false;
}
