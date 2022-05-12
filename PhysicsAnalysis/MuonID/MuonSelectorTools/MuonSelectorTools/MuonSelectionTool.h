/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSELECTORTOOLS_MUONSELECTIONTOOL_H
#define MUONSELECTORTOOLS_MUONSELECTIONTOOL_H

#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "PATCore/IAsgSelectionTool.h"
#include "TF1.h"
#include "TFile.h"
#include "TH2D.h"
#include "TMVA/Reader.h"
#include "TSystem.h"  // Replace with PathResolver
#include "xAODEventInfo/EventInfo.h"
namespace CP {

    /// Implementation of the muon selector tool
    ///
    /// Example implementation of how an object selector tool should
    /// look according to the TF3 recommendations.
    ///
    /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    ///
    /// $Revision: 299883 $
    /// $Date: 2014-03-28 18:34:16 +0100 (Fri, 28 Mar 2014) $
    ///
    class MuonSelectionTool : public virtual IAsgSelectionTool, public virtual IMuonSelectionTool, public asg::AsgTool {
        /// Create a proper constructor for Athena
        ASG_TOOL_CLASS2(MuonSelectionTool, IAsgSelectionTool, CP::IMuonSelectionTool)

    public:
        /// Constructor for standalone usage
        MuonSelectionTool(const std::string& name = "MuonSelection");

        virtual ~MuonSelectionTool();

        /// @name Function(s) implementing the asg::IAsgTool interface
        /// @{

        /// Function initialising the tool
        virtual StatusCode initialize() override;

        /// @}

        /// @name Function(s) implementing the IAsgSelectionTool interface
        /// @{

        /// Get an object describing the "selection steps" of the tool
        virtual const asg::AcceptInfo& getAcceptInfo() const override;

        /// Get the decision using a generic IParticle pointer
        virtual asg::AcceptData accept(const xAOD::IParticle* p) const override;

        /// @}

        /// @name Function(s) implementing the IMuonSelectorTool interface
        /// @{

        /// Get the decision for a specific Muon object
        virtual asg::AcceptData accept(const xAOD::Muon& mu) const override;

        /// set the passes ID cuts variable of the muon
        virtual void setPassesIDCuts(xAOD::Muon&) const override;

        /// set the passes high pT cuts variable of the muon
        virtual void setPassesHighPtCuts(xAOD::Muon& mu) const override;

        /// set the passes low pT cuts variable of the muon
        // void setPassesLowPtEfficiencyCuts( xAOD::Muon& mu ) const;

        /// set the passes quality variable of the muon
        virtual void setQuality(xAOD::Muon& mu) const override;

        /// Returns true if the muon passes the standard MCP ID cuts. To set the value on the muon, instead call
        /// setPassesIDCuts(xAOD::Muon&) const
        virtual bool passedIDCuts(const xAOD::Muon&) const override;

        /// Returns true if the muon passes a standardized loose preselection.
        virtual bool passedMuonCuts(const xAOD::Muon&) const override;

        /// Returns true if the track particle passes the standard MCP ID cuts.
        virtual bool passedIDCuts(const xAOD::TrackParticle&) const override;

        /// Returns true if the muon passes the standard MCP High Pt cuts. To set the value on the muon, instead call
        /// setPassesHighPtCuts(xAOD::Muon&) const
        virtual bool passedHighPtCuts(const xAOD::Muon&) const override;

        /// Returns true if the muon passes the standard MCP low pt cuts. To set the value on the muon, instead call
        /// setPassesLowPtEfficiencyCuts(xAOD::Muon&) const
        virtual bool passedLowPtEfficiencyCuts(const xAOD::Muon&) const override;
        virtual bool passedLowPtEfficiencyCuts(const xAOD::Muon&, xAOD::Muon::Quality thisMu_quality) const override;

        /// Returns true if a CB muon passes a pt- and eta-dependent cut on the relative CB q/p error
        virtual bool passedErrorCutCB(const xAOD::Muon&) const override;

        /// Returns true if a CB muon fails some loose quaility requirements designed to remove pathological tracks
        virtual bool isBadMuon(const xAOD::Muon&) const override;

        /// Returns the quality of the muon. To set the value on the muon, instead call setQuality(xAOD::Muon&) const
        virtual xAOD::Muon::Quality getQuality(const xAOD::Muon& mu) const override;

        /// Returns true if the muon passed additional calo-tag quality cuts
        virtual bool passedCaloTagQuality(const xAOD::Muon& mu) const override;

        /// Returns true if the muon passed the CaloScore calo-tag working point
        virtual bool passedCaloScore(const xAOD::Muon& mu) const override;

        /// Returns an integer corresponding to categorization of muons with different resolutions
        virtual int getResolutionCategory(const xAOD::Muon&) const override;
        /// @}

