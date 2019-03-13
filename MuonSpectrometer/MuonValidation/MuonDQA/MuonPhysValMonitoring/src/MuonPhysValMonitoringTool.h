///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonPhysValMonitoringTool.cxx 
// Implementation file for class MuonPhysValMonitoringTool
// Author: Felix Socher <Felix.Socher@cern.ch>
/////////////////////////////////////////////////////////////////// 

#ifndef MUONPHYSVALMONITORING_MUONPHYSVALMONITORINGTOOL_H
#define MUONPHYSVALMONITORING_MUONPHYSVALMONITORINGTOOL_H

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuon.h"

#include "xAODMuon/SlowMuon.h"

// Tools
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"



// Local includes
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

// Root includes
#include "MuonValidationPlots.h"
#include "TriggerMuonValidationPlots.h"
#include "MuonTrackValidationPlots.h"
#include "MuonSegmentValidationPlots.h"
#include "SlowMuonValidationPlots.h"

// Forward declaration
namespace Rec {
  class IMuonPrintingTool;
}
namespace MuonPhysValMonitoring {

class MuonPhysValMonitoringTool
  : public ManagedMonitorToolBase
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  MuonPhysValMonitoringTool( const std::string& type,
		  const std::string& name, 
		  const IInterface* parent );

  /// Destructor: 
  virtual ~MuonPhysValMonitoringTool(); 

  // Athena algtool's Hooks
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 


  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  MuonPhysValMonitoringTool();

  void handleMuon(const xAOD::Muon* mu,const xAOD::SlowMuon *smu=0);
  void handleSlowMuon(const xAOD::SlowMuon *smu);
  void handleTruthMuon(const xAOD::TruthParticle* truthMu);
  void handleMuonTrack(const xAOD::TrackParticle* tp, xAOD::Muon::TrackParticleType type);
  void handleMuonSegment(const xAOD::MuonSegment* muSeg);
  void handleTruthMuonSegment(const xAOD::MuonSegment* truthMuSeg, const xAOD::TruthParticleContainer* muonTruthContainer);

  void handleMuonL1Trigger(const xAOD::MuonRoI* TrigL1mu);
  void handleMuonL2Trigger(const xAOD::L2StandAloneMuon* L2SAMu);
  void handleMuonL2Trigger(const xAOD::L2CombinedMuon* L2CBMu);
  void handleMuonTrigger(const xAOD::Muon* mu);
  void L2SATriggerResolution();
  void L2CBTriggerResolution();
  void EFTriggerResolution();
  
  void printMuonDebug(const xAOD::Muon* mu);
  void printTruthMuonDebug(const xAOD::TruthParticle* truthMu, const xAOD::Muon* mu);
  
  StatusCode bookValidationPlots(PlotBase& valPlots);
  const xAOD::Muon* findRecoMuon(const xAOD::TruthParticle* truthMu);
  const xAOD::SlowMuon* findRecoSlowMuon(const xAOD::TruthParticle* truthMu);
  const xAOD::MuonSegment* findRecoMuonSegment(const xAOD::MuonSegment* truthMuSeg);
  xAOD::Muon* getCorrectedMuon(const xAOD::Muon &mu);
    
  const xAOD::TrackParticleContainer* MSTracks;
  
  TH1F* findHistogram(std::vector<HistData> hists,std::string hnameTag,std::string hdirTag,std::string hNewName);
  void modifyHistogram(TH1* hist);

  bool m_isData;




  // Containers
  std::string m_tracksName;
  std::string m_fwdtracksName;
  std::string m_muonsName;
  std::string m_slowMuonsName;
  std::string m_muonsTruthName;
  std::string m_muonTracksName;
  std::string m_muonExtrapolatedTracksName;
  std::string m_muonMSOnlyExtrapolatedTracksName;
  std::string m_muonSegmentsName;
  std::string m_muonSegmentsTruthName;
  std::string m_muonL1TrigName;
  std::string m_muonL2SAName;
  std::string m_muonL2CBName;
  std::string m_muonEFCombTrigName;


