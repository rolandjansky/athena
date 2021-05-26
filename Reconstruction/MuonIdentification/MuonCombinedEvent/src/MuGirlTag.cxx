/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/MuGirlTag.h"
#include "TrkTrack/Track.h"
#include "MuonSegment/MuonSegment.h"

namespace MuonCombined {

  MuGirlTag::MuGirlTag( const ElementLink<TrackCollection>& comblink, const std::vector<ElementLink<Trk::SegmentCollection> >& segments ) :
    TagBase(TagBase::Author::MuGirl,TagBase::Type::Combined), m_combLink(comblink), m_meLink(ElementLink<TrackCollection>()), m_segments(segments) {}

  MuGirlTag::MuGirlTag( const std::vector<ElementLink<Trk::SegmentCollection> >& segments ) :
    TagBase(TagBase::Author::MuGirl,TagBase::Type::SegmentTagged), m_combLink(ElementLink<TrackCollection>()),m_meLink(ElementLink<TrackCollection>()),m_segments(segments) {}

  MuGirlTag::~MuGirlTag() {
    m_segments.clear();
  }

}
