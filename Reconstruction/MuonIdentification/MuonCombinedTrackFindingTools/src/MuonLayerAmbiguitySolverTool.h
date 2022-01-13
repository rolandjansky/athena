/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERAMBIGUITYSOLVERTOOL_H
#define MUON_MUONLAYERAMBIGUITYSOLVERTOOL_H

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonLayerAmbiguitySolverTool.h"
#include "MuonLayerEvent/MuonCandidate.h"
#include "MuonLayerEvent/MuonLayerRecoData.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonSegmentTrackBuilder.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentMatchingTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"
#include "MuonStationIndex/MuonStationIndex.h"

namespace Muon {
    struct MuonCandidate;

    class MuonLayerAmbiguitySolverTool : virtual public IMuonLayerAmbiguitySolverTool, public AthAlgTool {
    public:
        /** Default AlgTool functions */
        MuonLayerAmbiguitySolverTool(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~MuonLayerAmbiguitySolverTool() = default;
        virtual StatusCode initialize() override;

        /**IMuonLayerAmbiguitySolverTool interface: find */
        virtual void resolveOverlaps(const EventContext& ctx, const std::vector<Muon::MuonLayerRecoData>& allLayers,
                                     std::vector<MuonCandidate>& resolvedCandidates) const override;

    private:
        void buildLayerVec(const EventContext& ctx, const std::vector<MuonLayerRecoData>& allLayers,
                           std::vector<std::vector<MuonLayerIntersection> >& muonLayerDataHashVec) const;

        bool getNextSeed(const std::vector<std::vector<MuonLayerIntersection> >& muonLayerDataHashVec,
                         std::set<const MuonSegment*>& usedSegments, std::vector<MuonStationIndex::StIndex>& inverseSeedLayerOrder,
                         MuonLayerIntersection& layerIntersection) const;

        bool extendCandidatesWithLayers(const EventContext& ctx, std::vector<MuonCandidate>& candidates,
                                        const std::vector<std::vector<MuonLayerIntersection> >& muonLayerRecoDataHashVec,
                                        const std::vector<MuonStationIndex::StIndex>& inverseSeedLayerOrder) const;

        void resolveSmallLargeOverlaps(const EventContext& ctx, std::vector<MuonLayerIntersection>& existingLayerIntersections,
                                       const std::vector<MuonLayerIntersection>& newLayerIntersections) const;

        bool match(const EventContext& ctx, const MuonCandidate& candidate, const MuonLayerIntersection& layerIntersection) const;

        ToolHandle<IMuonSegmentSelectionTool> m_segmentSelector{this, "MuonSegmentSelectionTool",
                                                                "Muon::MuonSegmentSelectionTool/MuonSegmentSelectionTool"};
        ToolHandle<IMuonSegmentMatchingTool> m_segmentMatchingTool{this, "MuonSegmentMatchingTool",
                                                                   "Muon::MuonSegmentMatchingTool/MuonSegmentMatchingToolTight"};
        ToolHandle<IMuonSegmentTrackBuilder> m_muonTrackBuilder{this, "MuonSegmentTrackBuilder",
                                                                "Muon::MooTrackBuilder/MooMuonTrackBuilder"};
        ToolHandle<MuonEDMPrinterTool> m_printer{this, "MuonEDMPrinterTool", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};

        Gaudi::Property<unsigned int> m_maxSeeds{this, "MaxSeeds", 30, "maximum number of seeds to be tried"};
        Gaudi::Property<int> m_seedQualityThreshold{this, "SeedQualityThreshold", 2, "seed quality threshold"};
        Gaudi::Property<int> m_minSegmentQuality{this, "MinimumSegmentQuality", 1, "minimum quality for a segment to be considered"};
    };
}  // namespace Muon

#endif
