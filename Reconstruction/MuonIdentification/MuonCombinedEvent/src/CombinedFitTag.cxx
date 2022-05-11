/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/CombinedFitTag.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include <iostream>
#include "TrkTrack/Track.h"
#include "MuonSegment/MuonSegment.h"

#include "TrkTrack/Track.h"

namespace MuonCombined {

    CombinedFitTag::CombinedFitTag(xAOD::Muon::Author author, const MuonCandidate& muonCandidate, const Trk::TrackScore& score) :
        TagBase(author, TagBase::Type::Combined), m_muonCandidate(&muonCandidate), m_trackScore(score) {}

    CombinedFitTag::~CombinedFitTag() = default;

    std::string CombinedFitTag::toString() const {
        std::ostringstream sout;
        sout << name() << " TrackScore " << trackScore() << " matchChi2 " << matchChi2();
        return sout.str();
    }
    bool CombinedFitTag::isCommissioning() const { return muonCandidate().isCommissioning(); }
    const Trk::Track* CombinedFitTag::combinedTrack() const { return m_combLink.isValid() ? *m_combLink : nullptr; }
    const MuonCandidate& CombinedFitTag::muonCandidate() const { return *m_muonCandidate; }
    const Trk::Track* CombinedFitTag::updatedExtrapolatedTrack() const { return m_MELink.isValid() ? *m_MELink : nullptr; }
    void CombinedFitTag::innerMatch(double chi2, int dof, double prob) {
        m_matchChi2 = chi2;
        m_matchDoF = dof;
        m_matchProb = prob;
    }

    const Trk::Track* CombinedFitTag::primaryTrack() const { return combinedTrack(); }
    ElementLink<TrackCollection> CombinedFitTag::combinedTrackLink() const { return m_combLink; }
    void CombinedFitTag::setCombinedTrackLink(ElementLink<TrackCollection> combLink) { m_combLink = combLink; }
    ElementLink<TrackCollection> CombinedFitTag::updatedExtrapolatedTrackLink() const { return m_MELink; }
    void CombinedFitTag::setUpdatedExtrapolatedTrackLink(ElementLink<TrackCollection> MELink) { m_MELink = MELink; }
    double CombinedFitTag::matchChi2() const { return m_matchChi2; }
    int CombinedFitTag::matchDoF() const { return m_matchDoF; }
    double CombinedFitTag::matchProb() const { return m_matchProb; }
    void CombinedFitTag::fieldIntegral(Rec::FieldIntegral fieldI) { m_fieldIntegral = fieldI; }
    Rec::FieldIntegral CombinedFitTag::fieldIntegral() const { return m_fieldIntegral; }
    void CombinedFitTag::momentumBalanceSignificance(double m) { m_momentumBalanceSignificance = m; }
    double CombinedFitTag::momentumBalanceSignificance() const { return m_momentumBalanceSignificance; }
    const Trk::TrackScore& CombinedFitTag::trackScore() const { return m_trackScore; }
    std::string CombinedFitTag::name() const { return "CombinedFitTag"; }

    std::vector<const Muon::MuonSegment*> CombinedFitTag::associatedSegments() const {
        std::vector<const Muon::MuonSegment*> assoc_seg;
        assoc_seg.reserve(muonCandidate().getSegments().size());
        for (const ElementLink<xAOD::MuonSegmentContainer>& seg_link : muonCandidate().getSegments()) {
            if (!seg_link.isValid()) continue;
            const xAOD::MuonSegment* seg = (*seg_link);
            const Trk::Segment* trk_seg =*seg->muonSegment();
            const Muon::MuonSegment* muon_seg = dynamic_cast<const Muon::MuonSegment*>(trk_seg);
            if (muon_seg) assoc_seg.push_back(muon_seg);             
        }
        return assoc_seg;
    }

}  // namespace MuonCombined
