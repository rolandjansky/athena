/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERRECODATA_H
#define MUON_MUONLAYERRECODATA_H

#include <vector>

#include "MuonLayerEvent/MuonSystemExtension.h"

namespace Muon {

    class MuonSegment;

    struct MuonLayerRecoData {
        /** constructor */
        MuonLayerRecoData(const MuonSystemExtension::Intersection& intersection_,
                          std::vector<std::shared_ptr<const MuonSegment> >&& segments_);

        /** intersection with layer */
        MuonSystemExtension::Intersection intersection;

        /** segments */
        std::vector<std::shared_ptr<const MuonSegment> > segments;
    };

}  // namespace Muon

#endif
