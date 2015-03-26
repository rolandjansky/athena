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

// Tools
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

// Local includes
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

// Root includes
#include "MuonValidationPlots.h"
#include "TriggerMuonValidationPlots.h"
#include "MuonTrackValidationPlots.h"
#include "MuonSegmentValidationPlots.h"
#include "TruthMuonPlots.h"
#include "RecoMuonPlots.h"
#include "RecoMuonTrackPlots.h"
#include "TruthRelatedMuonTrackPlots.h"
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
  
  void handleMuon(const xAOD::Muon* mu);
  void handleTruthMuon(const xAOD::TruthParticle* truthMu);
  void handleMuonTrack(const xAOD::TrackParticle* muTP);
  void handleMuonSegment(const xAOD::MuonSegment* muSeg);
  void handleTruthMuonSegment(const xAOD::MuonSegment* truthMuSeg, const xAOD::TruthParticleContainer* muonTruthContainer);
  void handleMuonTrigger(  std::vector<const xAOD::Muon*> m_vEFMuonsSelected, std::vector<const xAOD::Muon*> m_vRecoMuons);
  void handleMuonL1Trigger(const xAOD::MuonRoI* TrigL1mu);
  void handleMuonL2Trigger(const xAOD::L2StandAloneMuon* L2SAmu);
  void handleMuonL2Trigger(const xAOD::L2CombinedMuon* L2CBmu);
  
  void printMuonDebug(const xAOD::Muon* mu);
  void printMuonL1TriggerDebug(const xAOD::MuonRoI* TrigL1mu);
  void printTruthMuonDebug(const xAOD::TruthParticle* truthMu, const xAOD::Muon* mu);
  
  StatusCode bookValidationPlots(PlotBase& valPlots);
  const xAOD::Muon* findRecoMuon(const xAOD::TruthParticle* truthMu);
  const xAOD::MuonSegment* findRecoMuonSegment(const xAOD::MuonSegment* truthMuSeg);
  xAOD::Muon* getCorrectedMuon(const xAOD::Muon &mu);
  
  const xAOD::TrackParticleContainer* MSTracks;
  
  TH1F* findHistogram(std::vector<HistData> hists,std::string hnameTag,std::string hdirTag,std::string hNewName);
  void modifyHistogram(TH1* hist);

  bool m_isData;




  // Containers
  std::string m_muonsName;
  std::string m_muonsTruthName;
  std::string m_muonTracksName;
  std::string m_muonSegmentsName;
  std::string m_muonSegmentsTruthName;
  std::string m_muonL1TrigName;
  std::string m_muonL2SAName;
  std::string m_muonL2CBName;
  std::string m_muonEFCombTrigName;
  std::string m_trigDecisionKey;
  std::vector<std::string> m_muonItems;

  // Configurable properties
  std::map<std::string,int> m_counterBits;
  std::vector<unsigned int> m_selectMuonAuthors;
  std::vector<unsigned int> m_selectMuonCategories;  
  bool m_doMuonSegmentValidation;
  bool m_doMuonTrackValidation;
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

 
  enum MUCATEGORY{ALL=0, PROMPT, INFLIGHT, NONISO, REST};
  std::vector<std::string> m_selectMuonCategoriesStr;
  MuonPhysValMonitoringTool::MUCATEGORY getMuonSegmentTruthCategory(const xAOD::MuonSegment* truthMuSeg, const xAOD::TruthParticleContainer* muonTruthContainer);
  MuonPhysValMonitoringTool::MUCATEGORY getMuonTruthCategory(const xAOD::IParticle* prt);

  // Hists
  std::vector<MuonValidationPlots*> m_muonValidationPlots;
  std::vector<TriggerMuonValidationPlots*> m_TriggerMuonValidationPlots;
  std::vector<MuonTrackValidationPlots*> m_muonTrackValidationPlots;
  std::vector<MuonSegmentValidationPlots*> m_muonSegmentValidationPlots;
  RecoMuonPlots*       m_oUnmatchedRecoMuonPlots;
  TruthMuonPlots*      m_oUnmatchedTruthMuonPlots;
  RecoMuonTrackPlots*  m_oUnmatchedRecoMuonTrackPlots;
  Muon::MuonSegmentPlots* m_oUnmatchedRecoMuonSegmentPlots;

  //overview hists
  std::vector<TH1F*> h_overview_nObjects;
  TH1F* h_overview_reco_category;
  std::vector<TH1F*> h_overview_reco_authors;

  std::vector<const xAOD::Muon*> m_vMatchedMuons;
  std::vector<const xAOD::TrackParticle*> m_vMatchedMuonTracks;
  std::vector<const xAOD::MuonSegment*> m_vMatchedMuonSegments;
  std::vector<const xAOD::Muon*> m_vEFMuons;
  std::vector<const xAOD::Muon*> m_vEFMuonsSelected;
  std::vector<const xAOD::L2StandAloneMuon*> m_vL2SAMuons;
  std::vector<const xAOD::L2StandAloneMuon*> m_vL2SAMuonsSelected;
  std::vector<const xAOD::L2CombinedMuon*> m_vL2CBMuons;
  std::vector<const xAOD::L2CombinedMuon*> m_vL2CBMuonsSelected;
  std::vector<const xAOD::Muon*> m_vRecoMuons;


}; 




// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
}

#endif //> !MUONPHYSVALMONITORING_MUONPHYSVALMONITORINGTOOL_H
