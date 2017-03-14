/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_TrtTest.cxx
 * @author margaret lutz
 **/

#include "InDetPerfPlot_TrtTest.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "xAODEventInfo/EventInfo.h"

// get truth/track matching probability
namespace {
  float
  getMatchingProbability(const xAOD::TrackParticle& trackParticle) {
    float result(std::numeric_limits<float>::quiet_NaN());

    if (trackParticle.isAvailable<float>("truthMatchProbability")) {
      result = trackParticle.auxdata<float>("truthMatchProbability");
    }
    return result;
  }

  bool
  inRange(const double eta) {
    return((eta >= -2.0)and(eta <= 2.0));
  }
}
InDetPerfPlot_TrtTest::InDetPerfPlot_TrtTest(InDetPlotBase* pParent, const std::string& sDir) : InDetPlotBase(pParent,
                                                                                                              sDir),
  m_mu{}, m_nPV{}, m_matchProb{}, m_trtTubeFrac{},
  m_trackCategories{}, m_slimTrackCategories{},
  m_pt{}, m_pt_ext{}, m_pt_trtHit{}, m_pt_trtHitNoOut{}, m_pt_trtOutlier{}, m_pt_trtHitOut{}, m_pt_noTrt{},
  m_eta{}, m_eta_ext{}, m_eta_trtHit{}, m_eta_trtHitNoOut{}, m_eta_trtOutlier{}, m_eta_trtHitOut{}, m_eta_noTrt{},
  m_phi{}, m_phi_ext{}, m_phi_trtHit{}, m_phi_trtHitNoOut{}, m_phi_trtOutlier{}, m_phi_trtHitOut{}, m_phi_noTrt{},
  m_d0{}, m_d0_ext{}, m_d0_trtHit{}, m_d0_trtHitNoOut{}, m_d0_trtOutlier{}, m_d0_trtHitOut{}, m_d0_noTrt{},
  m_z0{}, m_z0_ext{}, m_z0_trtHit{}, m_z0_trtHitNoOut{}, m_z0_trtOutlier{}, m_z0_trtHitOut{}, m_z0_noTrt{},
  m_tube{}, m_tube_ext{}, m_tube_trtHit{}, m_tube_trtHitNoOut{}, m_tube_trtOutlier{}, m_tube_trtHitOut{},
  m_tube_noTrt{},

  m_pt_tubeHitFrac_05_1{}, m_eta_tubeHitFrac_05_1{}, m_phi_tubeHitFrac_05_1{}, m_d0_tubeHitFrac_05_1{},
  m_z0_tubeHitFrac_05_1{},
  m_pt_tubeHitFrac_04_05{}, m_eta_tubeHitFrac_04_05{}, m_phi_tubeHitFrac_04_05{}, m_d0_tubeHitFrac_04_05{},
  m_z0_tubeHitFrac_04_05{},
  m_pt_tubeHitFrac_03_04{}, m_eta_tubeHitFrac_03_04{}, m_phi_tubeHitFrac_03_04{}, m_d0_tubeHitFrac_03_04{},
  m_z0_tubeHitFrac_03_04{},
  m_pt_tubeHitFrac_0_03{}, m_eta_tubeHitFrac_0_03{}, m_phi_tubeHitFrac_0_03{}, m_d0_tubeHitFrac_0_03{},
  m_z0_tubeHitFrac_0_03{},

  m_pt_vs_nTrtTubeHits{}, m_eta_vs_nTrtTubeHits{}, m_phi_vs_nTrtTubeHits{}, m_d0_vs_nTrtTubeHits{},
  m_z0_vs_nTrtTubeHits{}, m_mu_vs_nTrtTubeHits{}, m_nPV_vs_nTrtTubeHits{},
  m_pt_vs_nTrtHits{}, m_eta_vs_nTrtHits{}, m_phi_vs_nTrtHits{}, m_d0_vs_nTrtHits{}, m_z0_vs_nTrtHits{},
  m_mu_vs_nTrtHits{}, m_nPV_vs_nTrtHits{},
  m_pt_vs_tubeHitFrac{}, m_eta_vs_tubeHitFrac{}, m_phi_vs_tubeHitFrac{}, m_d0_vs_tubeHitFrac{}, m_z0_vs_tubeHitFrac{},
  m_mu_vs_tubeHitFrac{}, m_nPV_vs_tubeHitFrac{},
  m_nTrtHit_vs_nTrtTubeHit{}, m_nTrtHit_vs_trtTubeFrac{},
  m_d0_vs_eta{} {
  // Filled with nothing (?)
}

