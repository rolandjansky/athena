/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSELECTORTOOLS_MUONSELECTIONTOOL_H
#define MUONSELECTORTOOLS_MUONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "PATCore/IAsgSelectionTool.h"
#include "TFile.h"
#include "TF1.h"
#include "TH2D.h"
#include "TSystem.h" // Replace with PathResolver
#include "TMVA/Reader.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"

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
   class MuonSelectionTool : public virtual IAsgSelectionTool,
                             public virtual IMuonSelectionTool,
                             public asg::AsgTool {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS2( MuonSelectionTool, IAsgSelectionTool,
                       CP::IMuonSelectionTool )

   public:

      /// Constructor for standalone usage
      MuonSelectionTool( const std::string& name = "MuonSelection");

     virtual ~MuonSelectionTool();

      /// @name Function(s) implementing the asg::IAsgTool interface
      /// @{

      /// Function initialising the tool
      virtual StatusCode initialize() override;

      /// @}

      /// @name Function(s) implementing the IAsgSelectionTool interface
      /// @{

      /// Get an object describing the "selection steps" of the tool
      virtual const Root::TAccept& getTAccept() const override;

      /// Get the decision using a generic IParticle pointer
      virtual const Root::TAccept& accept( const xAOD::IParticle* p ) const override;

      /// @}

      /// @name Function(s) implementing the IMuonSelectorTool interface
      /// @{

      /// Get the decision for a specific Muon object
      virtual const Root::TAccept& accept( const xAOD::Muon& mu ) const override;

      /// set the passes ID cuts variable of the muon 
      void setPassesIDCuts(xAOD::Muon&) const override;
	  
      /// set the passes high pT cuts variable of the muon 
      void setPassesHighPtCuts( xAOD::Muon& mu ) const override;

      /// set the passes low pT cuts variable of the muon
      //void setPassesLowPtEfficiencyCuts( xAOD::Muon& mu ) const;
     
      /// set the passes quality variable of the muon 
      void setQuality( xAOD::Muon& mu ) const override;

      /// Returns true if the muon passes the standard MCP ID cuts. To set the value on the muon, instead call setPassesIDCuts(xAOD::Muon&) const
      bool passedIDCuts(const xAOD::Muon&) const override;
 
      /// Returns true if the muon passes a standardized loose preselection.
      bool passedMuonCuts(const xAOD::Muon&) const override;

      /// Returns true if the track particle passes the standard MCP ID cuts.
      bool passedIDCuts(const xAOD::TrackParticle&) const override;
     
      /// Returns true if the muon passes the standard MCP High Pt cuts. To set the value on the muon, instead call setPassesHighPtCuts(xAOD::Muon&) const
      bool passedHighPtCuts(const xAOD::Muon&) const override;

      /// Returns true if the muon passes the standard MCP low pt cuts. To set the value on the muon, instead call setPassesLowPtEfficiencyCuts(xAOD::Muon&) const
      bool passedLowPtEfficiencyCuts(const xAOD::Muon&) const override;
      bool passedLowPtEfficiencyCuts(const xAOD::Muon&, xAOD::Muon::Quality thisMu_quality) const override;
      bool passedLowPtEfficiencyMVACut(const xAOD::Muon&) const;

      /// Returns true if a CB muon passes a pt- and eta-dependent cut on the relative CB q/p error
      bool passedErrorCutCB(const xAOD::Muon&) const override;

      /// Returns true if a CB muon fails some loose quaility requirements designed to remove pathological tracks
      bool isBadMuon(const xAOD::Muon&) const override;

      /// Returns true if the muon passes a cut which mimics the effect of the combined error cut
      /// This is necessary only when the resolution is very optimistic in the MC such that a large smearing is applied
      bool passedBMVmimicCut(const xAOD::Muon&) const;

      /// Returns the quality of the muon. To set the value on the muon, instead call setQuality(xAOD::Muon&) const
      xAOD::Muon::Quality getQuality( const xAOD::Muon& mu ) const override;

      /// Returns true if the muon passed additional calo-tag quality cuts
      bool passedCaloTagQuality (const xAOD::Muon& mu) const override;

      /// Returns true if the muon passed the tight working point cuts    
      bool passTight(const xAOD::Muon& mu, float rho, float oneOverPSig) const;

      /// Returns an integer corresponding to categorization of muons with different resolutions
      virtual int getResolutionCategory(const xAOD::Muon&) const override;
      /// @}


   private:

     MuonSelectionTool & operator=(const MuonSelectionTool &right);

     const std::string m_name;
      /// Maximum pseudorapidity for the selected muons
     double m_maxEta;
     /// xAOD::Muon::Quality m_quality;
     int  m_quality;
     
     /// Object used to store the last decision
     mutable Root::TAccept m_accept;
     
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
     bool m_doBadMuonVetoMimic;

     std::string m_eventInfoContName;

     std::string m_MVAreaderFile_EVEN_MuidCB;
     std::string m_MVAreaderFile_ODD_MuidCB;
     std::string m_MVAreaderFile_EVEN_MuGirl;
     std::string m_MVAreaderFile_ODD_MuGirl;

     std::string m_BMVcutFile;

     /// Checks for each histogram  
     StatusCode getHist( TFile* file, const char* histName, std::unique_ptr<TH2D>& hist );
     // 
     std::unique_ptr<TH2D> m_tightWP_lowPt_rhoCuts;
     std::unique_ptr<TH2D> m_tightWP_lowPt_qOverPCuts;
     std::unique_ptr<TH2D> m_tightWP_mediumPt_rhoCuts;
     std::unique_ptr<TH2D> m_tightWP_highPt_rhoCuts;
     //
     std::unique_ptr<TF1> m_BMVcutFunction_barrel;
     std::unique_ptr<TF1> m_BMVcutFunction_endcap;

     // subfolder to load from the calibration db
     std::string m_calibration_version;
     // possible override for the calibration version
     std::string m_custom_dir;

     //Need run number (or random run number) to apply period-dependent selections.
     //If selection depends only on data taking year, this can be specified by passing
     //argument needOnlyCorrectYear=true, in which case the random run number decoration
     //from the pile-up reweighting tool is not needed.
     unsigned int getRunNumber(bool needOnlyCorrectYear = false) const;

     //Check if muon eta/phi falls in BIS7/8, BEE, or BMG chambers
     bool isBIS78(const float eta, const float phi) const;
     bool isBEE(const float eta, const float phi) const;
     bool isBMG(const float eta, const float phi) const;

     //TMVA readers for low-pT working point
     std::unique_ptr<TMVA::Reader> readerE_MUID;
     std::unique_ptr<TMVA::Reader> readerO_MUID;
     std::unique_ptr<TMVA::Reader> readerE_MUGIRL;
     std::unique_ptr<TMVA::Reader> readerO_MUGIRL;

     //TMVA initialize function
     void PrepareReader(TMVA::Reader* reader);

     //variables for the TMVA readers
     Float_t *lowPTmva_middleHoles, *lowPTmva_muonSeg1ChamberIdx, *lowPTmva_muonSeg2ChamberIdx, *lowPTmva_momentumBalanceSig,
       *lowPTmva_scatteringCurvatureSig, *lowPTmva_scatteringNeighbourSig, *lowPTmva_energyLoss, *lowPTmva_muonSegmentDeltaEta;

   }; // class MuonSelectionTool

} // namespace CP

#endif // CPTOOLTESTS_MUONSELECTIONTOOL_H
