/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"

ClassImp(HTTLogicalEventOutputHeader)

HTTLogicalEventOutputHeader::~HTTLogicalEventOutputHeader(){
  reset();
}

void HTTLogicalEventOutputHeader::reset()
{
    m_TruthTrack.clear();
    m_OfflineTrack.clear();
    m_HTTTrack.clear();
}

std::ostream& operator<<(std::ostream& s, const HTTLogicalEventOutputHeader& h)
{
    s  << "NTruthTracks = " << h.nTruthTracks() << ", "
       << "NOfflineTracks = " << h.nOfflineTracks() << ", "
       << "NHTTTracks = " << h.nHTTTracks() << std::endl;

    return s;
}
