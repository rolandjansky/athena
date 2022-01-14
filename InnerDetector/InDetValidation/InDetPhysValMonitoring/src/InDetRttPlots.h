/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETRTTPLOTS
#define INDETPHYSVALMONITORING_INDETRTTPLOTS
/**
 * @file InDetRttPlots.h
 * @author shaun roe
 * @date 29/3/2014
 **/


// std includes
#include <string>
#include <memory>

// local includes
#include "InDetPlotBase.h"
#include "InDetBasicPlot.h"

#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TrkValHistUtils/TruthInfoPlots.h"
#include "InDetPerfPlot_nTracks.h"
#include "InDetPerfPlot_FakeRate.h"
#include "InDetPerfPlot_Efficiency.h"
#include "InDetPerfPlot_HitResidual.h"
#include "InDetPerfPlot_HitEfficiency.h"
#include "InDetPerfPlot_TrackParameters.h"
#include "TrkValHistUtils/IDHitPlots.h"
#include "InDetPerfPlot_Hits.h"
#include "InDetPerfPlot_Vertex.h"
#include "InDetPerfPlot_VertexTruthMatching.h"
#include "InDetPerfPlot_VerticesVsMu.h"
#include "InDetPerfPlot_TrkInJet.h"
#include "InDetPerfPlot_TRTExtension.h"
#include "InDetPerfPlot_ANTracking.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include "InDetPerfPlot_Resolution.h"
#include "InDetPerfNtuple_TruthToReco.h" 

#include "InDetTrackSystematicsTools/InDetTrackTruthOriginDefs.h"


/// helper struct - steer the configuration from the parent tool's side 
struct InDetRttPlotConfig{
  /// Plots for (selected) tracks, not necessarily truth matched
  bool doTrackParameters{true}; 
  bool doNTracks{true}; 
  bool doHitResidualPlot{false}; 
  bool doHitEffPlot{false}; 
  bool doHitsRecoTracksPlots{true}; 
  bool doTrtExtensionPlots{true}; 

  /// Fake plots (and unlinked) 
  bool doFakePlots{true}; 
  bool doMissingTruthFakePlots{true}; 
  bool doHitsFakeTracksPlots{true};
  bool doHitsUnlinkedTracksPlots{true};

  /// Efficiency plots - require truth, optionally
  /// matching reco 
  bool doEffPlots{true}; 

  /// Resolution and "matched track" plots - 
  /// filled if both reco and truth exist
  bool doResolutionPlotPrim{true}; 
  bool doResolutionPlotPrim_truthFromB{false}; 
  bool doResolutionPlotSecd{false};
  bool doHitsMatchedTracksPlots{true};

  /// Vertexing plots - no truth requirement 
  bool doVertexPlots{true}; 
  bool doVerticesVsMuPlots{true}; 
  bool doHardScatterVertexPlots{true}; 

  /// Vertexing plots - truth requirement 
  bool doVertexTruthMatchingPlots{true};
  bool doHardScatterVertexTruthMatchingPlots{true}; 

  /// ANT (also known as LRT) tracking.
  /// Off by default as only used for merged containers
  bool doAnTrackingPlots{false}; 

  /// Plots for tracks in jets 
  bool doTrkInJetPlots{false}; 
  bool doTrkInJetPlots_bjets{false}; 
  bool doTrkInJetPlots_matched{false}; 
  bool doTrkInJetPlots_matched_bjets{false}; 
  bool doTrkInJetPlots_fake{false}; 
  bool doTrkInJetPlots_fake_bjets{false}; 
  bool doTrkInJetPlots_unlinked{false}; 
  bool doTrkInJetPlots_unlinked_bjets{false}; 
  bool doTrkInJetPlots_truthFromB{false}; 

  /// per author plots 
  bool doEfficienciesPerAuthor{false}; 
  bool doFakesPerAuthor{false};
  bool doTrackParametersPerAuthor{false};
  bool doResolutionsPerAuthor{false};
  
  /// Ntuple functionality 
  bool doNtupleTruthToReco{false}; 

  /// detail level (kept for compatibility)
  int detailLevel{10};
};

///class holding all plots for Inner Detector RTT Validation and implementing fill methods
class InDetRttPlots: public InDetPlotBase {
public:
  InDetRttPlots(InDetPlotBase* pParent, const std::string& dirName, const InDetRttPlotConfig & config);

