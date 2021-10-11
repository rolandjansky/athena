/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerEvent/MuonLayerRecoData.h"

namespace Muon {

    MuonLayerRecoData::MuonLayerRecoData(const MuonSystemExtension::Intersection& intersection_,
                                         std::vector<std::shared_ptr<const MuonSegment> >&& segments_) :
        intersection(intersection_), segments(segments_) {}

}  // namespace Muon
