/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/MuGirlLowBetaTag.h"
#include "TrkTrack/Track.h"
#include "MuonSegment/MuonSegment.h"

namespace MuonCombined {

  MuGirlLowBetaTag::MuGirlLowBetaTag( ElementLink<TrackCollection> combLink, const std::vector<ElementLink<Trk::SegmentCollection> >& segments ) :
    TagBase(TagBase::Author::MuGirlLowBeta,TagBase::Type::Combined), m_combLink(combLink), m_meLink(ElementLink<TrackCollection>()),m_segments(segments),
    m_muBeta(-9999.),m_stauSummary(0),m_stauExtras(0),m_rhExtras(0) {}

  MuGirlLowBetaTag::MuGirlLowBetaTag( const std::vector<ElementLink<Trk::SegmentCollection> >& segments ) :
    TagBase(TagBase::Author::MuGirlLowBeta,TagBase::Type::SegmentTagged), m_combLink(ElementLink<TrackCollection>()),m_meLink(ElementLink<TrackCollection>()),
    m_segments(segments),m_muBeta(-9999.),m_stauSummary(0),m_stauExtras(0),m_rhExtras(0) {}

  MuGirlLowBetaTag::~MuGirlLowBetaTag() {
    delete m_stauSummary;
    delete m_stauExtras;
    delete m_rhExtras;
    m_segments.clear();
  }

}
