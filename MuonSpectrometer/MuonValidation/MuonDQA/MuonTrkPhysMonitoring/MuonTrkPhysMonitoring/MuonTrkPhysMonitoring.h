/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_MUONTRKPHYSMONITORING_H
#define MUONTRKPHYSMONITORING_MUONTRKPHYSMONITORING_H

// // *****************************************************************************
// // MuonTrkPhysMonitoring.cxx
// // AUTHORS: Nektarios Chr.Benekos, Austin Basye, Christina G. Skarpathiotaki
// // *****************************************************************************

// #include <vector>
// #include <iostream>

// #include "GaudiKernel/ToolHandle.h"

// // Athena Monitoring (Base Class Def)
// #include "AthenaMonitoring/ManagedMonitorToolBase.h"

// // Generic Tracking and 4Momentum Interfaces
// #include "GeoPrimitives/GeoPrimitives.h"
// #include "EventPrimitives/EventPrimitives.h"
// #include "EventPrimitives/EventPrimitivesHelpers.h"
// #include "FourMomUtils/P4Helpers.h"

// // muons
// #include "MuonRecHelperTools/MuonEDMHelperTool.h"
// #include "MuonIdHelpers/MuonIdHelperTool.h"
// #include "MuonRecToolInterfaces/IMuonHitSummaryTool.h"
// #include "TrkToolInterfaces/IResidualPullCalculator.h"

// // MCP
// #include "MuonSelectorTools/IMuonSelectionTool.h" 
// #include "TrkToolInterfaces/ITrackSelectorTool.h"
// #include "MuonResonanceTools/IMuonResonanceSelectionTool.h"
// #include "MuonResonanceTools/IMuonResonancePairingTool.h"

// // xAODs
// #include "xAODMuon/MuonContainer.h"
// #include "xAODMuon/Muon.h"
// #include "xAODMuon/MuonSegment.h"
// #include "xAODMuon/MuonSegmentContainer.h"
// #include "xAODEventInfo/EventInfo.h"

// #include "TH1F.h"
// #include "TH2F.h"
// #include "TProfile.h"
// #include "TMath.h"
// #include "TLorentzVector.h"

// #include "MuonTrkPhysMonUtils.h"
// #include "MuMuSignalPlots.h"

// using namespace MuonTrkPhysMonUtils;

// // Begin MuonTrkPhysMonitoring Class Desciption
// //---------------------------------------------------------------------------------------
// //   The MuonTrkPhysMonitoring class is the result of simultaneous efforts to both create
// // a High-Level muon monitoring package, and to unify the MCP and Muon monitoring packages
// // as a result, this package grew out of a prototype for the first effort to encapsulate
// // the latter. Thus, this software is quite bifurcated, and efforts are ongoing to stream-
// // line the following class defintion.
// //---------------------------------------------------------------------------------------
// class MuonTrkPhysMonitoring : public ManagedMonitorToolBase
// {

//  public:

//   /** Conscructor */
//   MuonTrkPhysMonitoring( const std::string & type, const std::string & name, const IInterface* parent );
  
//   /** Descructor */
//   virtual ~MuonTrkPhysMonitoring();

//   /** Global Intialization Method */
//   StatusCode initialize();

//   /** Global Histogram Booking and Registration Method */
//   // We will call the Monitoring Subgroup Book Methods from here
//   //---------------------------------------------------------------------------------------
//   //virtual StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
  
//   virtual StatusCode bookHistograms();
    
//   /** Global Filling Method */
//   // We will call the Monitoring Subgroup Fill Methods from here
//   virtual StatusCode fillHistograms();
    
//   /** Global Proccessing Method */
//   // We will call the Monitoring Subgroup Process Methods from here
    
//   virtual StatusCode procHistograms();
//   //---------------------------------------------------------------------------------------
    
//   /** Global Finalization Method */
//   StatusCode finalize();
    
//   /** fillLumiBlock [??] */
//   StatusCode fillLumiBlock();

//   TH1* m_hNEvent;

//  protected:
    
//   // The Following Protected Methods provide functionality for each of our
//   //   Monitoring Sub-Groups. (The Directories of the Web Display)
//   //
//   // Each Group Is defined in the file with the named suffix from the Method Name
//   //   For example: bookHistograms_CbMSIDAlign is defined in MTPM_CbMSIDAlign
//   //---------------------------------------------------------------------------------------

