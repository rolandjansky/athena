// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonSelectionTool.h 299883 2014-03-28 17:34:16Z krasznaa $
#ifndef MUONSELECTORTOOLS_MUONSELECTIONTOOL_H
#define MUONSELECTORTOOLS_MUONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "PATCore/IAsgSelectionTool.h"
#include "TFile.h"
#include "TH2D.h"
#include "TSystem.h" // Replace with PathResolver
#include "TMVA/Reader.h"

// Local include(s):
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
      virtual StatusCode initialize();

      /// @}

      /// @name Function(s) implementing the IAsgSelectionTool interface
      /// @{

      /// Get an object describing the "selection steps" of the tool
      virtual const Root::TAccept& getTAccept() const;

      /// Get the decision using a generic IParticle pointer
      virtual const Root::TAccept& accept( const xAOD::IParticle* p ) const;

      /// @}

      /// @name Function(s) implementing the IMuonSelectorTool interface
      /// @{

      /// Get the decision for a specific Muon object
      virtual const Root::TAccept& accept( const xAOD::Muon& mu ) const;

      /// set the passes ID cuts variable of the muon 
      void setPassesIDCuts(xAOD::Muon&) const;
	  
      /// set the passes high pT cuts variable of the muon 
      void setPassesHighPtCuts( xAOD::Muon& mu ) const;

      /// set the passes low pT cuts variable of the muon
      //void setPassesLowPtEfficiencyCuts( xAOD::Muon& mu ) const;
     
      /// set the passes quality variable of the muon 
      void setQuality( xAOD::Muon& mu ) const;

      /// Returns true if the muon passes the standard MCP ID cuts. To set the value on the muon, instead call setPassesIDCuts(xAOD::Muon&) const
      bool passedIDCuts(const xAOD::Muon&) const;
 
      /// Returns true if the muon passes a standardized loose preselection.
      bool passedMuonCuts(const xAOD::Muon&) const;

      /// Returns true if the track particle passes the standard MCP ID cuts.
      bool passedIDCuts(const xAOD::TrackParticle&) const;
     
      /// Returns true if the muon passes the standard MCP High Pt cuts. To set the value on the muon, instead call setPassesHighPtCuts(xAOD::Muon&) const
      bool passedHighPtCuts(const xAOD::Muon&) const;

      /// Returns true if the muon passes the standard MCP low pt cuts. To set the value on the muon, instead call setPassesLowPtEfficiencyCuts(xAOD::Muon&) const
      bool passedLowPtEfficiencyCuts(const xAOD::Muon&) const;
      bool passedLowPtEfficiencyCuts(const xAOD::Muon&, xAOD::Muon::Quality thisMu_quality) const;
      bool passedLowPtEfficiencyMVACut(const xAOD::Muon&) const;

      /// Returns true if a CB muon fails a pt- and eta-dependent cut on the relative CB q/p error
      bool passedErrorCutCB(const xAOD::Muon&) const;

      /// Returns true if a CB muon fails some loose quaility requirements designed to remove pathological tracks
      bool isBadMuon(const xAOD::Muon&) const;

      /// Returns the quality of the muon. To set the value on the muon, instead call setQuality(xAOD::Muon&) const
      xAOD::Muon::Quality getQuality( const xAOD::Muon& mu ) const;

      /// Returns true if the muon passed additional calo-tag quality cuts
      bool passedCaloTagQuality (const xAOD::Muon& mu) const;

      /// Returns true if the muon passed the tight working point cuts    
      bool passTight(const xAOD::Muon& mu, float rho, float oneOverPSig) const;
      /// @}


   private:

     MuonSelectionTool & operator=(const MuonSelectionTool &right);
     MuonSelectionTool( const MuonSelectionTool& toCopy );
     const std::string m_name;
      /// Maximum pseudorapidity for the selected muons
     double m_maxEta;
     /// xAOD::Muon::Quality m_quality;
     int  m_quality;
     bool m_isSimulation;
     
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
     
     std::string m_eventInfoContName;

     /// Checks for each histogram  
     StatusCode getHist( TFile* file, const char* histName, TH2D*& hist );
     // 
     std::string m_tightWP_rootFile;
     TH2D* m_tightWP_lowPt_rhoCuts;
     TH2D* m_tightWP_lowPt_qOverPCuts;
     TH2D* m_tightWP_mediumPt_rhoCuts;
     TH2D* m_tightWP_highPt_rhoCuts;

     // subfolder to load from the calibration db
     std::string m_calibration_version;
     // possible override for the calibration version
     std::string m_custom_dir;

     //Need run number (or random run number) to apply period-dependent selections.
     //If selection depends only on data taking year, this can be specified by passing
     //argument needOnlyCorrectYear=true, in which case the random run number decoration
     //from the pile-up reweighting tool is not needed.
     unsigned int getRunNumber(bool needOnlyCorrectYear = false) const;

     //TMVA readers for low-pT working point
     TMVA::Reader* readerE_MUID;
     TMVA::Reader* readerO_MUID;
     TMVA::Reader* readerE_MUGIRL;
     TMVA::Reader* readerO_MUGIRL;

     //TMVA initialize function
     void PrepareReader(TMVA::Reader* reader);

     //variables for the TMVA readers
     Float_t *lowPTmva_middleHoles, *lowPTmva_muonSeg1ChamberIdx, *lowPTmva_muonSeg2ChamberIdx, *lowPTmva_momentumBalanceSig,
       *lowPTmva_scatteringCurvatureSig, *lowPTmva_scatteringNeighbourSig, *lowPTmva_energyLoss, *lowPTmva_muonSegmentDeltaEta;

   }; // class MuonSelectionTool

} // namespace CP

#endif // CPTOOLTESTS_MUONSELECTIONTOOL_H
