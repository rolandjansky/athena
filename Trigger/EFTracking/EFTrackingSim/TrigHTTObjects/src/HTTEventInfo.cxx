/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTEventInfo.h"
#include <iostream>

ClassImp(HTTEventInfo)


HTTEventInfo::~HTTEventInfo() {
  reset();
}


void HTTEventInfo::reset() {
  m_level1TriggerInfo.clear();
}

std::ostream& operator<<(std::ostream& s, const HTTEventInfo& h) {
  s << "Event " << h.eventNumber()
    << " \tRun " << h.runNumber();

  return s;
}