  ///fill for things needing truth and track only
  void fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle, bool truthIsFromB=false, float mu = 0.0, float weight=1.0);

  ///fill for things needing track only
  void fill(const xAOD::TrackParticle& particle, float weight=1.0);
  void fill(const xAOD::TrackParticle& particle, const float mu, const unsigned int nVtx, float weight=1.0); //mu dependent plots
  void fill(const unsigned int nTrkANT, const unsigned int nTrkSTD, const unsigned int nTrkBAT, const float mu, const unsigned int nVtx,const float weight=1.0);
  void fill(const unsigned int nTracks, const unsigned int mu, const unsigned nVtx, const float weight=1.0);
  ///fill for things needing truth only
  void fill(const xAOD::TruthParticle& particle, float weight);
  ///Fill for efficiency plots
  void fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::TrackParticle* track, const bool isGood, const float mu, const unsigned int nVtx, float weight);

  ///fill for things needing all truth - not just the ones from the reco tracks
  
  ///fill reco-vertex related plots
  void fill(const xAOD::VertexContainer& vertexContainer, const xAOD::Vertex* recoHardScatter, const std::vector<const xAOD::TruthVertex*>& truthHSVertices, const std::vector<const xAOD::TruthVertex*>& truthPUVertices, float weight=1.0); 
  ///fill reco-vertex related plots that need EventInfo
  void fill(const xAOD::VertexContainer& vertexContainer, const unsigned int nPU, const float weight=1.0);

  void fill(const xAOD::TrackParticle& track, const xAOD::Jet& jet, bool isBjet=false, bool isFake=false, bool isUnlinked=false, bool truthIsFromB=false, const float weight=1.0);
  void fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::Jet& jet, const bool isGood, bool isBjet=false, bool truthIsFromB=false, float weight=1.0);
  void fillFakeRate(const xAOD::TrackParticle& track, const xAOD::Jet& jet, const bool isFake, bool isBjet=false, bool truthIsFromB=false, float weight=1.0);
  
  ///fill for Counters
  void fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory counter, float weight);
  ///fill for fakes
  void fillFakeRate(const xAOD::TrackParticle& particle, const bool isFake, const bool isAssociatedTruth, const float mu, const unsigned int nVtx, float weight);

  // fill IDPVM Ntuple
  void fillNtuple(const xAOD::TrackParticle& track);
  void fillNtuple(const xAOD::TruthParticle& truth);
  void fillNtuple(const xAOD::TrackParticle& track, const xAOD::TruthParticle& truth, const int truthMatchRanking); 

  virtual ~InDetRttPlots() {/**nop**/
  };

