/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include "InDetPerfPlot_Resolution.h"

///class holding all plots for Inner Detector RTT Validation and implementing fill methods
class InDetRttPlots: public InDetPlotBase {
public:
  InDetRttPlots(InDetPlotBase* pParent, const std::string& dirName, const int iDetailLevel = 10);

  ///fill for things needing truth and track only
  void fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle);
  ///fill for things needing track only
  void fill(const xAOD::TrackParticle& particle);
  ///fill for things needing truth only
  void fill(const xAOD::TruthParticle& particle);
  ///Fill for efficiency plots
  void fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::TrackParticle& track, const bool isGood, unsigned int /*mu=0*/);

  ///fill for things needing all truth - not just the ones from the reco tracks
  
  ///fill reco-vertex related plots
  void fill(const xAOD::VertexContainer& vertexContainer, const std::vector<const xAOD::TruthVertex*>& truthHSVertices, const std::vector<const xAOD::TruthVertex*>& truthPUVertices);
  ///fill reco-vertex related plots that need EventInfo
  void fill(const xAOD::VertexContainer& vertexContainer, unsigned int nPU);

  void fill(const xAOD::TrackParticle& track, const xAOD::Jet& jet);
  void fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::Jet& jet, const bool isGood);
  void fillFakeRate(const xAOD::TrackParticle& track, const xAOD::Jet& jet, const bool isFake);
  
  virtual ~InDetRttPlots() {/**nop**/
  };
  ///fill for Counters
  void fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory counter);
  ///fill for fakes
  void fillFakeRate(const xAOD::TrackParticle& particle, const bool isFake, const bool isAssociatedTruth);

private:
  InDetPerfPlot_TrackParameters m_trackParameters;
  InDetPerfPlot_nTracks m_nTracks;
  InDetPerfPlot_HitResidual m_hitResidualPlot;
  InDetPerfPlot_HitEfficiency m_hitEffPlot;
  InDetPerfPlot_FakeRate m_fakePlots;
  InDetPerfPlot_FakeRate m_missingTruthFakePlots;
  InDetPerfPlot_Resolution m_resolutionPlotPrim;
  InDetPerfPlot_Hits m_hitsRecoTracksPlots;
  InDetPerfPlot_Efficiency m_effPlots;
  InDetPerfPlot_VerticesVsMu m_verticesVsMuPlots;
  InDetPerfPlot_Vertex m_vertexPlots;
  InDetPerfPlot_Vertex m_hardScatterVertexPlots;
  InDetPerfPlot_VertexTruthMatching m_hardScatterVertexTruthMatchingPlots;

  std::unique_ptr<InDetPerfPlot_Resolution> m_resolutionPlotSecd;
  std::unique_ptr<InDetPerfPlot_Hits> m_hitsMatchedTracksPlots;
  std::unique_ptr<InDetPerfPlot_VertexTruthMatching> m_vertexTruthMatchingPlots;
  bool m_doTrackInJetPlots;
  std::unique_ptr<InDetPerfPlot_TrkInJet> m_trkInJetPlots;

  //By track authors
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




  std::string m_trackParticleTruthProbKey;
  float m_truthProbLowThreshold;
};




#endif
