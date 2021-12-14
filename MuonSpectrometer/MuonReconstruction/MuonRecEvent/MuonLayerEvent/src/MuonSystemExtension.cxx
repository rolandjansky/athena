/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerEvent/MuonSystemExtension.h"

namespace Muon {

    MuonSystemExtension::MuonSystemExtension(const Trk::TrackParameters* muonEntry,
                                             std::vector<Intersection>&& intersections) :
        m_muonEntryLayerIntersection{muonEntry}, m_layerIntersections(intersections) {}

    MuonSystemExtension::~MuonSystemExtension() = default;

}  // namespace Muon
