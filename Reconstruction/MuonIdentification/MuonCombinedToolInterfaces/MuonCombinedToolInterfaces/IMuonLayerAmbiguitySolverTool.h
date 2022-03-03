/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONLAYERAMBIGUITYSOLVERTOOL_H
#define MUON_IMUONLAYERAMBIGUITYSOLVERTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "MuonLayerEvent/MuonCandidate.h"
#include "MuonLayerEvent/MuonLayerRecoData.h"

namespace Muon {

    class MuonSegment;

    /** Interface for tools calculating hit count summaries for track */
    class IMuonLayerAmbiguitySolverTool : virtual public IAlgTool {
    public:
        /** IAlgTool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonLayerAmbiguitySolverTool("Muon::IMuonLayerAmbiguitySolverTool", 1, 0);
            return IID_IMuonLayerAmbiguitySolverTool;
        }

        /** @brief resolve ambiguities and return a set of unique combinations */
        virtual void resolveOverlaps(const EventContext& ctx, const std::vector<Muon::MuonLayerRecoData>& allLayers,
                                     std::vector<MuonCandidate>& resolvedCandidates) const = 0;

        virtual ~IMuonLayerAmbiguitySolverTool() = default;
    };

}  // namespace Muon

#endif  // IMuonLayerAmbiguitySolverTool_H
