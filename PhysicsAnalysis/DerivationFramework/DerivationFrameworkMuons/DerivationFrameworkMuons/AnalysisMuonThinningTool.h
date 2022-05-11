/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DERIVATIONFRAMEWORK_ANALYSISMUONTHINNINGTOOL__H
#define DERIVATIONFRAMEWORK_ANALYSISMUONTHINNINGTOOL__H

#include <AthenaBaseComps/AthAlgTool.h>
#include <DerivationFrameworkInterfaces/IThinningTool.h>
#include <MuonAnalysisInterfaces/IMuonSelectionTool.h>
#include <StoreGate/ReadDecorHandleKeyArray.h>
#include <StoreGate/ThinningHandleKey.h>
#include <xAODMuon/MuonContainer.h>

namespace DerivationFramework {

    class AnalysisMuonThinningTool : public AthAlgTool, virtual public IThinningTool {
    public:
        AnalysisMuonThinningTool(const std::string& t, const std::string& n, const IInterface* p);

        ~AnalysisMuonThinningTool() = default;

        virtual StatusCode initialize() override;

        virtual StatusCode doThinning() const override;

    private:
        ToolHandle<CP::IMuonSelectionTool> m_muonSelTool{this, "SelectionTool", "", "Configured instance of the MuonSelectionTool"};
        Gaudi::Property<int> m_quality{this, "QualityWP", xAOD::Muon::Loose,
                                       "Minimum working point that the muon has to satisfy in order to be survive"};

        SG::ReadDecorHandleKeyArray<xAOD::MuonContainer> m_MuonPassKeys{
            this, "MuonPassFlags", {}, "Decorators to safe analysis muons needed for MCP studies "};

        SG::ReadDecorHandleKeyArray<xAOD::TrackParticleContainer> m_TrkPassKeys{
            this, "TrkPassFlags", {}, "Decorator to safe tracks needed for MCP studies"};

        /// Thinning of unneeded muon tracks
        Gaudi::Property<std::string> m_streamName{this, "StreamName", "", "Name of the stream being thinned"};

        SG::ThinningHandleKey<xAOD::MuonContainer> m_muonKey{this, "muonThinning", "Muons", "Name of the muon container behind"};
        SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_IdTrkKey{this, "IdTrkThinnig", "InnerDetectorTrackParticles",
                                                                       "Thin the unneeded ID track particles associated with muons"};
        SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_FwdIdTrkKey{
            this, "IdTrkThinning", "InnerDetectorTrackParticles",
            "Thin the unneeded forward ID tracks (Needed for Sillicon associated Forward)"};
        SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_MSTrkKey{this, "MSTrkThinning", "MSTrackParticles",
                                                                       "Key to get rid of the unneeded MS tracks"};
        SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_METrkKey{this, "METrkThinning", "METrackParticles",
                                                                       "Key to get rid of the unneeded ME tracks"};
        SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_MSOETrkKey{this, "MSOETrkThinning", "MSOETrackParticles",
                                                                         "Key to get rid of the unneeded MSOE tracks"};
        SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_CmbTrkKey{this, "CmbTrkThinning", "CombinedTrackParticles",
                                                                        "Key to get rid of the unneeded MSOE tracks"};
        SG::ThinningHandleKey<xAOD::MuonSegmentContainer> m_SegmentKey{this, "SegmentThinning", "Segments",
                                                                       "Key to get rid of the unneeded segments"};
    };

}  // namespace DerivationFramework

#endif