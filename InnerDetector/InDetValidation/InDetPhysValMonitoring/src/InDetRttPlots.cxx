/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  m_duplicatePlots(this, "Tracks/SelectedGoodTracks"),
  m_trkInJetPlot(this, "Tracks/SelectedGoodJetTracks"),
  m_trkInJetPlot_highPt(this, "Tracks/SelectedGoodHighPtJetTracks"),
  m_trkInJetPtPlot(this, "Tracks/SelectedGoodJetTracks"),
  m_trkInJetBasicPlot(this, "Tracks/SelectedGoodJetTracks"),
  m_trkInJetTrackRecoInfoPlots(this, "Tracks/SelectedGoodJetTracks"),
  m_trkInJetHitsDetailedPlots(this, "Tracks/SelectedGoodJetTracks"),
  m_trkInJetFakePlots(this, "Tracks/SelectedFakeJetTracks"),
  m_trkInJetResPlots(this, "Tracks/SelectedGoodJetTracks"),
  m_trkInJetResPlotsDr0010(nullptr),
  m_trkInJetResPlotsDr1020(nullptr),
  m_trkInJetResPlotsDr2030(nullptr),

  m_trkInJetEffPlots(this, "Tracks/SelectedGoodJetTracks"),
  m_trkInJetHighPtResPlots(this, "Tracks/SelectedGoodJetHighPtTracks"),
  m_trkInJetHitsFakeTracksPlots(this, "Tracks/SelectedFakeJetTracks"),
  m_trkInJetHitsMatchedTracksPlots(this, "Tracks/SelectedMatchedJetTracks"),
  m_trkInJetTrackTruthInfoPlots(this, "TruthInJet"),
  m_specPlots(this, "Tracks/PreSelectionSpectrumPlots") {
  m_moreJetPlots = false; // changed with setter function
  m_ITkResPlots = false;
  // These settings are probably all redundant & can be removed from this script
  m_trackParticleTruthProbKey = "truthMatchProbability";
  m_truthProbLowThreshold = 0.5;

  if (m_moreJetPlots) {
    m_trkInJetResPlotsDr0010 = new InDetPerfPlot_res(this, "Tracks/SelectedGoodJetDr0010Tracks");
    m_trkInJetResPlotsDr1020 = new InDetPerfPlot_res(this, "Tracks/SelectedGoodJetDr1020Tracks");
    m_trkInJetResPlotsDr2030 = new InDetPerfPlot_res(this, "Tracks/SelectedGoodJetDr2030Tracks");
  }
  if (m_ITkResPlots) {
    // Resolutions for ITk
    m_ITkResolutionPlotPrim = new InDetPerfPlot_resITk(this, "Tracks/SelectedMatchedTracks/Primary");
    m_ITkResolutionPlotSecd = new InDetPerfPlot_resITk(this, "Tracks/SelectedMatchedTracks/Secondary");
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

void
InDetRttPlots::fill(const xAOD::VertexContainer& vertexContainer) {
  // fill vertex container general properties
  // m_verticesPlots.fill(vertexContainer); //if ever needed
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
InDetRttPlots::fill(const xAOD::VertexContainer& vertexContainer, const xAOD::EventInfo& ei) {
  m_verticesPlots.fill(vertexContainer, ei);
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

bool
InDetRttPlots::filltrkInJetPlot(const xAOD::TrackParticle& particle, const xAOD::Jet& jet) {
  ATH_MSG_VERBOSE("Filling trk in jet");
  bool pass = m_trkInJetPlot.fill(particle, jet);
  ATH_MSG_VERBOSE("Filling trk in jet hi pt");
  m_trkInJetPlot_highPt.fill(particle, jet);
  return pass;
}

void
InDetRttPlots::fillSimpleJetPlots(const xAOD::TrackParticle& particle, float prob) {
  // the full suit of track plots
  m_trkInJetPtPlot.fill(particle);
  m_trkInJetBasicPlot.fill(particle);
  m_trkInJetTrackRecoInfoPlots.fill(particle);
  m_trkInJetHitsDetailedPlots.fill(particle);

  if (std::isnan(prob)) {
    return;
  }
  const bool isFake = (prob < m_truthProbLowThreshold);
  m_trkInJetFakePlots.fill(particle, isFake, InDetPerfPlot_fakes::ALL);
}

void
InDetRttPlots::fillJetHitsPlots(const xAOD::TrackParticle& particle, float prob, int barcode) {
  if (prob < m_truthProbLowThreshold) {
    m_trkInJetHitsFakeTracksPlots.fill(particle);
  } else if (barcode < 100000 && barcode != 0) {
    m_trkInJetHitsMatchedTracksPlots.fill(particle);
  }
}

void
InDetRttPlots::fillJetResPlots(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truth,
                               const xAOD::Jet& jet) {
  // fill pull and resolution plots
  m_trkInJetResPlots.fill(particle, truth);
  if (particle.pt() > 10e3) { // 10 GeV
    m_trkInJetHighPtResPlots.fill(particle, truth);
  }
  if (m_moreJetPlots) {
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
  m_trkInJetPlot.BookEffReco(truth, jet);         // fill hists with truth info!
  m_trkInJetPlot_highPt.BookEffReco(truth, jet);  // fill hists with truth info!
}

void
InDetRttPlots::jet_fill(const xAOD::TrackParticle& track, const xAOD::Jet& jet, float weight) {
  m_trkInJetEffPlots.jet_fill(track, jet, weight);
}

void
InDetRttPlots::jetBMR(const xAOD::TrackParticle& track, const xAOD::Jet& jet, float weight) {
  m_BadMatchRate.jetBMR(track, jet, weight);
}

void
InDetRttPlots::fillJetTrkTruth(const xAOD::TruthParticle& truth, const xAOD::Jet& jet) {
  m_trkInJetPlot.BookEffTruth(truth, jet);
  m_trkInJetPlot_highPt.BookEffTruth(truth, jet);
}

void
InDetRttPlots::fillJetPlotCounter(const xAOD::Jet& jet) {
  m_trkInJetPlot.fillCounter(jet);
  m_trkInJetPlot_highPt.fillCounter(jet);
}

void
InDetRttPlots::fillJetTrkTruthCounter(const xAOD::Jet& jet) {
  m_trkInJetPlot.fillEff(jet);
  m_trkInJetPlot_highPt.fillEff(jet);
}