    private:
        bool passedLowPtEfficiencyMVACut(const xAOD::Muon&) const;

        /// Returns true if the muon passed the tight working point cuts
        bool passTight(const xAOD::Muon& mu, float rho, float oneOverPSig) const;

        /// Returns true if the muon passes a cut which mimics the effect of the combined error cut
        /// This is necessary only when the resolution is very optimistic in the MC such that a large smearing is applied
        bool passedBMVmimicCut(const xAOD::Muon&) const;

        /// Returns a vector of the muon's segments, sorted according to chamber index
        std::vector<const xAOD::MuonSegment*> getSegmentsSorted(const xAOD::Muon& mu) const;

        /// Store selection information.
        asg::AcceptInfo m_acceptInfo;

        Gaudi::Property<double> m_maxEta{this, "MaxEta", 2.7, "Maximum eta range to select the muons"};
        Gaudi::Property<int> m_quality{this, "MuQuality", 1,
                                       "Quality to select. Values correspond to 0, 1, 2, 3, 4=HighPt, 5=LowPtEfficiency"};

        Gaudi::Property<bool> m_toroidOff{this, "ToroidOff", false, "Run the tool in Toroid off setup"};

        // Expert development options
        Gaudi::Property<bool> m_TurnOffMomCorr{this, "TurnOffMomCorr", false};
        Gaudi::Property<bool> m_disablePtCuts{this, "DisablePtCuts", false};

        Gaudi::Property<bool> m_developMode{this, "ExpertDevelopMode", false};
        Gaudi::Property<bool> m_TrtCutOff{this, "TrtCutOff", true};
        Gaudi::Property<bool> m_SctCutOff{this, "SctCutOff", false};
        Gaudi::Property<bool> m_PixCutOff{this, "PixCutOff", false};
        Gaudi::Property<bool> m_SiHolesCutOff{this, "SiHolesCutOff", false};
        Gaudi::Property<bool> m_useAllAuthors{this, "UseAllAuthors", true};

        Gaudi::Property<bool> m_use2stationMuonsHighPt{
            this, "Use2stationMuonsHighPt", true, "for users of high-pT working point to choose whether to include 'safe' 2-station muons"};

        Gaudi::Property<bool> m_useMVALowPt{
            this, "UseMVALowPt", false,
            "for users of low-pT working point to choose whether to use MVA and whether to include MuTagIMO muons"};

        Gaudi::Property<bool> m_useSegmentTaggedLowPt{this, "UseSegmentTaggedLowPt", false, "Use MVA low-pt WP. In development phase"};

        Gaudi::Property<bool> m_useCaloScore{this, "UseCaloScore", false,
                                             "Switch to use CaloScore for calo-tags in the Loose working point - In development phase"};

        // switch to cut away the tail of very large smearing in MC to mimic the effect of the bad muon veto for 2-station muons in the
        // high-pT selection
        Gaudi::Property<bool> m_doBadMuonVetoMimic{this, "DoBadMuonVetoMimic", false};

        SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this, "EventInfoContName", "EventInfo", "event info key"};

        // MVA configs for low-pT working point. Expert use only!
        Gaudi::Property<std::string> m_MVAreaderFile_EVEN_MuidCB{
            this, "MVAreaderFile_EVEN_MuidCB",
            "MuonSelectorTools/190118_PrelimLowPtMVA/LowPtMVA_Weights/BDTG_9JAN2019_MuidCB_EVEN.weights.xml"};
        Gaudi::Property<std::string> m_MVAreaderFile_ODD_MuidCB{
            this, "MVAreaderFile_ODD_MuidCB",
            "MuonSelectorTools/190118_PrelimLowPtMVA/LowPtMVA_Weights/BDTG_9JAN2019_MuidCB_ODD.weights.xml"};

        Gaudi::Property<std::string> m_MVAreaderFile_EVEN_MuGirl{
            this, "MVAreaderFile_EVEN_MuGirl",
            "MuonSelectorTools/190118_PrelimLowPtMVA/LowPtMVA_Weights/BDTG_9JAN2019_MuGirl_EVEN.weights.xml"};
        Gaudi::Property<std::string> m_MVAreaderFile_ODD_MuGirl{
            this, "MVAreaderFile_ODD_MuGirl",
            "MuonSelectorTools/190118_PrelimLowPtMVA/LowPtMVA_Weights/BDTG_9JAN2019_MuGirl_ODD.weights.xml"};

