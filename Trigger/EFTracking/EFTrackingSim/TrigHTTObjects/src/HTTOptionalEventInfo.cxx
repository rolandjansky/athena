/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTOptionalEventInfo.h"
#include <iostream>

ClassImp(HTTOptionalEventInfo)

HTTOptionalEventInfo::~HTTOptionalEventInfo() {
  reset();
}

void HTTOptionalEventInfo::reset() {
  m_OfflineClusters.clear();
  m_OfflineTracks.clear();
  m_TruthTracks.clear();
}

std::ostream& operator<<(std::ostream& s, const HTTOptionalEventInfo& info) {
  s << "nOfflineClusters: " << info.nOfflineClusters() << ", "
    << "nOfflineTracks: " << info.nOfflineTracks() << ", "
    << "nTruthTracks: " << info.nTruthTracks() << std::endl;
  return s;
}
