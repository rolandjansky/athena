/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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


namespace { // utility functions used in this area - stolen from InDetPhysValMonitoringTool and other places - not so
            // nice as should be in a common header somewhere
  // get truth/track matching probability
  float
  getMatchingProbability(const xAOD::TrackParticle& trackParticle) {
    float result(std::numeric_limits<float>::quiet_NaN());

    if (trackParticle.isAvailable<float>("truthMatchProbability")) {
      result = trackParticle.auxdata<float>("truthMatchProbability");
    }
    return result;
  }
}// namespace


InDetRttPlots::InDetRttPlots(InDetPlotBase* pParent, const std::string& sDir) : InDetPlotBase(pParent, sDir),

  m_ptPlot(this, "Tracks/SelectedGoodTracks"),
  m_basicPlot(this, "Tracks/SelectedGoodTracks"),
  m_TrackRecoInfoPlots(this, "Tracks/SelectedGoodTracks"),
  m_TrackTruthInfoPlots(this, "Truth"),
  m_nTracks(this, "Tracks/SelectedGoodTracks"),
  m_resPlots(this, "Tracks/SelectedGoodTracks"),
  m_hitResidualPlot(this, "Tracks/SelectedGoodTracks"),
  m_hitEffPlot(this, "Tracks/SelectedGoodTracks"),
  m_fakePlots(this, "Tracks/SelectedFakeTracks"),
  m_ITkResolutionPlotPrim(nullptr),
  m_ITkResolutionPlotSecd(nullptr),
  m_hitsMatchedTracksPlots(this, "Tracks/SelectedMatchedTracks"),
  m_hitsDetailedPlots(this, "Tracks/SelectedGoodTracks"),
  m_effPlots(this, "Tracks/SelectedGoodTracks"),
  m_dumPlots(this, "Tracks/SelectedGoodTracks"),
  m_BadMatchRate(this, "Tracks/SelectedBadMatchTracks"),
  m_verticesPlots(this, "Vertices/AllPrimaryVertices"),
  m_vertexPlots(this, "Vertices/AllPrimaryVertices"),
  m_hardScatterVertexPlots(this, "Vertices/HardScatteringVertex"),
  m_vertexTruthMatchingPlots(this, "Vertices/Details"),
  m_duplicatePlots(this, "Tracks/SelectedGoodTracks"),
  m_specPlots(this, "Tracks/PreSelectionSpectrumPlots"),
  m_doTrackInJetPlots(true),
  m_doTrackInBJetPlots(true)

 {
  // These settings are probably all redundant & can be removed from this script
  m_trackParticleTruthProbKey = "truthMatchProbability";
  m_truthProbLowThreshold = 0.5;

}


void InDetRttPlots::SetFillJetPlots(bool fillJets, bool fillBJets){

  m_doTrackInJetPlots = fillJets;
  m_doTrackInBJetPlots = fillBJets;

  if(m_doTrackInJetPlots){
    m_trkInJetPlots = std::make_unique<InDetPerfPlot_TrkInJet>(this, "TracksInJets/Tracks");
    if (m_iDetailLevel >= 200){
      m_trkInJetPlots_matched = std::make_unique<InDetPerfPlot_TrkInJet>(this, "TracksInJets/Matched",false);
      m_trkInJetPlots_fake = std::make_unique<InDetPerfPlot_TrkInJet>(this, "TracksInJets/Fakes",false);
      m_trkInJetPlots_unlinked = std::make_unique<InDetPerfPlot_TrkInJet>(this, "TracksInJets/Unlinked",false);
    }
    if(m_doTrackInBJetPlots){
      m_trkInJetPlots_bjets = std::make_unique<InDetPerfPlot_TrkInJet>(this, "TracksInBJets/Tracks");
      if (m_iDetailLevel >= 200){
        m_trkInJetPlots_matched_bjets = std::make_unique<InDetPerfPlot_TrkInJet>(this, "TracksInBJets/Matched",false);
        m_trkInJetPlots_fake_bjets = std::make_unique<InDetPerfPlot_TrkInJet>(this, "TracksInBJets/Fakes",false);
        m_trkInJetPlots_unlinked_bjets = std::make_unique<InDetPerfPlot_TrkInJet>(this, "TracksInBJets/Unlinked",false);
      }
    }
  }

}


