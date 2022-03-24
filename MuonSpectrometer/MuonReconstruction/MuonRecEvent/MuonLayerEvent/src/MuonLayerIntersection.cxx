/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerEvent/MuonLayerIntersection.h"

namespace Muon {
    MuonLayerIntersection::MuonLayerIntersection(const MuonSystemExtension::Intersection& intersection_,
                                                 const std::shared_ptr<const MuonSegment>& segment_,
                                                 int _quality) :
        intersection{intersection_}, segment{segment_}, quality{_quality} {}

}  // namespace Muon
