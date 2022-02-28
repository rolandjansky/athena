/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSELECTORTOOLS_MUONSELECTIONTOOL_H
#define MUONSELECTORTOOLS_MUONSELECTIONTOOL_H

#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgTools/AsgTool.h"
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

	///Returns a vector of the muon's segments, sorted according to chamber index
	std::vector<const xAOD::MuonSegment*> getSegmentsSorted(const xAOD::Muon& mu) const;

        /// Maximum pseudorapidity for the selected muons
        double m_maxEta;
        int m_quality;

        /// Store selection information.
        asg::AcceptInfo m_acceptInfo;

        bool m_toroidOff;
        bool m_developMode;
        bool m_TrtCutOff;
        bool m_SctCutOff;
        bool m_PixCutOff;
        bool m_SiHolesCutOff;
        bool m_TurnOffMomCorr;
        bool m_useAllAuthors;
        bool m_use2stationMuonsHighPt;
        bool m_useMVALowPt;
	bool m_useSegmentTaggedLowPt;
	bool m_useCaloScore;
        bool m_doBadMuonVetoMimic;

        SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this, "EventInfoContName", "EventInfo", "event info key"};

        std::string m_MVAreaderFile_EVEN_MuidCB;
        std::string m_MVAreaderFile_ODD_MuidCB;
        std::string m_MVAreaderFile_EVEN_MuGirl;
        std::string m_MVAreaderFile_ODD_MuGirl;

	std::string m_MVAreaderFile_MuTagIMO_etaBin1;
	std::string m_MVAreaderFile_MuTagIMO_etaBin2;
	std::string m_MVAreaderFile_MuTagIMO_etaBin3;

        std::string m_BMVcutFile;

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

        // subfolder to load from the calibration db
        std::string m_calibration_version;
        // possible override for the calibration version
        std::string m_custom_dir;

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
        std::unique_ptr<TMVA::Reader> m_readerE_MUID;
        std::unique_ptr<TMVA::Reader> m_readerO_MUID;
        std::unique_ptr<TMVA::Reader> m_readerE_MUGIRL;
        std::unique_ptr<TMVA::Reader> m_readerO_MUGIRL;
	
	std::unique_ptr<TMVA::Reader> m_reader_MUTAGIMO_etaBin1;
	std::unique_ptr<TMVA::Reader> m_reader_MUTAGIMO_etaBin2;
	std::unique_ptr<TMVA::Reader> m_reader_MUTAGIMO_etaBin3;

        // variables for the TMVA readers
        mutable std::mutex m_low_pt_mva_mutex;

    };  // class MuonSelectionTool

}  // namespace CP

#endif  // CPTOOLTESTS_MUONSELECTIONTOOL_H
