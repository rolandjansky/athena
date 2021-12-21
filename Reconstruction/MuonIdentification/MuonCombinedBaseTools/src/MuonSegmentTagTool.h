/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONSEGMENTTAGTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONSEGMENTTAGTOOL_H

#include <array>
#include <atomic>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MSSurfaces.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"
#include "MuonCombinedToolInterfaces/IMuonSegmentTagTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagAmbiguitySolverTool.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagIMOTool.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagMatchingTool.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODMuon/MuonSegmentContainer.h"

namespace Muon {
    class MuonSegment;
}

namespace MuonCombined {

    class MuonSegmentTagTool : public AthAlgTool, virtual public IMuonSegmentTagTool {
        using SegmentMap = std::map<const Muon::MuonSegment*, ElementLink<xAOD::MuonSegmentContainer>>;

    public:
        MuonSegmentTagTool(const std::string& type, const std::string& name, const IInterface* parent);
        ~MuonSegmentTagTool() = default;

        StatusCode initialize()override;
        StatusCode finalize()override;

        /**IMuonSegmentTagTool interface: build muons from ID and MuonSegments */
        void tag(const EventContext& ctx, const InDetCandidateCollection& inDetCandidates, const xAOD::MuonSegmentContainer& segments,
                 InDetCandidateToTagMap* tagMap) const override;
        void tag(const EventContext& ctx, const InDetCandidateCollection& inDetCandidates, const std::vector<const Muon::MuonSegment*>& segments,
                 SegmentMap* segmentToxAODSegmentMap, InDetCandidateToTagMap* tagMap) const override;

    private:
        void printTable(const std::vector<std::string>& didEx, const std::vector<std::string> &segStation,
                       const std::vector<std::vector<std::string>>& segToSurf, MSSurfaces& surfaces) const;  //!< method for extra DEBUG

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
        ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc{this, "edmHelper", "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
                                                              "Handle to the service providing the IMuonEDMHelperSvc interface"};

        ToolHandle<Muon::MuonEDMPrinterTool> m_printer{this, "Printer", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
        ToolHandle<IMuTagMatchingTool> m_MuTagMatchingTool{this, "MuTagMatchingTool", "MuTagMatchingTool/MuTagMatchingTool"};
        ToolHandle<IMuTagAmbiguitySolverTool> m_MuTagAmbiguitySolverTool{this, "MuTagAmbiguitySolverTool",
                                                                         "MuTagAmbiguitySolverTool/MuTagAmbiguitySolverTool"};
   
        ToolHandle<Muon::IMuonSegmentSelectionTool> m_segmentSelector{this, "MuonSegmentSelectionTool",
                                                                      "Muon::MuonSegmentSelectionTool/MuonSegmentSelectionTool"};
        ToolHandle<Muon::IMuonSegmentHitSummaryTool> m_hitSummaryTool{this, "MuonSegmentHitSummaryTool",
                                                                      "Muon::MuonSegmentHitSummaryTool/MuonSegmentHitSummaryTool"};

      
        Gaudi::Property<bool> m_doSegmentsFilter{this, "DoSegmentsFilter", true, "flag to switch segment filtering on/off (for trigger)"};
        Gaudi::Property<bool> m_doTable{this, "DoOverviewTable", false};
        Gaudi::Property<bool> m_doBidirectional{this, "DoBidirectionalExtrapolation", false,
                                                "flag to toggle to bidirectional extrapolation"};
        Gaudi::Property<bool> m_doPtDependentPullCut{this, "DoPtDependentPullCut", false, "flag to enable the pT-dependent pull cut"};
        Gaudi::Property<bool> m_removeLowPLowFieldRegion{this, "RemoveLowPLowFieldRegion", false,
                                                         "remove track with p < 6 GeV in eta 1.4-17 region (low p and low field)"};
        Gaudi::Property<bool> m_useSegmentPreselection{this, "UseIDTrackSegmentPreSelect", true,
                                                       "use loose ID / segment matching to avoid extrapolations"};

        Gaudi::Property<int> m_segmentQualityCut{this, "SegmentQualityCut", 1, "minimum segment quality"};
        Gaudi::Property<unsigned int> m_nmdtHits{this, "nmdtHits", 4};
        Gaudi::Property<unsigned int> m_nmdtHoles{this, "nmdtHoles", 3};
        Gaudi::Property<unsigned int> m_nmdtHitsML{this, "nmdtHitsML", 2};

        Gaudi::Property<bool> m_triggerHitCut{this, "TriggerHitCut", true, "apply Trigger hit cut if trigger hits are expected"};
        Gaudi::Property<bool> m_makeMuons{this, "MakeMuons", false, "switch off the making of muons (temporarily)"};
        Gaudi::Property<bool> m_ignoreSiAssocated{this, "IgnoreSiAssociatedCandidates", true,
                                                  "If true, ignore InDetCandidates which are SiAssociated"};

        mutable std::atomic_uint m_ntotTracks{0};
        mutable std::atomic_uint m_nangleMatch{0};
        mutable std::atomic_uint m_npmatch{0};
        mutable std::atomic_uint m_natMSEntrance{0};
        mutable std::atomic_uint m_naccepted{0};
        mutable std::array<std::atomic_int, 15> m_extrapolated ATLAS_THREAD_SAFE{
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // 15 is maximum possible size. Guarded by atomicity
        mutable std::array<std::atomic_int, 15> m_goodExtrapolated ATLAS_THREAD_SAFE{
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // 15 is maximum possible size. Guarded by atomicity
    };

}  // namespace MuonCombined

#endif
