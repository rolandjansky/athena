/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"

ClassImp(HTTLogicalEventOutputHeader)

HTTLogicalEventOutputHeader::~HTTLogicalEventOutputHeader() {
  reset();
}

void HTTLogicalEventOutputHeader::reset() {
  m_HTTRoads_1st.clear();
  m_HTTRoads_2nd.clear();
  m_HTTTracks_1st.clear();
  m_HTTTracks_2nd.clear();
  m_dataflowInfo.reset();
}

std::ostream& operator<<(std::ostream& s, HTTLogicalEventOutputHeader const& h) {

  s << "NHTTRoads_1st = " << h.nHTTRoads_1st() << ", "
    << "NHTTRoads_2nd = " << h.nHTTRoads_2nd() << ", "
    << "NHTTTracks_1st = " << h.nHTTTracks_1st() << ", "
    << "NHTTTracks_2nd = " << h.nHTTTracks_2nd() << std::endl;

  return s;
}
