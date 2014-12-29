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

// Local includes
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

// Root includes
#include "MuonValidationPlots.h"
#include "MuonTrackValidationPlots.h"
#include "MuonSegmentValidationPlots.h"
#include "TruthMuonPlots.h"
#include "RecoMuonPlots.h"
#include "RecoMuonTrackPlots.h"
#include "TruthRelatedMuonTrackPlots.h"
// Forward declaration

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
  
  void printMuonDebug(const xAOD::Muon* mu);
  void printTruthMuonDebug(const xAOD::TruthParticle* truthMu, const xAOD::Muon* mu);
  StatusCode bookValidationPlots(PlotBase& valPlots);
  const xAOD::Muon* findRecoMuon(const xAOD::TruthParticle* truthMu);
  const xAOD::MuonSegment* findRecoMuonSegment(const xAOD::MuonSegment* truthMuSeg);

  TH1F* findHistogram(std::vector<HistData> hists,std::string hnameTag,std::string hdirTag,std::string hNewName);

  // Containers
  std::string m_muonsName;
  std::string m_muonsTruthName;
  std::string m_muonTracksName;
  std::string m_muonSegmentsName;
  std::string m_muonSegmentsTruthName;

  // Configurable properties
  std::vector<unsigned int> m_selectMuonAuthors;
  std::vector<unsigned int> m_selectMuonCategories;  
  bool m_doMuonSegmentValidation;
  bool m_doMuonTrackValidation;
  bool m_doBinnedResolutionPlots;

  //
  enum MUCATEGORY{ALL=0, PROMPT, INFLIGHT, NONISO, REST};
  std::vector<std::string> m_selectMuonCategoriesStr;
  MuonPhysValMonitoringTool::MUCATEGORY getMuonSegmentTruthCategory(const xAOD::MuonSegment* truthMuSeg, const xAOD::TruthParticleContainer* muonTruthContainer);
  MuonPhysValMonitoringTool::MUCATEGORY getMuonTruthCategory(const xAOD::IParticle* prt);

  // Hists
  std::vector<MuonValidationPlots*> m_muonValidationPlots;
  std::vector<MuonTrackValidationPlots*> m_muonTrackValidationPlots;
  std::vector<MuonSegmentValidationPlots*> m_muonSegmentValidationPlots;
  RecoMuonPlots*       m_oUnmatchedRecoMuonPlots;
  TruthMuonPlots*      m_oUnmatchedTruthMuonPlots;
  RecoMuonTrackPlots*  m_oUnmatchedRecoMuonTrackPlots;
  Muon::MuonSegmentPlots* m_oUnmatchedRecoMuonSegmentPlots;

  //overview hists
  TH1F* h_overview_reco_category;
  std::vector<TH1F*> h_overview_reco_authors;

  std::vector<const xAOD::Muon*> m_vMatchedMuons;
  std::vector<const xAOD::TrackParticle*> m_vMatchedMuonTracks;
  std::vector<const xAOD::MuonSegment*> m_vMatchedMuonSegments;
}; 




// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
}

#endif //> !MUONPHYSVALMONITORING_MUONPHYSVALMONITORINGTOOL_H
