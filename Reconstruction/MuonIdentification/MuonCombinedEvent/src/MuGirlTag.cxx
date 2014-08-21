/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/MuGirlTag.h"
#include "TrkTrack/Track.h"
#include "MuonSegment/MuonSegment.h"

namespace MuonCombined {

  MuGirlTag::MuGirlTag( const Trk::Track* combinedTrack, const std::vector<const Muon::MuonSegment*>& segments ) :
    TagBase(TagBase::Author::MuGirl,TagBase::Type::Combined), m_combinedTrack(combinedTrack),m_segments(segments) {}

  MuGirlTag::MuGirlTag( const std::vector<const Muon::MuonSegment*>& segments ) :
    TagBase(TagBase::Author::MuGirl,TagBase::Type::SegmentTagged), m_combinedTrack(0),m_segments(segments) {}

  MuGirlTag::~MuGirlTag() {
    delete m_combinedTrack;
    for( auto x : m_segments ) delete x;
  }

}
