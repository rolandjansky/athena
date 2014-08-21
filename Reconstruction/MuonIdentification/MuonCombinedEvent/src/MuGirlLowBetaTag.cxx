/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/MuGirlLowBetaTag.h"
#include "TrkTrack/Track.h"
#include "MuonSegment/MuonSegment.h"

namespace MuonCombined {

  MuGirlLowBetaTag::MuGirlLowBetaTag( const Trk::Track* combinedTrack, const std::vector<const Muon::MuonSegment*>& segments ) :
    TagBase(TagBase::Author::MuGirlLowBeta,TagBase::Type::Combined), m_combinedTrack(combinedTrack),m_segments(segments) {}

  MuGirlLowBetaTag::MuGirlLowBetaTag( const std::vector<const Muon::MuonSegment*>& segments ) :
    TagBase(TagBase::Author::MuGirlLowBeta,TagBase::Type::SegmentTagged), m_combinedTrack(0),m_segments(segments) {}

  MuGirlLowBetaTag::~MuGirlLowBetaTag() {
    delete m_combinedTrack;
    for( auto x : m_segments ) delete x;
  }

}
