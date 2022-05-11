/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTMatchInfo.h"
#include <iostream>

ClassImp(HTTMatchInfo)

std::ostream& operator<<(std::ostream& s, const HTTMatchInfo& h) {

  s << "barcode: " << h.barcode()
    << ", event index: " << h.evtindex() << std::endl;

  return s;
}
