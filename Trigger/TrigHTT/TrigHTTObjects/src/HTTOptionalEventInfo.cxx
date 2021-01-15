/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTOptionalEventInfo.h"
#include <iostream>

ClassImp(HTTOptionalEventInfo)

HTTOptionalEventInfo::~HTTOptionalEventInfo(){
  reset();
}

void HTTOptionalEventInfo::reset(){
  m_OfflineClusters.clear();
  m_Truth.clear();
  m_Track.clear();
}


std::ostream& operator<<(std::ostream& s, const HTTOptionalEventInfo& h) {
  s <<"NOfflineTracks="   << h.nOfflineTracks()
    <<"Ntruth="   << h.nTruthTracks()
    << "\t Noffline=" << h.nOfflineTracks();
  return s;
}

