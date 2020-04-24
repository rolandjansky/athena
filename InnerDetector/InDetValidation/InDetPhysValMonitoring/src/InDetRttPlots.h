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
  InDetRttPlots(InDetPlotBase* pParent, const std::string& dirName);

  ///fill for things needing truth and track only
  void fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle);
  ///fill for things needing track only
  void fill(const xAOD::TrackParticle& particle);
  ///fill for things needing truth only
  void fill(const xAOD::TruthParticle& particle);
  ///Fill for efficiency plots
  void fillEfficiency(const xAOD::TruthParticle& truth, const bool isGood, unsigned int /*mu=0*/);

  ///fill for things needing all truth - not just the ones from the reco tracks
  
  ///fill reco-vertex related plots
  void fill(const xAOD::VertexContainer& vertexContainer);
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
  InDetPerfPlot_Resolution* m_resolutionPlotSecd;
  InDetPerfPlot_Hits m_hitsMatchedTracksPlots;
  InDetPerfPlot_Hits m_hitsRecoTracksPlots;
  InDetPerfPlot_Efficiency m_effPlots;

  InDetPerfPlot_VerticesVsMu m_verticesVsMuPlots;
  InDetPerfPlot_Vertex m_vertexPlots;
  InDetPerfPlot_Vertex m_hardScatterVertexPlots;
  InDetPerfPlot_VertexTruthMatching m_vertexTruthMatchingPlots;
  InDetPerfPlot_VertexTruthMatching m_hardScatterVertexTruthMatchingPlots;

  bool m_doTrackInJetPlots;

  InDetPerfPlot_TrkInJet *m_trkInJetPlots;

  std::string m_trackParticleTruthProbKey;
  float m_truthProbLowThreshold;
};




#endif
