/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/MuGirlTag.h"

#include "MuonSegment/MuonSegment.h"
#include "TrkTrack/Track.h"

namespace MuonCombined {

    MuGirlTag::MuGirlTag(const ElementLink<TrackCollection>& comblink, const std::vector<ElementLink<Trk::SegmentCollection> >& segments) :
        TagBase(TagBase::Author::MuGirl, TagBase::Type::Combined),
        m_combLink(comblink),
        m_meLink(ElementLink<TrackCollection>()),
        m_segments(segments) {}

    MuGirlTag::MuGirlTag(const std::vector<ElementLink<Trk::SegmentCollection> >& segments) :
        TagBase(TagBase::Author::MuGirl, TagBase::Type::SegmentTagged),
        m_combLink(ElementLink<TrackCollection>()),
        m_meLink(ElementLink<TrackCollection>()),
        m_segments(segments) {}

    MuGirlTag::~MuGirlTag() = default;

    const Trk::Track* MuGirlTag::combinedTrack() const { return m_combLink.isValid() ? *m_combLink : nullptr; }

    const Trk::Track* MuGirlTag::updatedExtrapolatedTrack() const { return m_meLink.isValid() ? *m_meLink : nullptr; }

    const std::vector<ElementLink<Trk::SegmentCollection> >& MuGirlTag::segments() const { return m_segments; }

    const Trk::Track* MuGirlTag::primaryTrack() const { return combinedTrack(); }

    ElementLink<TrackCollection> MuGirlTag::combinedTrackLink() const { return m_combLink; }
    ElementLink<TrackCollection> MuGirlTag::updatedExtrapolatedTrackLink() const { return m_meLink; }
    void MuGirlTag::setUpdatedExtrapolatedTrack(ElementLink<TrackCollection> melink) { m_meLink = melink; }

    std::string MuGirlTag::name() const { return "MuGirlTag"; }
    std::string MuGirlTag::toString() const { return name(); }
    bool operator<(const MuGirlTag& t1, const MuGirlTag& t2) {
        const Trk::FitQuality* t1FQ = t1.combinedTrack() ? t1.combinedTrack()->fitQuality() : nullptr;
        const Trk::FitQuality* t2FQ = t2.combinedTrack() ? t2.combinedTrack()->fitQuality() : nullptr;
        if (t1FQ && t2FQ) { return t1FQ->chiSquared() < t2FQ->chiSquared(); }
        return t1.segments().size() < t2.segments().size();
    }
    void MuGirlTag::setCommissioning(bool b) { m_isCommissioning = b; }
    bool MuGirlTag::isCommissioning() const { return m_isCommissioning; }
   

}  // namespace MuonCombined