//   // X->MuMu tag-and-probe Signal Monitoring
//   void fillHistograms_MuMuSignal(std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > mumucandidates, std::vector<MuMuSignalPlots*> plots);
//   void fillTagAndProbe(const xAOD::Muon* tag, const xAOD::Muon* probe, std::vector<MuMuSignalPlots*> plots);
//   void fillQuality(const xAOD::Muon* mu_plus, const xAOD::Muon* mu_minus, std::vector<MuMuSignalPlots*> plots);
//   void fillAuthor(const xAOD::Muon* mu_plus, const xAOD::Muon* mu_minus, std::vector<MuMuSignalPlots*> plots);

//  private:

//   // Athena Bookkeeping
//   //___________________________
//   /* Gaudi message stream - msgstream as private member (-> speed) */
//   mutable MsgStream      m_log;

//   //---------------------------------------------------------------------------------------
//   // Miscellaneous Helper Methods
//   //---------------------------------------------------------------------------------------
    
//   // Tool Initialization

//   // General functions for handling containers and analysis objects
//   bool handleMuonContainer();

//   // General Helpers
//   bool passTrigger();
//   bool passPtCut( float pt );

//   // GenTracking Monitoring
//   int retrieveLumiBlock();

//   // Data Identification
//   //___________________________
//   /* Trigger Aware Members */
//   bool m_useTrigger;
//   std::vector<std::string> m_muon_triggers;
//   std::string m_MuonTriggerChainName; //[??] <- Does MTPM-Class really need this?
        
//   // User Requested Data Container Names

//   std::string m_InnerTrackContainer;
//   std::string m_MuonCollectionName;
        
//   // ATLAS Detector Description
        
//   /** ToolHandle idHelper*/
//   ToolHandle<Muon::MuonIdHelperTool>    m_idHelperTool;
//   ToolHandle<Muon::MuonEDMHelperTool>   m_helperTool;
//   ToolHandle<Muon::IMuonHitSummaryTool> m_muonHitSummaryTool;

//   //<! tool to calculate residuals and pulls
//   ToolHandle<Trk::IResidualPullCalculator> m_pullCalculator;
    
//   // Selection Criteria Tools
//   float m_GlobalPtCut_pp;
//   float m_GlobalPtCut_cosmic;
//   float m_GlobalPtCut_heavyIon;
//   float m_GlobalResidualError;
    
//   // Trigger Matching for Tag Muon
//   bool        m_doZTrigMatch;
//   std::string m_tagMuonTrigger;
    
//   double m_cut_diMuonMass;

//   // what is this?
//   int m_lumiblock;
//   int m_eventCounter; //[??] Currently used to swap ZSignal tnp charge
  
//   std::vector<MuMuSignalPlots*> m_oZSignalPlots;
//   std::vector<MuMuSignalPlots*> m_oJpsiSignalPlots;

//   // MCP T&P helpers
//   ToolHandle<IMuonResonanceSelectionTool> m_ZmumuResonanceSelectionTool;
//   ToolHandle<IMuonResonancePairingTool>   m_ZmumuResonancePairingTool;
//   ToolHandle<IMuonResonanceSelectionTool> m_JpsimumuResonanceSelectionTool;
//   ToolHandle<IMuonResonancePairingTool>   m_JpsimumuResonancePairingTool;

//   // isMC required by MCP tools. 
//   // only matters for scalefactors, i.e., keep isMC=false for SF=1.
//   bool m_isMC;

//   StatusCode setupTools();
//   StatusCode bookInMongroup(TH1* hist, MonGroup& mongroup);
//   StatusCode bookInMongroup(HistData& hist, MonGroup& mongroup);
//   StatusCode bookInMongroup(PlotBase& valPlots, MonGroup& mongroup); 
//   StatusCode bookInMongroup_Segment(PlotBase& valPlots, MonGroup& mongroup); 

//   std::string pathToHistName(std::string str){
//     std::replace( str.begin(), str.end(), '/', '_');
//     return str;
//   }

// uint8_t RetrieveHitInfo(const xAOD::Muon* muon,const xAOD::MuonSummaryType& info, const uint8_t defaultVal = 0){
//   uint8_t val = defaultVal;
//   if (!muon->summaryValue(val,info)) val = defaultVal;
//   return val;
// }

// uint8_t RetrieveHitInfo(const xAOD::TrackParticle* trkprt,const xAOD::SummaryType& info, const uint8_t defaultVal = 0){
//   uint8_t val = defaultVal;
//   if (!trkprt->summaryValue(val,info)) val = defaultVal;
//   return val;
// }

// };

#endif
