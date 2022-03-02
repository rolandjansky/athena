/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONINSIDEOUTRECOTOOL_H
#define MUON_MUONINSIDEOUTRECOTOOL_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuonCombinedToolInterfaces/IMuonCandidateTrackBuilderTool.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"
#include "MuonCombinedToolInterfaces/IMuonLayerAmbiguitySolverTool.h"
#include "MuonCombinedToolInterfaces/IMuonLayerSegmentMatchingTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonLayerEvent/MuonLayerRecoData.h"
#include "MuonPrepRawDataProviderTools/MuonLayerHashProviderTool.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecToolInterfaces/IMuonRecoValidationTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonLayerSegmentFinderTool.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include "xAODTracking/VertexContainer.h"

namespace Muon {
    struct MuonCandidate;
    struct MuonLayerPrepRawData;
}  // namespace Muon

namespace Trk {
    class Track;
    class IExtendedTrackSummaryTool;
}  // namespace Trk

static const InterfaceID IID_MuonInsideOutRecoTool("MuonCombined::MuonInsideOutRecoTool", 1, 0);

namespace MuonCombined {

    class MuonInsideOutRecoTool : virtual public MuonCombined::IMuonCombinedInDetExtensionTool, public AthAlgTool {
    public:
        /** Default AlgTool functions */
        MuonInsideOutRecoTool(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~MuonInsideOutRecoTool() = default;
        virtual StatusCode initialize() override;

        /** @brief access to tool interface */
        static const InterfaceID& interfaceID() { return IID_MuonInsideOutRecoTool; }

        /**IMuonCombinedInDetExtensionTool interface: extend ID candidate with PRDs for segment-finding */
        virtual void extendWithPRDs(const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap,
                                    IMuonCombinedInDetExtensionTool::MuonPrdData prdData, TrackCollection* combTracks,
                                    TrackCollection* meTracks, Trk::SegmentCollection* segments, const EventContext& ctx) const override;

        /**IMuonCombinedInDetExtensionTool interface: deprecated*/
        virtual void extend(const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, TrackCollection* combTracks,
                            TrackCollection* meTracks, Trk::SegmentCollection* segments, const EventContext& ctx) const override;

        /** find the best candidate for a given set of segments */
        std::pair<std::unique_ptr<const Muon::MuonCandidate>, std::unique_ptr<Trk::Track>> findBestCandidate(
            const EventContext& ctx, const xAOD::TrackParticle& indetTrackParticle,
            const std::vector<Muon::MuonLayerRecoData>& allLayers) const;

    private:
        /** handle a single candidate */
        void handleCandidate(const InDetCandidate& inDetCandidate, InDetCandidateToTagMap* tagMap,
                             IMuonCombinedInDetExtensionTool::MuonPrdData prdData, TrackCollection* combTracks, TrackCollection* meTracks,
                             Trk::SegmentCollection* segments, const EventContext& ctx) const;

        /** add muon candidate to indet candidate */
        void addTag(const EventContext& ctx, const InDetCandidate& indetCandidate, InDetCandidateToTagMap* tagMap,
                    const Muon::MuonCandidate& candidate, std::unique_ptr<Trk::Track>& selectedTrack, TrackCollection* combTracks,
                    TrackCollection* meTracks, Trk::SegmentCollection* segments) const;

        /** access data in layer */
        bool getLayerData(const Muon::MuonLayerSurface& surf, Muon::MuonLayerPrepRawData& layerPrepRawData,
                          IMuonCombinedInDetExtensionTool::MuonPrdData prdData) const;

        /** access data in layer for a given technology */
        template <class COL>
        bool getLayerDataTech(Muon::MuonStationIndex::TechnologyIndex technology, const Muon::MuonLayerSurface& surf,
                              const Muon::MuonPrepDataContainer<COL>* input, std::vector<const COL*>& output) const;

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
        ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc{this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
                                                              "Handle to the service providing the IMuonEDMHelperSvc interface"};
        ToolHandle<Muon::MuonEDMPrinterTool> m_printer{this, "MuonEDMPrinterTool", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
        ToolHandle<Muon::IMuonLayerSegmentFinderTool> m_segmentFinder{this, "MuonLayerSegmentFinderTool",
                                                                      "Muon::MuonLayerSegmentFinderTool/MuonLayerSegmentFinderTool"};
        ToolHandle<Muon::IMuonLayerSegmentMatchingTool> m_segmentMatchingTool{
            this, "MuonLayerSegmentMatchingTool", "Muon::MuonLayerSegmentMatchingTool/MuonLayerSegmentMatchingTool"};
        ToolHandle<Muon::IMuonLayerAmbiguitySolverTool> m_ambiguityResolver{
            this, "MuonLayerAmbiguitySolverTool", "Muon::MuonLayerAmbiguitySolverTool/MuonLayerAmbiguitySolverTool"};
        ToolHandle<Muon::IMuonCandidateTrackBuilderTool> m_candidateTrackBuilder{
            this, "MuonCandidateTrackBuilderTool", "Muon::MuonCandidateTrackBuilderTool/MuonCandidateTrackBuilderTool"};
        ToolHandle<Rec::ICombinedMuonTrackBuilder> m_trackFitter{this, "MuonTrackBuilder",
                                                                 "Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder"};
        ToolHandle<Trk::ITrackAmbiguityProcessorTool> m_trackAmbiguityResolver{this, "TrackAmbiguityProcessor",
                                                                               "Trk::TrackSelectionProcessorTool/MuonAmbiProcessor"};
        ToolHandle<Muon::MuonLayerHashProviderTool> m_layerHashProvider{this, "MuonLayerHashProviderTool",
                                                                        "Muon::MuonLayerHashProviderTool"};
        ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool{this, "TrackSummaryTool", "MuonTrackSummaryTool"};
        ToolHandle<Muon::IMuonRecoValidationTool> m_recoValidationTool{this, "MuonRecoValidationTool", ""};

        Gaudi::Property<double> m_idTrackMinPt{this, "IDTrackMinPt", 2500};
        Gaudi::Property<bool> m_ignoreSiAssocated{this, "IgnoreSiAssociatedCandidates", true};

        // vertex container key
        SG::ReadHandleKey<xAOD::VertexContainer> m_vertexKey{this, "VertexContainer", "PrimaryVertices", "vertex container key"};
    };
}  // namespace MuonCombined

#endif
