/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// MuonPhysValMonitoringTool.cxx 
// Implementation file for class MuonPhysValMonitoringTool
// Author: Felix Socher <Felix.Socher@cern.ch>
/////////////////////////////////////////////////////////////////// 

#ifndef MUONPHYSVALMONITORING_MUONPHYSVALMONITORINGTOOL_H
#define MUONPHYSVALMONITORING_MUONPHYSVALMONITORINGTOOL_H

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODMuon/SlowMuon.h"
#include "xAODEventInfo/EventInfo.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"
#include "MuonCombinedToolInterfaces/IMuonPrintingTool.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "MuonValidationPlots.h"
#include "TriggerMuonValidationPlots.h"
#include "MuonTrackValidationPlots.h"
#include "MuonSegmentValidationPlots.h"
#include "SlowMuonValidationPlots.h"

#include <string>
#include <vector>

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
  virtual ~MuonPhysValMonitoringTool()=default;

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

  enum MUCATEGORY{ALL=0, PROMPT, INFLIGHT, NONISO, REST};

  void handleMuon(const xAOD::Muon* mu,const xAOD::SlowMuon *smu=0, float weight=1.0);
  void handleSlowMuon(const xAOD::SlowMuon *smu, float weight=1.0);
  void handleTruthMuon(const xAOD::TruthParticle* truthMu,float weight=1.0);
  void handleMuonTrack(const xAOD::TrackParticle* tp, xAOD::Muon::TrackParticleType type, float weight=1.0);
  void handleMuonSegment(const xAOD::MuonSegment* muSeg, float weight=1.0);
  void handleTruthMuonSegment(const xAOD::MuonSegment* truthMuSeg, const xAOD::TruthParticleContainer* muonTruthContainer, float weight=1.0);
  
  void handleMuonTrees(const xAOD::EventInfo* eventInfo, bool isData);

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
    
  const xAOD::TrackParticleContainer* m_MSTracks;
  std::map<std::string,int> m_counterBits;
  std::vector<std::string> m_muonItems;
  std::vector<std::string> m_L1Seed;
  int m_SelectedAuthor;

  TH1F* findHistogram(const std::vector<HistData>& hists,const std::string& hnameTag,const std::string& hdirTag,const std::string& hNewName);
  void modifyHistogram(TH1* hist);

  Gaudi::Property<std::string> m_tracksName{this,"TrackContainerName","InDetTrackParticles"};
  Gaudi::Property<std::string> m_fwdtracksName{this,"FwdTrackContainerName",""};
  Gaudi::Property<std::string> m_muonsName{this,"MuonContainerName","Muons"};
  Gaudi::Property<std::string> m_slowMuonsName{this,"SlowMuonContainerName","SlowMuons"};
  Gaudi::Property<std::string> m_muonsTruthName{this,"MuonTruthParticleContainerName","MuonTruthParticles"};
  Gaudi::Property<std::string> m_muonTracksName{this,"MuonTrackContainerName","MuonSpectrometerTrackParticles"};
  Gaudi::Property<std::string> m_muonExtrapolatedTracksName{this,"MuonExtrapolatedTrackContainerName","ExtrapolatedMuonTrackParticles"};
  Gaudi::Property<std::string> m_muonMSOnlyExtrapolatedTracksName{this,"MuonOnlyExtrapolatedTrackContainerName","MSOnlyExtrapolatedMuonTrackParticles"};
  Gaudi::Property<std::string> m_muonSegmentsName{this,"MuonSegmentContainerName","MuonSegments"};
  Gaudi::Property<std::string> m_muonSegmentsTruthName{this,"MuonTruthSegmentContainerName","MuonTruthSegments"};
  Gaudi::Property<std::string> m_muonL1TrigName{this,"L1TrigMuonContainerName","LVL1MuonRoIs"};
  Gaudi::Property<std::string> m_muonL2SAName{this,"L2SAMuonContainerName","HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfo"};
  Gaudi::Property<std::string> m_muonL2CBName{this,"L2CBMuonContainerName","HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfo"};
  Gaudi::Property<std::string> m_muonEFCombTrigName{this,"EFCombTrigMuonContainerName","HLT_xAOD__MuonContainer_MuonEFInfo"};

  Gaudi::Property<std::vector<int>> m_selectMuonWPs{this,"SelectMuonWorkingPoints",{xAOD::Muon::Loose,xAOD::Muon::Medium}};
  Gaudi::Property<std::vector<unsigned int>> m_selectMuonAuthors{this,"SelectMuonAuthors",{xAOD::Muon::MuidCo,xAOD::Muon::MuTagIMO,xAOD::Muon::MuidSA,xAOD::Muon::MuGirl,xAOD::Muon::CaloTag,xAOD::Muon::CaloLikelihood}};
  Gaudi::Property<std::vector<std::vector<std::string>>> m_selectHLTMuonItems{this,"SelectHLTMuonItems",{}};
  Gaudi::Property<std::vector<std::string>> m_L1MuonItems{this,"SelectL1MuonItems",{}};
  Gaudi::Property<std::vector<unsigned int>> m_selectMuonCategories{this,"SelectMuonCategories",{MUCATEGORY::ALL,MUCATEGORY::PROMPT,MUCATEGORY::INFLIGHT,MUCATEGORY::NONISO,MUCATEGORY::REST}};

  Gaudi::Property<bool> m_doBinnedResolutionPlots{this,"DoBinnedResolutionPlots",true};
  Gaudi::Property<bool> m_doTrigMuonValidation{this,"DoTrigMuonValidation",false};
  Gaudi::Property<bool> m_doTrigMuonL1Validation{this,"DoTrigMuonL1Validation",false};
  Gaudi::Property<bool> m_doTrigMuonL2Validation{this,"DoTrigMuonL2Validation",false};
  Gaudi::Property<bool> m_doTrigMuonEFValidation{this,"DoTrigMuonEFValidation",false};
  Gaudi::Property<bool> m_doMuonTree{this,"DoMuonTree",false};
  Gaudi::Property<bool> m_isData{this,"IsData",false};

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this,"EventInfo","EventInfo","event info"};
  
  // Tools
  ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
  ToolHandle<Rec::IMuonPrintingTool> m_muonPrinter;
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;
  ToolHandle<Trk::ITrackSelectorTool> m_trackSelector;
  ToolHandle<CP::IIsolationSelectionTool> m_isoTool{this,"IsoTool",""};
 
  
  std::vector<std::string> m_selectMuonCategoriesStr;
  MuonPhysValMonitoringTool::MUCATEGORY getMuonSegmentTruthCategory(const xAOD::MuonSegment* truthMuSeg, const xAOD::TruthParticleContainer* muonTruthContainer);
  MuonPhysValMonitoringTool::MUCATEGORY getMuonTruthCategory(const xAOD::IParticle* prt);
  MuonPhysValMonitoringTool::MUCATEGORY getMuonTruthCategory(const xAOD::Muon* prt);
  bool passesAcceptanceCuts(const xAOD::IParticle* prt);
  float deltaR(const xAOD::IParticle* prt1, const xAOD::IParticle* prt2);
  void SplitString(TString x, const TString& delim, std::vector<TString> &v);
  
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
  std::vector<TH1F*> m_h_overview_nObjects;
  TH1F* m_h_overview_reco_category;
  std::vector<TH1F*> m_h_overview_reco_authors;

  TH1F *m_h_overview_Z_mass;
  TH1F *m_h_overview_Z_mass_ME;
  TH1F *m_h_overview_Z_mass_ID;

  std::vector<const xAOD::TruthParticle*> m_vMatchedTruthMuons;
  std::vector<const xAOD::Muon*> m_vMatchedMuons;
  std::vector<const xAOD::SlowMuon*> m_vMatchedSlowMuons;
  std::vector<const xAOD::TrackParticle*> m_vMatchedMuonTracks;
  std::vector<const xAOD::MuonSegment*> m_vMatchedMuonSegments;
  std::vector<const xAOD::TrackParticle*> m_vZmumuIDTracks;
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