void
InDetPerfPlot_TrtTest::initializePlots() {
  book(m_mu, "mu");
  book(m_nPV, "nPV");
  book(m_matchProb, "matchProb");
  book(m_trtTubeFrac, "trtTubeFrac");
  book(m_trackCategories, "trackCategories");
  book(m_slimTrackCategories, "slimTrackCategories");
  book(m_pt, "pt");
  book(m_pt_ext, "pt_ext");
  book(m_pt_trtHit, "pt_trtHit");
  book(m_pt_trtHitNoOut, "pt_trtHitNoOut");
  book(m_pt_trtOutlier, "pt_trtOutlier");
  book(m_pt_trtHitOut, "pt_trtHitOut");
  book(m_pt_noTrt, "pt_noTrt");
  book(m_eta, "eta");
  book(m_eta_ext, "eta_ext");
  book(m_eta_trtHit, "eta_trtHit");
  book(m_eta_trtHitNoOut, "eta_trtHitNoOut");
  book(m_eta_trtOutlier, "eta_trtOutlier");
  book(m_eta_trtHitOut, "eta_trtHitOut");
  book(m_eta_noTrt, "eta_noTrt");
  book(m_phi, "phi");
  book(m_phi_ext, "phi_ext");
  book(m_phi_trtHit, "phi_trtHit");
  book(m_phi_trtHitNoOut, "phi_trtHitNoOut");
  book(m_phi_trtOutlier, "phi_trtOutlier");
  book(m_phi_trtHitOut, "phi_trtHitOut");
  book(m_phi_noTrt, "phi_noTrt");
  book(m_d0, "d0");
  book(m_d0_ext, "d0_ext");
  book(m_d0_trtHit, "d0_trtHit");
  book(m_d0_trtHitNoOut, "d0_trtHitNoOut");
  book(m_d0_trtOutlier, "d0_trtOutlier");
  book(m_d0_trtHitOut, "d0_trtHitOut");
  book(m_d0_noTrt, "d0_noTrt");
  book(m_z0, "z0");
  book(m_z0_ext, "z0_ext");
  book(m_z0_trtHit, "z0_trtHit");
  book(m_z0_trtHitNoOut, "z0_trtHitNoOut");
  book(m_z0_trtOutlier, "z0_trtOutlier");
  book(m_z0_trtHitOut, "z0_trtHitOut");
  book(m_z0_noTrt, "z0_noTrt");
  book(m_tube, "tube");
  book(m_tube_ext, "tube_ext");
  book(m_tube_trtHit, "tube_trtHit");
  book(m_tube_trtHitNoOut, "tube_trtHitNoOut");
  book(m_tube_trtOutlier, "tube_trtOutlier");
  book(m_tube_trtHitOut, "tube_trtHitOut");
  book(m_tube_noTrt, "tube_noTrt");
  book(m_pt_vs_nTrtTubeHits, "pt_vs_nTrtTubeHits");
  book(m_eta_vs_nTrtTubeHits, "eta_vs_nTrtTubeHits");
  book(m_phi_vs_nTrtTubeHits, "phi_vs_nTrtTubeHits");
  book(m_d0_vs_nTrtTubeHits, "d0_vs_nTrtTubeHits");
  book(m_z0_vs_nTrtTubeHits, "z0_vs_nTrtTubeHits");
  book(m_mu_vs_nTrtTubeHits, "mu_vs_nTrtTubeHits");
  book(m_nPV_vs_nTrtTubeHits, "nPV_vs_nTrtTubeHits");
  book(m_pt_vs_nTrtHits, "pt_vs_nTrtHits");
  book(m_eta_vs_nTrtHits, "eta_vs_nTrtHits");
  book(m_phi_vs_nTrtHits, "phi_vs_nTrtHits");
  book(m_d0_vs_nTrtHits, "d0_vs_nTrtHits");
  book(m_z0_vs_nTrtHits, "z0_vs_nTrtHits");
  book(m_mu_vs_nTrtHits, "mu_vs_nTrtHits");
  book(m_nPV_vs_nTrtHits, "nPV_vs_nTrtHits");
  book(m_pt_vs_tubeHitFrac, "pt_vs_tubeHitFraction");
  book(m_eta_vs_tubeHitFrac, "eta_vs_tubeHitFraction");
  book(m_phi_vs_tubeHitFrac, "phi_vs_tubeHitFraction");
  book(m_d0_vs_tubeHitFrac, "d0_vs_tubeHitFraction");
  book(m_z0_vs_tubeHitFrac, "z0_vs_tubeHitFraction");
  book(m_mu_vs_tubeHitFrac, "mu_vs_tubeHitFraction");
  book(m_nPV_vs_tubeHitFrac, "nPV_vs_tubeHitFraction");
  book(m_nTrtHit_vs_nTrtTubeHit, "nTrtHit_vs_nTrtTubeHit");
  book(m_nTrtHit_vs_trtTubeFrac, "nTrtHit_vs_trtTubeFrac");
  book(m_d0_vs_eta, "d0_vs_eta");
}

