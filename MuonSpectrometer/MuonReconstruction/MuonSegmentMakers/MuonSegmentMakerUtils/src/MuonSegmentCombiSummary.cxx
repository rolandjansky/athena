/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentMakerUtils/MuonSegmentCombiSummary.h"

namespace Muon {

    MuonSegmentCombiSummary::MuonSegmentCombiSummary(const MuonSegmentCombination& combi) { m_observer_ptr = &combi; }
    bool MuonSegmentCombiSummary::has_ownership() const { return m_owner_combi.get() != nullptr; }
    MuonSegmentCombiSummary::MuonSegmentCombiSummary(std::unique_ptr<MuonSegmentCombination> segment) : m_owner_combi(std::move(segment)) {
        m_observer_ptr = m_owner_combi.get();
    }
    const MuonSegmentCombination* MuonSegmentCombiSummary::get() const { return m_observer_ptr; }
    const MuonSegmentCombination& MuonSegmentCombiSummary::segmentCombination() const { return *m_observer_ptr; }
    std::unique_ptr<MuonSegmentCombination> MuonSegmentCombiSummary::release() { return std::move(m_owner_combi); }

    MuonSegmentCombiSummary::ChSegVec& MuonSegmentCombiSummary::segmentsPerLayer() { return m_segmentsPerChamberLayer; }
    MuonSegmentCombiSummary::SegVec& MuonSegmentCombiSummary::chamberSegments(unsigned int chIndex) {
        return m_segmentsPerChamberLayer[chIndex];
    }
}  // namespace Muon
