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
  m_hitsMatchedTracksPlots(this, "Tracks/Matched/HitsOnTracks"),
  m_hitsRecoTracksPlots(this, "Tracks/Selected/HitsOnTracks"),
  m_effPlots(this, "Tracks/Efficiency"),
  m_verticesVsMuPlots(this, "Vertices/AllPrimaryVertices"),
  m_vertexPlots(this, "Vertices/AllPrimaryVertices"),
  m_hardScatterVertexPlots(this, "Vertices/HardScatteringVertex"),
  m_doTrackInJetPlots(true) //FIX CONFIGURATION
  {
  m_trackParticleTruthProbKey = "truthMatchProbability";
  m_truthProbLowThreshold = 0.5;
  
  if(m_iDetailLevel >= 200){
    m_resolutionPlotSecd = new InDetPerfPlot_Resolution(this, "Tracks/Matched/Resolutions/Secondary");
  }

  //A lot of Jets... do we need these at all???
  if(m_doTrackInJetPlots){
    m_trkInJetPlot = new InDetPerfPlot_TrkInJet(this, "TracksInJets/SelectedGoodJetTracks");
    m_trkInJetPlot_highPt = new InDetPerfPlot_TrkInJet(this, "TracksInJets/SelectedGoodHighPtJetTracks");
    m_trkInJetBasicPlot = new InDetPerfPlot_TrackParameters(this, "TracksInJets/SelectedGoodJetTracks");
    m_trkInJetHitsDetailedPlots = new InDetPerfPlot_Hits(this, "TracksInJets/SelectedGoodJetTracks");
    m_trkInJetFakePlots = new InDetPerfPlot_FakeRate(this, "TracksInJets/SelectedFakeJetTracks");
    m_trkInJetResPlots = new InDetPerfPlot_Resolution(this, "TracksInJets/SelectedGoodJetTracks");
    m_trkInJetEffPlots = new InDetPerfPlot_Efficiency(this, "TracksInJets/SelectedGoodJetTracks");
    m_trkInJetHighPtResPlots = new InDetPerfPlot_Resolution(this, "TracksInJets/SelectedGoodJetHighPtTracks");
    m_trkInJetHitsFakeTracksPlots = new InDetPerfPlot_Hits(this, "TracksInJets/SelectedFakeJetTracks");
    m_trkInJetHitsMatchedTracksPlots = new InDetPerfPlot_Hits(this, "TracksInJets/SelectedMatchedJetTracks");
    if(m_iDetailLevel >= 200) {
      m_trkInJetResPlotsDr0010 = new InDetPerfPlot_Resolution(this, "Tracks/SelectedGoodJetDr0010Tracks");
      m_trkInJetResPlotsDr1020 = new InDetPerfPlot_Resolution(this, "Tracks/SelectedGoodJetDr1020Tracks");
      m_trkInJetResPlotsDr2030 = new InDetPerfPlot_Resolution(this, "Tracks/SelectedGoodJetDr2030Tracks");
    }
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
  // Not sure that the following hitsMatchedTracksPlots does anything...
  float barcode = truthParticle.barcode();
  if (barcode < 100000 && barcode != 0) { // Not sure why the barcode limit is 100k instead of 200k...
    m_hitsMatchedTracksPlots.fill(particle);
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
      continue; // skip dummy vertex
    }
    m_vertexPlots.fill(*vtx);
    if (vtx->vertexType() == xAOD::VxType::PriVtx) {
      m_hardScatterVertexPlots.fill(*vtx);
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


bool
InDetRttPlots::filltrkInJetPlot(const xAOD::TrackParticle& particle, const xAOD::Jet& jet) {
  ATH_MSG_VERBOSE("Filling trk in jet");
  bool pass = m_trkInJetPlot->fill(particle, jet);
  ATH_MSG_VERBOSE("Filling trk in jet hi pt");
  m_trkInJetPlot_highPt->fill(particle, jet);
  return pass;
}

void
InDetRttPlots::fillSimpleJetPlots(const xAOD::TrackParticle& particle, float prob) {
  // the full suit of track plots
  m_trkInJetBasicPlot->fill(particle);
  m_trkInJetHitsDetailedPlots->fill(particle);

  if (std::isnan(prob)) {
    return;
  }
  const bool isFake = prob < 0.5;
  m_trkInJetFakePlots->fill(particle, isFake);
}

void
InDetRttPlots::fillJetHitsPlots(const xAOD::TrackParticle& particle, float prob, int barcode) {
  if (prob < m_truthProbLowThreshold) {
    m_trkInJetHitsFakeTracksPlots->fill(particle);
  } else if (barcode < 100000 && barcode != 0) {
    m_trkInJetHitsMatchedTracksPlots->fill(particle);
  }
}

void
InDetRttPlots::fillJetResPlots(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truth,
                               const xAOD::Jet& jet) {
  // fill pull and resolution plots
  m_trkInJetResPlots->fill(particle, truth);
  if (particle.pt() > 10e3) { // 10 GeV
    m_trkInJetHighPtResPlots->fill(particle, truth);
  }
  if(m_iDetailLevel >= 200) {
    float dR(jet.p4().DeltaR(particle.p4()));
    if (dR < 0.1) {
      m_trkInJetResPlotsDr0010->fill(particle, truth);
    } else if (dR < 0.2) {
      m_trkInJetResPlotsDr1020->fill(particle, truth);
    } else if (dR < 0.3) {
      m_trkInJetResPlotsDr2030->fill(particle, truth);
    }
  } // m_moreJetPlots
}

void
InDetRttPlots::fillJetEffPlots(const xAOD::TruthParticle& truth, const xAOD::Jet& jet) {
  m_trkInJetPlot->BookEffReco(truth, jet);         // fill hists with truth info!
  m_trkInJetPlot_highPt->BookEffReco(truth, jet);  // fill hists with truth info!
}


void
InDetRttPlots::fillJetTrkTruth(const xAOD::TruthParticle& truth, const xAOD::Jet& jet) {
  m_trkInJetPlot->BookEffTruth(truth, jet);
  m_trkInJetPlot_highPt->BookEffTruth(truth, jet);
}

void
InDetRttPlots::fillJetPlotCounter(const xAOD::Jet& jet) {
  m_trkInJetPlot->fillCounter(jet);
  m_trkInJetPlot_highPt->fillCounter(jet);
}

void
InDetRttPlots::fillJetTrkTruthCounter(const xAOD::Jet& jet) {
  m_trkInJetPlot->fillEff(jet);
  m_trkInJetPlot_highPt->fillEff(jet);
}