void
InDetPerfPlot_TrtTest::fill(const xAOD::TrackParticle& particle, const xAOD::EventInfo& ei,
                            const xAOD::VertexContainer& pv) {
  uint8_t iTrtHits(0), iTrtOutliers(0), iTrtTubeHits(0);

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternReco = particle.patternRecoInfo();
  float prob = getMatchingProbability(particle);
  float mu = ei.actualInteractionsPerCrossing();
  int nPV = pv.size();
  double pt = particle.pt() * 0.001;
  ;
  double eta = particle.eta();
  double phi = particle.phi();
  double d0 = particle.d0();
  double z0 = particle.z0();
  //  double theta = particle.theta();
  //  double rFirst = particle.radiusOfFirstHit();
  int nTubeHits(0), nTrtHits(0);
  bool TRTHit = false;
  bool TRTOut = false;
  bool Ext = false;
  bool SiSd = false;
  bool TRTSd = false;
  bool TRTStdAl = false;
  bool Ambi = false;
  bool LRT = false;
  double tubeFrac = -1.0;

  if (patternReco.test(0)) {
    SiSd = true;
  }
  if (patternReco.test(3)) {
    Ext = true;
  }
  if (patternReco.test(4)) {
    TRTSd = true;
  }
  if (patternReco.test(19)) {
    Ambi = true;
  }
  if (patternReco.test(20)) {
    TRTStdAl = true;
  }
  if (patternReco.test(49)) {
    LRT = true;
  }
  if (particle.summaryValue(iTrtHits, xAOD::numberOfTRTHits)) {
    if (iTrtHits > 0) {
      TRTHit = true;
      nTrtHits = (int) iTrtHits;
    }
  }
  if (particle.summaryValue(iTrtOutliers, xAOD::numberOfTRTOutliers)) {
    if (iTrtOutliers > 0) {
      TRTOut = true;
    }
  }
  if (particle.summaryValue(iTrtTubeHits, xAOD::numberOfTRTTubeHits)) {
    if (iTrtTubeHits > 0) {
      nTubeHits = (int) iTrtTubeHits;
    }
  }

  if (TRTHit) {
    tubeFrac = ((double) nTubeHits) / ((double) nTrtHits);
  }
  // std::cout << "nTrtHits is " << nTrtHits << ", nTrtTubeHits is " << nTubeHits << ", and the fraction of trt tube
  // hits is " << tubeFrac << std::endl;

  // Fill histograms for matching probability and µ, other general things//
  fillHisto(m_mu, mu);
  fillHisto(m_nPV, nPV);
  fillHisto(m_matchProb, prob);

  if (TRTHit) {
    fillHisto(m_trtTubeFrac, tubeFrac);
  }

  // Fill histograms for track categories//
  fillHisto(m_trackCategories, 1);
  if (SiSd || Ambi) {
    fillHisto(m_trackCategories, 2);
  }
  if (SiSd || Ambi || Ext) {
    fillHisto(m_trackCategories, 3);
  }
  if ((SiSd || Ambi) && Ext) {
    fillHisto(m_trackCategories, 4);
  }
  if (LRT) {
    fillHisto(m_trackCategories, 5);
  }
  if (LRT && Ext) {
    fillHisto(m_trackCategories, 6);
  }
  if (LRT && TRTHit) {
    fillHisto(m_trackCategories, 7);
  }
  if (LRT && TRTOut) {
    fillHisto(m_trackCategories, 8);
  }
  if (Ext && !TRTHit && !TRTOut) {
    fillHisto(m_trackCategories, 9);
  }
  if ((SiSd || Ambi) && LRT) {
    fillHisto(m_trackCategories, 10);
  }
  if (TRTHit && !Ext) {
    fillHisto(m_trackCategories, 11);
  }
  if (TRTOut && !Ext) {
    fillHisto(m_trackCategories, 12);
  }
  if (TRTOut && !Ext && !TRTSd && !TRTStdAl) {
    fillHisto(m_trackCategories, 13);
  }
  if (TRTHit) {
    fillHisto(m_trackCategories, 14);
  }
  if (TRTOut) {
    fillHisto(m_trackCategories, 15);
  }

  fillHisto(m_slimTrackCategories, 1);
  if (TRTHit) {
    fillHisto(m_slimTrackCategories, 2);
  }
  if (TRTOut && !TRTHit) {
    fillHisto(m_slimTrackCategories, 3);
  }
  if (!TRTOut && !TRTHit) {
    fillHisto(m_slimTrackCategories, 4);
  }

  // Fill 1D histograms split by TRT hits//

  fillHisto(m_eta, eta);
  fillHisto(m_tube, nTubeHits);
  if (inRange(eta)) {
    fillHisto(m_pt, pt);
    fillHisto(m_phi, phi);
    fillHisto(m_d0, d0);
    fillHisto(m_z0, z0);
  }

  if (Ext) {
    fillHisto(m_eta_ext, eta);
    fillHisto(m_tube_ext, nTubeHits);
    if (inRange(eta)) {
      fillHisto(m_pt_ext, pt);
      fillHisto(m_phi_ext, phi);
      fillHisto(m_d0_ext, d0);
      fillHisto(m_z0_ext, z0);
    }
  }
  if (TRTHit) {
    fillHisto(m_eta_trtHit, eta);
    fillHisto(m_tube_trtHit, nTubeHits);
    if (inRange(eta)) {
      fillHisto(m_pt_trtHit, pt);
      fillHisto(m_eta_trtHit, eta);
      fillHisto(m_phi_trtHit, phi);
      fillHisto(m_d0_trtHit, d0);
      fillHisto(m_z0_trtHit, z0);
    }
  }
  if (TRTHit && !TRTOut) {
    fillHisto(m_eta_trtHitNoOut, eta);
    fillHisto(m_tube_trtHitNoOut, nTubeHits);
    if (inRange(eta)) {
      fillHisto(m_pt_trtHitNoOut, pt);
      fillHisto(m_phi_trtHitNoOut, phi);
      fillHisto(m_d0_trtHitNoOut, d0);
      fillHisto(m_z0_trtHitNoOut, z0);
    }
  }
  if (TRTHit && TRTOut) {
    fillHisto(m_eta_trtHitOut, eta);
    fillHisto(m_tube_trtHitOut, nTubeHits);
    if (inRange(eta)) {
      fillHisto(m_pt_trtHitOut, pt);
      fillHisto(m_phi_trtHitOut, phi);
      fillHisto(m_d0_trtHitOut, d0);
      fillHisto(m_z0_trtHitOut, z0);
    }
  }
  if (!TRTHit && TRTOut) {
    fillHisto(m_eta_trtOutlier, eta);
    fillHisto(m_tube_trtOutlier, nTubeHits);
    if (inRange(eta)) {
      fillHisto(m_pt_trtOutlier, pt);
      fillHisto(m_phi_trtOutlier, phi);
      fillHisto(m_d0_trtOutlier, d0);
      fillHisto(m_z0_trtOutlier, z0);
    }
  }
  if (!TRTHit && !TRTOut) {
    fillHisto(m_eta_noTrt, eta);
    fillHisto(m_tube_noTrt, nTubeHits);
    if (inRange(eta)) {
      fillHisto(m_pt_noTrt, pt);
      fillHisto(m_phi_noTrt, phi);
      fillHisto(m_d0_noTrt, d0);
      fillHisto(m_z0_noTrt, z0);
    }
  }

  // Fill 2D histograms//
  // general things//
  fillHisto(m_d0_vs_eta, d0, eta);

  // trt things
  fillHisto(m_eta_vs_nTrtTubeHits, eta, nTubeHits);
  fillHisto(m_mu_vs_nTrtTubeHits, mu, nTubeHits);
  fillHisto(m_nPV_vs_nTrtTubeHits, nPV, nTubeHits);
  fillHisto(m_eta_vs_nTrtHits, eta, nTrtHits);
  fillHisto(m_mu_vs_nTrtHits, mu, nTrtHits);
  fillHisto(m_nPV_vs_nTrtHits, nPV, nTrtHits);
  if (inRange(eta)) {
    fillHisto(m_pt_vs_nTrtTubeHits, pt, nTubeHits);
    fillHisto(m_phi_vs_nTrtTubeHits, phi, nTubeHits);
    fillHisto(m_d0_vs_nTrtTubeHits, d0, nTubeHits);
    fillHisto(m_z0_vs_nTrtTubeHits, z0, nTubeHits);
    fillHisto(m_pt_vs_nTrtHits, pt, nTrtHits);
    fillHisto(m_phi_vs_nTrtHits, phi, nTrtHits);
    fillHisto(m_d0_vs_nTrtHits, d0, nTrtHits);
    fillHisto(m_z0_vs_nTrtHits, z0, nTrtHits);
  }


  if (TRTHit && !TRTStdAl && !TRTSd) {
    fillHisto(m_nTrtHit_vs_nTrtTubeHit, nTrtHits, nTubeHits);
    fillHisto(m_nTrtHit_vs_trtTubeFrac, nTrtHits, tubeFrac);
    fillHisto(m_eta_vs_tubeHitFrac, eta, tubeFrac);
    fillHisto(m_mu_vs_tubeHitFrac, mu, tubeFrac);
    fillHisto(m_nPV_vs_tubeHitFrac, nPV, tubeFrac);
    if (inRange(eta)) {
      fillHisto(m_pt_vs_tubeHitFrac, pt, tubeFrac);
      fillHisto(m_phi_vs_tubeHitFrac, phi, tubeFrac);
      fillHisto(m_d0_vs_tubeHitFrac, d0, tubeFrac);
      fillHisto(m_z0_vs_tubeHitFrac, z0, tubeFrac);
    }
  }
}

void
InDetPerfPlot_TrtTest::finalizePlots() {
}