private:
  
  /// configuration object
  InDetRttPlotConfig m_config; 

  /// key for the truth match prob 
  const std::string m_trackParticleTruthProbKey{"truthMatchProbability"};

  /// plot members.
  /// No explicit initialisation, since unique_ptr will default-construct 
  /// to return a nullptr
  std::unique_ptr<InDetPerfPlot_TrackParameters> m_trackParameters;
  std::unique_ptr<InDetPerfPlot_nTracks> m_nTracks;
  std::unique_ptr<InDetPerfPlot_HitResidual> m_hitResidualPlot;
  std::unique_ptr<InDetPerfPlot_HitEfficiency> m_hitEffPlot;
  std::unique_ptr<InDetPerfPlot_FakeRate> m_fakePlots;
  std::unique_ptr<InDetPerfPlot_FakeRate> m_missingTruthFakePlots;
  std::unique_ptr<InDetPerfPlot_Resolution> m_resolutionPlotPrim;
  std::unique_ptr<InDetPerfPlot_Resolution> m_resolutionPlotPrim_truthFromB;
  std::unique_ptr<InDetPerfPlot_Hits> m_hitsRecoTracksPlots;
  std::unique_ptr<InDetPerfPlot_Efficiency> m_effPlots;
  std::unique_ptr<InDetPerfPlot_VerticesVsMu> m_verticesVsMuPlots;
  std::unique_ptr<InDetPerfPlot_Vertex> m_vertexPlots;
  std::unique_ptr<InDetPerfPlot_Vertex> m_hardScatterVertexPlots;
  std::unique_ptr<InDetPerfPlot_VertexTruthMatching> m_hardScatterVertexTruthMatchingPlots;
  std::unique_ptr<InDetPerfPlot_TRTExtension> m_trtExtensionPlots;
  std::unique_ptr<InDetPerfPlot_ANTracking> m_anTrackingPlots;
  std::unique_ptr<InDetPerfNtuple_TruthToReco> m_ntupleTruthToReco;
  std::unique_ptr<InDetPerfPlot_Resolution> m_resolutionPlotSecd;
  std::unique_ptr<InDetPerfPlot_Hits> m_hitsMatchedTracksPlots;
  std::unique_ptr<InDetPerfPlot_Hits> m_hitsFakeTracksPlots;
  std::unique_ptr<InDetPerfPlot_Hits> m_hitsUnlinkedTracksPlots;
  std::unique_ptr<InDetPerfPlot_VertexTruthMatching> m_vertexTruthMatchingPlots;
  std::unique_ptr<InDetPerfPlot_TrkInJet> m_trkInJetPlots;
  std::unique_ptr<InDetPerfPlot_TrkInJet> m_trkInJetPlots_bjets;
  std::unique_ptr<InDetPerfPlot_TrkInJet> m_trkInJetPlots_matched;
  std::unique_ptr<InDetPerfPlot_TrkInJet> m_trkInJetPlots_matched_bjets;
  std::unique_ptr<InDetPerfPlot_TrkInJet> m_trkInJetPlots_fake;
  std::unique_ptr<InDetPerfPlot_TrkInJet> m_trkInJetPlots_fake_bjets;
  std::unique_ptr<InDetPerfPlot_TrkInJet> m_trkInJetPlots_unlinked;
  std::unique_ptr<InDetPerfPlot_TrkInJet> m_trkInJetPlots_unlinked_bjets;
  
  std::unique_ptr<InDetPerfPlot_TrkInJet> m_trkInJetPlots_truthFromB;

  std::unique_ptr<InDetPerfPlot_Efficiency> m_effSiSPSeededFinderPlots;
  std::unique_ptr<InDetPerfPlot_Efficiency> m_effInDetExtensionProcessorPlots;
  std::unique_ptr<InDetPerfPlot_Efficiency> m_effTRTSeededTrackFinderPlots;
  std::unique_ptr<InDetPerfPlot_Efficiency> m_effTRTStandalonePlots;
  std::unique_ptr<InDetPerfPlot_Efficiency> m_effSiSpacePointsSeedMaker_LargeD0Plots;

  std::unique_ptr<InDetPerfPlot_FakeRate> m_fakeSiSPSeededFinderPlots;
  std::unique_ptr<InDetPerfPlot_FakeRate> m_fakeInDetExtensionProcessorPlots;
  std::unique_ptr<InDetPerfPlot_FakeRate> m_fakeTRTSeededTrackFinderPlots;
  std::unique_ptr<InDetPerfPlot_FakeRate> m_fakeTRTStandalonePlots;
  std::unique_ptr<InDetPerfPlot_FakeRate> m_fakeSiSpacePointsSeedMaker_LargeD0Plots;

  std::unique_ptr<InDetPerfPlot_TrackParameters> m_trkParaSiSPSeededFinderPlots;
  std::unique_ptr<InDetPerfPlot_TrackParameters> m_trkParaInDetExtensionProcessorPlots;
  std::unique_ptr<InDetPerfPlot_TrackParameters> m_trkParaTRTSeededTrackFinderPlots;
  std::unique_ptr<InDetPerfPlot_TrackParameters> m_trkParaTRTStandalonePlots;
  std::unique_ptr<InDetPerfPlot_TrackParameters> m_trkParaSiSpacePointsSeedMaker_LargeD0Plots;

  std::unique_ptr<InDetPerfPlot_Resolution> m_resSiSPSeededFinderPlots;
  std::unique_ptr<InDetPerfPlot_Resolution> m_resInDetExtensionProcessorPlots;
  std::unique_ptr<InDetPerfPlot_Resolution> m_resTRTSeededTrackFinderPlots;
  std::unique_ptr<InDetPerfPlot_Resolution> m_resTRTStandalonePlots;
  std::unique_ptr<InDetPerfPlot_Resolution> m_resSiSpacePointsSeedMaker_LargeD0Plots;
};




#endif
