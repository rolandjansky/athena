/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERINTERSECTION_H
#define MUON_MUONLAYERINTERSECTION_H

#include "MuonLayerEvent/MuonSystemExtension.h"
namespace Muon {

    class MuonSegment;

    struct MuonLayerIntersection {
        /** constructor */
        MuonLayerIntersection() = default;
        MuonLayerIntersection(const MuonSystemExtension::Intersection& intersection_, const std::shared_ptr<const MuonSegment>& segment_, int quality);

        /** intersection with layer */
        MuonSystemExtension::Intersection intersection{};

        /** segment */
        std::shared_ptr<const MuonSegment> segment{nullptr};

        int quality{0};
    };

}  // namespace Muon

#endif
