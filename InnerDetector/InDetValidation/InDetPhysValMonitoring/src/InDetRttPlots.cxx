/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetRttPlots.cxx
 * @author shaun roe
 **/

#include "InDetRttPlots.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include <cmath> // std::isnan()
#include <limits>

InDetRttPlots::InDetRttPlots(InDetPlotBase* pParent, const std::string& sDir) : InDetPlotBase(pParent, sDir),
  m_trackParameters(this, "Tracks/Selected/Parameters"),
  m_nTracks(this, "Tracks/Tracks"),
  m_hitResidualPlot(this, "Tracks/Hits/Residuals"),
  m_hitEffPlot(this, "Tracks/Hits/Efficiency"),
  m_fakePlots(this, "Tracks/FakeRate"),
  m_missingTruthFakePlots(this, "Tracks/Unlinked/FakeRate"),
  m_resolutionPlotPrim(this, "Tracks/Matched/Resolutions/Primary"),
  m_resolutionPlotSecd(nullptr),
  m_hitsRecoTracksPlots(this, "Tracks/Selected/HitsOnTracks"),
  m_effPlots(this, "Tracks/Efficiency"),
  m_verticesVsMuPlots(this, "Vertices/AllPrimaryVertices"),
  m_vertexPlots(this, "Vertices/AllPrimaryVertices"),
  m_hardScatterVertexPlots(this, "Vertices/HardScatteringVertex"),
  m_hardScatterVertexTruthMatchingPlots(this, "Vertices/HardScatteringVertex"),
  m_doTrackInJetPlots(true) //FIX CONFIGURATION
  {
  m_trackParticleTruthProbKey = "truthMatchProbability";
  m_truthProbLowThreshold = 0.5;
  
  if(m_iDetailLevel >= 200){
    m_resolutionPlotSecd = new InDetPerfPlot_Resolution(this, "Tracks/Matched/Resolutions/Secondary");
    m_hitsMatchedTracksPlots = new InDetPerfPlot_Hits(this, "Tracks/Matched/HitsOnTracks");
    m_vertexTruthMatchingPlots = new InDetPerfPlot_VertexTruthMatching(this, "Vertices/AllPrimaryVertices");
  }

  //A lot of Jets... do we need these at all???
  if(m_doTrackInJetPlots){
    m_trkInJetPlots = new InDetPerfPlot_TrkInJet(this, "TracksInJets/Tracks");
  }
}

//
//Fill plots for matched particles
//

void
InDetRttPlots::fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle) {
  // fill measurement bias, resolution, and pull plots

  // fill ITK resolutions (bias / resolutions)
  if (particle.isAvailable<float>(m_trackParticleTruthProbKey)) {
    const float prob = particle.auxdata<float>(m_trackParticleTruthProbKey);
    float barcode = truthParticle.barcode();
    if (barcode < 200000 && barcode != 0 && prob > 0.5) {
        m_resolutionPlotPrim.fill(particle, truthParticle);
    } else if (barcode >= 200000 && prob > 0.7 && m_iDetailLevel >= 200) {
        m_resolutionPlotSecd->fill(particle, truthParticle);
    }
  }
 
  if(m_iDetailLevel >= 200){
    float barcode = truthParticle.barcode();
    if (barcode < 200000 && barcode != 0) { 
      m_hitsMatchedTracksPlots->fill(particle);
    }
  }
}

//
//Fill basic track properties for reconstructed tracks 
//

void
InDetRttPlots::fill(const xAOD::TrackParticle& particle) {
  m_hitResidualPlot.fill(particle);
  m_hitEffPlot.fill(particle);
  // fill pt plots
  m_trackParameters.fill(particle);
  m_hitsRecoTracksPlots.fill(particle);
}

//
//Fill plots for selected truth particle
//

void
InDetRttPlots::fill(const xAOD::TruthParticle& truthParticle) {
  // fill truth plots 
  m_trackParameters.fill(truthParticle);
}

//
//Fill Efficiencies
//


void
InDetRttPlots::fillEfficiency(const xAOD::TruthParticle& truth, const bool isGood, const unsigned int /*nMuEvents*/) {
  m_effPlots.fill(truth, isGood);
}

//
//Fill Fake Rates
//

void
InDetRttPlots::fillFakeRate(const xAOD::TrackParticle& track, const bool isFake, const bool isAssociatedTruth){

  m_missingTruthFakePlots.fill(track, !isAssociatedTruth);
  if(isAssociatedTruth) m_fakePlots.fill(track, isFake);

}



//
//Fill Vertexing Plots
//
void
InDetRttPlots::fill(const xAOD::VertexContainer& vertexContainer) {
  // fill vertex container general properties
  // m_verticesVsMuPlots.fill(vertexContainer); //if ever needed
  // fill vertex-specific properties, for all vertices and for hard-scattering vertex
  for (const auto& vtx : vertexContainer.stdcont()) {
    if (vtx->vertexType() == xAOD::VxType::NoVtx) {
      ATH_MSG_DEBUG("IN InDetRttPlots::fill, found xAOD::VxType::NoVtx");
      continue; // skip dummy vertex
    }
    m_vertexPlots.fill(*vtx);
    if(m_iDetailLevel >= 200){
      m_vertexTruthMatchingPlots->fill(*vtx);
    }
    ATH_MSG_DEBUG("IN InDetRttPlots::fill, filling for all vertices");
    if (vtx->vertexType() == xAOD::VxType::PriVtx) {
      m_hardScatterVertexPlots.fill(*vtx);
      m_hardScatterVertexTruthMatchingPlots.fill(*vtx);
      ATH_MSG_DEBUG("IN InDetRttPlots::fill, filling for all HS vertex");
    }
  }
}


void
InDetRttPlots::fill(const xAOD::VertexContainer& vertexContainer, unsigned int nPU) {
  m_verticesVsMuPlots.fill(vertexContainer, nPU);
}

//
//Fill Counters
//
void
InDetRttPlots::fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory counter) {
  m_nTracks.fill(freq, counter);
}

//Track in Jet Plots
void
InDetRttPlots::fill(const xAOD::TrackParticle& track, const xAOD::Jet& jet){
  m_trkInJetPlots->fill(track, jet);
}

void
InDetRttPlots::fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::Jet& jet, bool isEfficient) {
  m_trkInJetPlots->fillEfficiency(truth, jet, isEfficient); 
}

void
InDetRttPlots::fillFakeRate(const xAOD::TrackParticle& track, const xAOD::Jet& jet, bool isFake) {
   m_trkInJetPlots->fillFakeRate(track, jet, isFake); 
}

