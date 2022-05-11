/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/MuGirlLowBetaTag.h"

#include "MuonSegment/MuonSegment.h"
#include "TrkTrack/Track.h"

namespace MuonCombined {

    MuGirlLowBetaTag::MuGirlLowBetaTag(const ElementLink<TrackCollection>& combLink,
                                       const std::vector<ElementLink<Trk::SegmentCollection> >& segments) :
        TagBase(TagBase::Author::MuGirlLowBeta, TagBase::Type::Combined), m_combLink{combLink}, m_segments{segments} {}

    MuGirlLowBetaTag::MuGirlLowBetaTag(const std::vector<ElementLink<Trk::SegmentCollection> >& segments) :
        TagBase(TagBase::Author::MuGirlLowBeta, TagBase::Type::SegmentTagged), m_segments{segments} {}

    MuGirlLowBetaTag::~MuGirlLowBetaTag() = default;

    const Trk::Track* MuGirlLowBetaTag::combinedTrack() const { return m_combLink.isValid() ? *m_combLink : nullptr; }

    const Trk::Track* MuGirlLowBetaTag::updatedExtrapolatedTrack() const { return m_meLink.isValid() ? *m_meLink : nullptr; }

    void MuGirlLowBetaTag::setUpdatedExtrapolatedTrack(ElementLink<TrackCollection> meLink) { m_meLink = meLink; }

    const std::vector<ElementLink<Trk::SegmentCollection> >& MuGirlLowBetaTag::segments() const { return m_segments; }

    const Trk::Track* MuGirlLowBetaTag::primaryTrack() const { return combinedTrack(); }
    std::vector<const Muon::MuonSegment*> MuGirlLowBetaTag::associatedSegments() const {
        std::vector<const Muon::MuonSegment*> ret{};
        for (const ElementLink<Trk::SegmentCollection>& link : m_segments) {
            const Muon::MuonSegment* muo_seg = dynamic_cast<const Muon::MuonSegment*>(*link);
            ret.push_back(muo_seg);
        }
        return ret;
    }
    // stau dressing
    void MuGirlLowBetaTag::setMuBeta(float muBeta) { m_muBeta = muBeta; }
    float MuGirlLowBetaTag::getMuBeta() { return m_muBeta; }

    void MuGirlLowBetaTag::setStauSummary(MuGirlNS::CandidateSummary* stauSummary) { m_stauSummary.reset(stauSummary); }
    MuGirlNS::CandidateSummary* MuGirlLowBetaTag::getStauSummary() { return m_stauSummary.get(); }

    void MuGirlLowBetaTag::setStauExtras(MuGirlNS::StauExtras* stauExtras) { m_stauExtras.reset(stauExtras); }
    MuGirlNS::StauExtras* MuGirlLowBetaTag::getStauExtras() { return m_stauExtras.get(); }
    const MuGirlNS::StauExtras* MuGirlLowBetaTag::getStauExtras() const { return m_stauExtras.get(); }

    void MuGirlLowBetaTag::setRHExtras(MuGirlNS::RHExtras* rhExtras) { m_rhExtras.reset(rhExtras); }
    MuGirlNS::RHExtras* MuGirlLowBetaTag::getRHExtras() { return m_rhExtras.get(); }

}  // namespace MuonCombined
