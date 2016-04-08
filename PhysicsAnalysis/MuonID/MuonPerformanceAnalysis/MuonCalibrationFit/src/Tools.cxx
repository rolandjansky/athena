/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define __Tools_C__
#include "MuonCalibrationFit/Tools.h"

bool Tools::RegionsSorter( RegionInfo* a, RegionInfo* b ) {
  return ( a->Order < b->Order );
}