        Gaudi::Property<std::string> m_MVAreaderFile_MuTagIMO_etaBin1{
            this, "MVAreaderFile_MuTagIMO_etaBin1", "dev/MuonSelectorTools/181121_MuTagIMO_BDT/BDT_NOV2021_MuTagIMO_etaBin1.weights.xml"};
        Gaudi::Property<std::string> m_MVAreaderFile_MuTagIMO_etaBin2{
            this, "MVAreaderFile_MuTagIMO_etaBin2", "dev/MuonSelectorTools/181121_MuTagIMO_BDT/BDT_NOV2021_MuTagIMO_etaBin2.weights.xml"};
        Gaudi::Property<std::string> m_MVAreaderFile_MuTagIMO_etaBin3{
            this, "MVAreaderFile_MuTagIMO_etaBin3", "dev/MuonSelectorTools/181121_MuTagIMO_BDT/BDT_NOV2021_MuTagIMO_etaBin3.weights.xml"};

        // subfolder to load from the calibration db
        Gaudi::Property<std::string> m_calibration_version{this, "CalibrationRelease", "PreRec2016_2016-04-13"};

        // possible override for the calibration version
        Gaudi::Property<std::string> m_custom_dir{this, "CustomInputFolder", ""};

        /// file for bad muon veto mimic cut functions
        Gaudi::Property<std::string> m_BMVcutFile{this, "BMVcutFile",
                                                  "MuonSelectorTools/180620_BMVmimicCutFunctions/BMVmimicCutFunctions.root"};

        /// Checks for each histogram
        StatusCode getHist(TFile* file, const std::string& histName, std::unique_ptr<TH1>& hist) const;
        //
        std::unique_ptr<TH1> m_tightWP_lowPt_rhoCuts;
        std::unique_ptr<TH1> m_tightWP_lowPt_qOverPCuts;
        std::unique_ptr<TH1> m_tightWP_mediumPt_rhoCuts;
        std::unique_ptr<TH1> m_tightWP_highPt_rhoCuts;
        //
        std::unique_ptr<TF1> m_BMVcutFunction_barrel;
        std::unique_ptr<TF1> m_BMVcutFunction_endcap;

        // Need run number (or random run number) to apply period-dependent selections.
        // If selection depends only on data taking year, this can be specified by passing
        // argument needOnlyCorrectYear=true, in which case the random run number decoration
        // from the pile-up reweighting tool is not needed.
        unsigned int getRunNumber(bool needOnlyCorrectYear = false) const;

        // Check if muon eta/phi falls in BIS7/8, BEE, or BMG chambers
        bool isBIS78(const float eta, const float phi) const;
        bool isBEE(const float eta, const float phi) const;
        bool isBMG(const float eta, const float phi) const;

        // TMVA readers for low-pT working point
        std::unique_ptr<TMVA::Reader> m_readerE_MUID{nullptr};
        std::unique_ptr<TMVA::Reader> m_readerO_MUID{nullptr};
        std::unique_ptr<TMVA::Reader> m_readerE_MUGIRL{nullptr};
        std::unique_ptr<TMVA::Reader> m_readerO_MUGIRL{nullptr};

        std::unique_ptr<TMVA::Reader> m_reader_MUTAGIMO_etaBin1{nullptr};
        std::unique_ptr<TMVA::Reader> m_reader_MUTAGIMO_etaBin2{nullptr};
        std::unique_ptr<TMVA::Reader> m_reader_MUTAGIMO_etaBin3{nullptr};

        // variables for the TMVA readers
        mutable std::mutex m_low_pt_mva_mutex;

        struct hitSummary {
            uint8_t nprecisionLayers{0};
            uint8_t nprecisionHoleLayers{0};
            uint8_t nGoodPrecLayers{0};
            uint8_t innerSmallHits{0};
            uint8_t innerLargeHits{0};
            uint8_t middleSmallHits{0};
            uint8_t middleLargeHits{0};
            uint8_t outerSmallHits{0};
            uint8_t outerLargeHits{0};
            uint8_t extendedSmallHits{0};
            uint8_t extendedLargeHits{0};
            uint8_t extendedSmallHoles{0};
            uint8_t isSmallGoodSectors{0};
            uint8_t cscUnspoiledEtaHits{0};
        };

        template <class P, class T, class S> inline void retrieveSummaryValue(const P& muon, T& value, const S type) const {
            if (!muon.summaryValue(value, type)) {
                ATH_MSG_FATAL(__FILE__ << ":" << __LINE__ << " Failed to retrieve summary value " << type);
                throw std::runtime_error("MuonSelectorTool summary retrieval failed");
            }
        }
        inline void fillSummary(const xAOD::Muon& muon, hitSummary& summary) const;

        inline void retrieveParam(const xAOD::Muon& muon, float& value, const xAOD::Muon::ParamDef param) const;

        inline float qOverPsignificance(const xAOD::Muon& muon) const;
        inline float rhoPrime(const xAOD::Muon& muon) const;

        inline void IdMsPt(const xAOD::Muon& muon, float& idPt, float& msPt) const;

    };  // class MuonSelectionTool

}  // namespace CP

#endif  // CPTOOLTESTS_MUONSELECTIONTOOL_H