void
InDetRttPlots::fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle) {
  // fill measurement bias, resolution, and pull plots
  m_resPlots.fill(particle, truthParticle);
  m_basicPlot.fill(truthParticle);

  // fill ITK resolutions (bias / resolutions)
  if (m_ITkResPlots) {
    if (particle.isAvailable<float>(m_trackParticleTruthProbKey)) {
      const float prob = particle.auxdata<float>(m_trackParticleTruthProbKey);
      float barcode = truthParticle.barcode();
      if (barcode < 200000 && barcode != 0 && prob > 0.5) {
        m_ITkResolutionPlotPrim->fill(particle, truthParticle);
      } else if (barcode >= 200000 && prob > 0.7) {
        m_ITkResolutionPlotSecd->fill(particle, truthParticle);
      }
    }
  }
  // Not sure that the following hitsMatchedTracksPlots does anything...
  float barcode = truthParticle.barcode();
  if (barcode < 100000 && barcode != 0) { // Not sure why the barcode limit is 100k instead of 200k...
    m_hitsMatchedTracksPlots.fill(particle);
  }
}

void
InDetRttPlots::fillSpectrum(const xAOD::TrackParticle& trackParticle) {
  float prob = getMatchingProbability(trackParticle);
  m_specPlots.fillSpectrum(trackParticle, prob);
}

void
InDetRttPlots::fillSpectrum(const xAOD::TruthParticle& particle) {
  m_specPlots.fillSpectrum(particle);
}

void
InDetRttPlots::fillSpectrum(const xAOD::TrackParticle& trkprt, const xAOD::TruthVertex& truthVrt) {
  m_specPlots.fillSpectrum(trkprt, truthVrt);
}

void
InDetRttPlots::fillSpectrum(const xAOD::TrackParticle& trkprt, const xAOD::Vertex& vertex, bool fill) {
  m_specPlots.fillSpectrum(trkprt, vertex, fill);
}

void
InDetRttPlots::fillSpectrumLinked(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle,
                                  float /*weight*/) {
  double prob = getMatchingProbability(particle);
  m_specPlots.fillSpectrumLinked(particle, truthParticle, prob);
}

void
InDetRttPlots::fillLinkedandUnlinked(const xAOD::TrackParticle& particle, float Prim_w, float Sec_w, float Unlinked_w, unsigned int nMuEvents) {
  m_fakePlots.fillLinkedandUnlinked(particle, Prim_w, Sec_w, Unlinked_w,nMuEvents);
}

void
InDetRttPlots::fillSpectrumUnlinked2(const xAOD::TrackParticle& particle) {
  double prob = getMatchingProbability(particle);
  m_specPlots.fillSpectrumUnlinked2(particle, prob);
}

void
InDetRttPlots::fillSingleMatch(const xAOD::TrackParticle& trackParticle) {
  m_duplicatePlots.fillSingleMatch(trackParticle);
}

void
InDetRttPlots::fillTwoMatchDuplicate(Float_t prob1, Float_t prob2, const xAOD::TrackParticle& trackParticle,
                                     const xAOD::TrackParticle& particle, const xAOD::TruthParticle& tp) {
  m_duplicatePlots.fillTwoMatchDuplicate(prob1, prob2, trackParticle, particle, tp);
}

void
InDetRttPlots::fill(const xAOD::TrackParticle& particle) {
  m_hitResidualPlot.fill(particle);
  m_hitEffPlot.fill(particle);
  // fill pt plots
  m_ptPlot.fill(particle); // to maintain correspondence with old InDetPerformanceRTT
  m_basicPlot.fill(particle);
  m_TrackRecoInfoPlots.fill(particle);
  m_hitsDetailedPlots.fill(particle);
}

void
InDetRttPlots::fillEfficiency(const xAOD::TruthParticle& truth, const bool isGood, const unsigned int nMuEvents) {
  m_effPlots.fill(truth, isGood);
  m_effPlots.mu_fill(truth,isGood,nMuEvents);
  m_fakePlots.fillIncFakeDenom(truth,nMuEvents);
}

void
InDetRttPlots::algoEfficiency(double radius, int SiSPweight, int TRTSeededweight, int TRTStandaloneweight, int other_weight){
  m_dumPlots.algoEfficiency(radius, SiSPweight, TRTSeededweight, TRTStandaloneweight, other_weight);
}

void
InDetRttPlots::track_author(std::bitset<xAOD::NumberOfTrackRecoInfo> authorset){
  m_dumPlots.track_author(authorset);
}

void
InDetRttPlots::lepton_fill(const xAOD::TruthParticle& truth, float weight) {
  m_dumPlots.lepton_fill(truth, weight);
}

void
InDetRttPlots::prim_photon_fill(const xAOD::TruthParticle& truth) {
  m_dumPlots.prim_photon_fill(truth);
}

void
InDetRttPlots::brem_photon_fill(const xAOD::TruthParticle& truth) {
  m_dumPlots.brem_photon_fill(truth);
}

