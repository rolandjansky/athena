/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerEvent/MuonLayerIntersection.h"

namespace Muon {

    MuonLayerIntersection::MuonLayerIntersection() {}
    MuonLayerIntersection::MuonLayerIntersection(const MuonSystemExtension::Intersection& intersection_,
                                                 const std::shared_ptr<const MuonSegment>& segment_) :
        intersection(intersection_), segment(segment_) {}

}  // namespace Muon
