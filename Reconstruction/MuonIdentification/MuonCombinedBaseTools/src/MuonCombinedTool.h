/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONCOMBINEDTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONCOMBINEDTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedDebuggerTool.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTagTool.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuidInterfaces/IMuonAlignmentUncertTool.h"

ATLAS_CHECK_FILE_THREAD_SAFETY;
namespace MuonCombined {

    class MuonCombinedTool : public AthAlgTool, virtual public IMuonCombinedTool {
    public:
        MuonCombinedTool(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~MuonCombinedTool() = default;

        virtual StatusCode initialize() override;

        virtual void combine(const MuonCandidateCollection& muonCandidates, const InDetCandidateCollection& inDetCandidates,
                             std::vector<InDetCandidateToTagMap*> tagMaps, TrackCollection* combinedTracks, TrackCollection* METracks,
                             const EventContext& ctx) const override;

    private:
        void associate(const MuonCandidate& muonCandidate, const InDetCandidateCollection& inDetCandidates,
                       std::vector<const InDetCandidate*>& associatedIdCandidates) const;

        void fill_debugging(const MuonCandidateCollection& muonCandidates, const InDetCandidateCollection& inDetCandidates) const;

        ///
        bool pass_prematching(const MuonCandidate& muonCandidate, const InDetCandidate& idCandidate) const;
        // helpers, managers, tools
        ToolHandle<Muon::MuonEDMPrinterTool> m_printer{this, "Printer", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
        ToolHandleArray<MuonCombined::IMuonCombinedTagTool> m_muonCombinedTagTools{this, "MuonCombinedTagTools", {}};
        ToolHandle<MuonCombinedDebuggerTool> m_muonCombDebugger{this, "MuonCombinedDebuggerTool",
                                                                "MuonCombined::MuonCombinedDebuggerTool/MuonCombinedDebuggerTool"};
        
        /// Use this tool to retrieve the last and first measurments of the ID and MS, respectively.
        PublicToolHandle<Muon::IMuonAlignmentUncertTool> m_alignUncertTool{this, "AlignmentUncertTool", ""};
  
        Gaudi::Property<float> m_deltaEtaPreSelection{this, "DeltaEtaPreSelection", 0.5};
        Gaudi::Property<float> m_deltaPhiPreSelection{this, "DeltaPhiPreSelection", 1};
        Gaudi::Property<float> m_ptBalance{this, "PtBalancePreSelection", -1.};

        Gaudi::Property<bool> m_runMuonCombinedDebugger{this, "RunMuonCombinedDebugger", false};
    };

}  // namespace MuonCombined

#endif