void
InDetRttPlots::track_vs_truth(const xAOD::TrackParticle& track,const xAOD::TruthParticle& truth, float tmp){
  m_dumPlots.track_vs_truth(track, truth, tmp);
}

void
InDetRttPlots::minDR(float min_dR, float prod_rad, float bestmatch, double BIDPt){
  m_dumPlots.minDR(min_dR, prod_rad, bestmatch, BIDPt);
}

void
InDetRttPlots::BT_fill(const xAOD::TruthParticle& truth, float weight) {
  m_effPlots.BT_fill(truth, weight);
}

void
InDetRttPlots::fill(const xAOD::TruthParticle& truthParticle) {
  // fill truth plots
  m_TrackTruthInfoPlots.fill(truthParticle);
}

void
InDetRttPlots::fillBMR(const xAOD::TrackParticle& track, float weight) {
  // fill the plot requiring truth matching probability less than the upper limit (50.1% right now)
  m_BadMatchRate.fillBMR(track, weight);
}

void
InDetRttPlots::fillRF(const xAOD::TrackParticle& track, float weight) {
  // fill the plot requiring truth matching probability less than the lower limit (50.0% right now)
  m_BadMatchRate.fillRF(track, weight);
}


//Fill Vertexing Plots
//
void
InDetRttPlots::fill(const xAOD::VertexContainer& vertexContainer, const std::vector<const xAOD::TruthVertex*>& truthHSVertices, const std::vector<const xAOD::TruthVertex*>& truthPUVertices) {
  // fill vertex container general properties
  // m_verticesVsMuPlots.fill(vertexContainer); //if ever needed
  // fill vertex-specific properties, for all vertices and for hard-scattering vertex
  for (const auto& vtx : vertexContainer.stdcont()) {
    if (vtx->vertexType() == xAOD::VxType::NoVtx) {
      ATH_MSG_DEBUG("IN InDetRttPlots::fill, found xAOD::VxType::NoVtx");
      continue; // skip dummy vertex
    }
    m_vertexPlots.fill(*vtx);
    ATH_MSG_DEBUG("IN InDetRttPlots::fill, filling for all vertices");
    if (vtx->vertexType() == xAOD::VxType::PriVtx) {
      m_hardScatterVertexPlots.fill(*vtx);
    }
  }

  m_vertexTruthMatchingPlots.fill(vertexContainer, truthHSVertices, truthPUVertices);
  
}

void
InDetRttPlots::fill(const xAOD::VertexContainer& vertexContainer, unsigned int nPU ) {
  m_verticesPlots.fill(vertexContainer, nPU);
}

void
InDetRttPlots::fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory counter) {
  m_nTracks.fill(freq, counter);
}

void
InDetRttPlots::fillFakeRate(const xAOD::TrackParticle& particle, const bool match,
                            const InDetPerfPlot_fakes::Category c) {
  m_fakePlots.fill(particle, match, c);
}

//Track in Jet Plots
void
InDetRttPlots::fill(const xAOD::TrackParticle& track, const xAOD::Jet& jet, bool isBjet, bool isFake, bool isUnlinked){
  m_trkInJetPlots->fill(track, jet);
  if (m_iDetailLevel >= 200){
    if (isFake){
      m_trkInJetPlots_fake->fill(track,jet);
    }
    else if (isUnlinked){
      m_trkInJetPlots_unlinked->fill(track,jet);
    }
    else {
      m_trkInJetPlots_matched->fill(track,jet);
    }
  }
  if(isBjet && m_doTrackInBJetPlots){
     m_trkInJetPlots_bjets->fill(track, jet);
    if (m_iDetailLevel >= 200){
      if (isFake){
        m_trkInJetPlots_fake_bjets->fill(track,jet);
      }
      else if (isUnlinked){
        m_trkInJetPlots_unlinked_bjets->fill(track,jet);
      }
      else {
        m_trkInJetPlots_matched_bjets->fill(track,jet);
      }
    }
  }
}

void
InDetRttPlots::fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::Jet& jet, bool isEfficient, bool isBjet) {
  m_trkInJetPlots->fillEfficiency(truth, jet, isEfficient);
  if(isBjet && m_doTrackInBJetPlots) m_trkInJetPlots_bjets->fillEfficiency(truth, jet, isEfficient);
}

void
InDetRttPlots::fillFakeRate(const xAOD::TrackParticle& track, const xAOD::Jet& jet, bool isFake, bool isBjet) {
   m_trkInJetPlots->fillFakeRate(track, jet, isFake);
   if(isBjet && m_doTrackInBJetPlots) m_trkInJetPlots_bjets->fillFakeRate(track, jet, isFake);
}

