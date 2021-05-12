/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "MuPatPrimitives/MuPatGarbage.h"
namespace Muon {
    void GarbageContainer::push_back(const MuPatSegment* segment) { push_back(segment, m_pat_segments); }
    void GarbageContainer::push_back(const MuonSegment* segment) { push_back(segment, m_muon_segments); }
    void GarbageContainer::push_back(const MuPatHit* hit) { push_back(hit, m_hits); }
    void GarbageContainer::push_back(const Trk::MeasurementBase* meas) { push_back(meas, m_measurements); }
    void GarbageContainer::push_back(const Trk::Perigee* perigee) { push_back(perigee, m_perigees); }

    void GarbageContainer::push_back(std::unique_ptr<const MuPatSegment> segment) { push_back(segment, m_pat_segments); }
    void GarbageContainer::push_back(std::unique_ptr<const MuonSegment> segment) { push_back(segment, m_muon_segments); }
    void GarbageContainer::push_back(std::unique_ptr<const MuPatHit> hit) { push_back(hit, m_hits); }
    void GarbageContainer::push_back(std::unique_ptr<const Trk::MeasurementBase> meas) { push_back(meas, m_measurements); }
    void GarbageContainer::push_back(std::unique_ptr<const Trk::Perigee> perigee) { push_back(perigee, m_perigees); }

}  // namespace Muon
