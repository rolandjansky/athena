/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerEvent/MuonSystemExtension.h"

namespace Muon {

    MuonSystemExtension::MuonSystemExtension(std::unique_ptr<const Trk::TrackParameters> muonEntry,
                                             std::vector<Intersection>&& intersections) :
        m_muonEntryLayerIntersection(muonEntry.release()), m_layerIntersections(intersections) {}

    MuonSystemExtension::~MuonSystemExtension() {}

}  // namespace Muon