  // Configurable properties
  std::map<std::string,int> m_counterBits;
  std::vector<int> m_selectMuonWPs;
  std::vector<unsigned int> m_selectMuonAuthors;
  std::vector<std::vector<std::string>> m_selectHLTMuonItems;
  std::vector<std::string> m_muonItems;
  std::vector<std::string> m_L1Seed;
  std::vector<std::string> m_L1MuonItems;
  int SelectedAuthor;
  std::vector<unsigned int> m_selectMuonCategories;  
  bool m_doBinnedResolutionPlots;
  bool m_doTrigMuonValidation;
  bool m_doTrigMuonL1Validation;
  bool m_doTrigMuonL2Validation;
  bool m_doTrigMuonEFValidation;
  bool m_doMuonTree;


  
  // Tools
  ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
  ToolHandle<Rec::IMuonPrintingTool> m_muonPrinter;
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;
  ToolHandle<Trk::ITrackSelectorTool> m_trackSelector;
  ToolHandle<CP::IIsolationSelectionTool> m_isoTool;


 
  enum MUCATEGORY{ALL=0, PROMPT, INFLIGHT, NONISO, REST};
  std::vector<std::string> m_selectMuonCategoriesStr;
  MuonPhysValMonitoringTool::MUCATEGORY getMuonSegmentTruthCategory(const xAOD::MuonSegment* truthMuSeg, const xAOD::TruthParticleContainer* muonTruthContainer);
  MuonPhysValMonitoringTool::MUCATEGORY getMuonTruthCategory(const xAOD::IParticle* prt);
  MuonPhysValMonitoringTool::MUCATEGORY getMuonTruthCategory(const xAOD::Muon* prt);
  bool passesAcceptanceCuts(const xAOD::IParticle* prt);
  float deltaR(const xAOD::IParticle* prt1, const xAOD::IParticle* prt2);
  void SplitString(TString x, TString delim, std::vector<TString> &v);
  
  // Hists
  std::vector<MuonValidationPlots*> m_muonValidationPlots;
  std::vector<TriggerMuonValidationPlots*> m_TriggerMuonValidationPlots;
  std::vector<MuonTrackValidationPlots*> m_muonMSTrackValidationPlots;
  std::vector<MuonTrackValidationPlots*> m_muonMETrackValidationPlots;
  std::vector<MuonTrackValidationPlots*> m_muonMSOnlyMETrackValidationPlots;
  std::vector<MuonTrackValidationPlots*> m_muonIDTrackValidationPlots;
  std::vector<MuonTrackValidationPlots*> m_muonIDSelectedTrackValidationPlots;
  std::vector<MuonTrackValidationPlots*> m_muonIDForwardTrackValidationPlots;
  std::vector<MuonSegmentValidationPlots*> m_muonSegmentValidationPlots;
  Muon::RecoMuonPlotOrganizer*       m_oUnmatchedRecoMuonPlots;
  Muon::TruthMuonPlotOrganizer*      m_oUnmatchedTruthMuonPlots;
  Muon::RecoMuonTrackPlotOrganizer*  m_oUnmatchedRecoMuonTrackPlots;
  Muon::MuonSegmentPlots* m_oUnmatchedRecoMuonSegmentPlots;

  std::vector<SlowMuonValidationPlots*> m_slowMuonValidationPlots;
  
  //overview hists
  std::vector<TH1F*> h_overview_nObjects;
  TH1F* h_overview_reco_category;
  std::vector<TH1F*> h_overview_reco_authors;

  TH1F *h_overview_Z_mass;
  TH1F *h_overview_Z_mass_ME;
  TH1F *h_overview_Z_mass_ID;

  std::vector<const xAOD::TruthParticle*> m_vMatchedTruthMuons;
  std::vector<const xAOD::Muon*> m_vMatchedMuons;
  std::vector<const xAOD::SlowMuon*> m_vMatchedSlowMuons;
  std::vector<const xAOD::TrackParticle*> m_vMatchedMuonTracks;
  std::vector<const xAOD::MuonSegment*> m_vMatchedMuonSegments;
  std::vector<const xAOD::TrackParticle*> m_vZmumuIDTracks;
  std::vector<const xAOD::TrackParticle*> m_vZmumuMSTracks;
  std::vector<const xAOD::TrackParticle*> m_vZmumuMETracks;
  std::vector<const xAOD::Muon*> m_vZmumuMuons;
  std::vector<const xAOD::Muon*> m_vEFMuons;
  std::vector<const xAOD::Muon*> m_vEFMuonsSelected;
  std::vector<const xAOD::L2StandAloneMuon*> m_vL2SAMuons;
  std::vector<const xAOD::L2StandAloneMuon*> m_vL2SAMuonsSelected;
  std::vector<const xAOD::L2CombinedMuon*> m_vL2CBMuons;
  std::vector<const xAOD::L2CombinedMuon*> m_vL2CBMuonsSelected;
  std::vector<const xAOD::Muon*> m_vRecoMuons;
  std::vector<const xAOD::Muon*> m_vRecoMuons_EffDen_CB;
  std::vector<const xAOD::Muon*> m_vRecoMuons_EffDen_MS;
  std::vector<const xAOD::Muon*> m_vRecoMuons_EffDen;


  template<class T>
  const T* getContainer( const std::string & containerName);
};
  
  template<class T>
  const T* MuonPhysValMonitoringTool::getContainer(const std::string & containerName){
    const T * ptr = evtStore()->retrieve< const T >( containerName );
    if (!ptr) {
      ATH_MSG_ERROR("Container '"<<containerName<<"' could not be retrieved");
    }
    return ptr;
  }
 




// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
}

#endif //> !MUONPHYSVALMONITORING_MUONPHYSVALMONITORINGTOOL_H
