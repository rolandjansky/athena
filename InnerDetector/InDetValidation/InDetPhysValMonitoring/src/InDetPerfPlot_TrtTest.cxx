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

// get truth/track matching probability
float
getMatchingProbability(const xAOD::TrackParticle &trackParticle) {
  float result(std::numeric_limits<float>::quiet_NaN());

  if (trackParticle.isAvailable<float>("truthMatchProbability")) {
    result = trackParticle.auxdata<float>("truthMatchProbability");
  }
  return result;
}

InDetPerfPlot_TrtTest::InDetPerfPlot_TrtTest(InDetPlotBase *pParent, const std::string &sDir) : InDetPlotBase(pParent,
                                                                                                              sDir),
  m_matchProb{}, m_trackCategories{}, m_ld0TrackCategories{},
  m_pt_lrt{}, m_pt_lrtExt{}, m_pt_lrtTrtHit{}, m_pt_lrtTrtOutlier{}, m_pt_lrtTrtHitOut{},
  m_eta_lrt{}, m_eta_lrtExt{}, m_eta_lrtTrtHit{}, m_eta_lrtTrtOutlier{}, m_eta_lrtTrtHitOut{},
  m_phi_lrt{}, m_phi_lrtExt{}, m_phi_lrtTrtHit{}, m_phi_lrtTrtOutlier{}, m_phi_lrtTrtHitOut{},
  m_d0_lrt{}, m_d0_lrtExt{}, m_d0_lrtTrtHit{}, m_d0_lrtTrtOutlier{}, m_d0_lrtTrtHitOut{},
  m_z0_lrt{}, m_z0_lrtExt{}, m_z0_lrtTrtHit{}, m_z0_lrtTrtOutlier{}, m_z0_lrtTrtHitOut{} {
  // Filled with nothing (?)
}

void
InDetPerfPlot_TrtTest::initializePlots() {
  book(m_matchProb, "matchProb");
  book(m_trackCategories, "trackCategories");
  book(m_ld0TrackCategories, "ld0TrackCategories");
  book(m_pt_lrt, "pt_lrt");
  book(m_pt_lrtExt, "pt_lrtExt");
  book(m_pt_lrtTrtHit, "pt_lrtTrtHit");
  book(m_pt_lrtTrtOutlier, "pt_lrtTrtOutlier");
  book(m_pt_lrtTrtHitOut, "pt_lrtTrtHitOut");
  book(m_eta_lrt, "eta_lrt");
  book(m_eta_lrtExt, "eta_lrtExt");
  book(m_eta_lrtTrtHit, "eta_lrtTrtHit");
  book(m_eta_lrtTrtOutlier, "eta_lrtTrtOutlier");
  book(m_eta_lrtTrtHitOut, "eta_lrtTrtHitOut");
  book(m_phi_lrt, "phi_lrt");
  book(m_phi_lrtExt, "phi_lrtExt");
  book(m_phi_lrtTrtHit, "phi_lrtTrtHit");
  book(m_phi_lrtTrtOutlier, "phi_lrtTrtOutlier");
  book(m_phi_lrtTrtHitOut, "phi_lrtTrtHitOut");
  book(m_d0_lrt, "d0_lrt");
  book(m_d0_lrtExt, "d0_lrtExt");
  book(m_d0_lrtTrtHit, "d0_lrtTrtHit");
  book(m_d0_lrtTrtOutlier, "d0_lrtTrtOutlier");
  book(m_d0_lrtTrtHitOut, "d0_lrtTrtHitOut");
  book(m_z0_lrt, "z0_lrt");
  book(m_z0_lrtExt, "z0_lrtExt");
  book(m_z0_lrtTrtHit, "z0_lrtTrtHit");
  book(m_z0_lrtTrtOutlier, "z0_lrtTrtOutlier");
  book(m_z0_lrtTrtHitOut, "z0_lrtTrtHitOut");
}

void
InDetPerfPlot_TrtTest::fill(const xAOD::TrackParticle &particle) {
  uint8_t iTrtHits(0), iTrtOutliers(0);
  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternReco = particle.patternRecoInfo();
  float prob = getMatchingProbability(particle);
  double pt = particle.pt() / 1000.;
  double eta = particle.eta();
  double phi = particle.phi();
  double d0 = particle.d0();
  double z0 = particle.z0();
  bool TRTHit = false;
  bool TRTOut = false;
  bool Ext = false;
  bool SiSd = false;
  bool TRTSd = false;
  bool TRTStdAl = false;
  bool Ambi = false;
  bool LRT = false;

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
    }
  }
  if (particle.summaryValue(iTrtOutliers, xAOD::numberOfTRTOutliers)) {
    if (iTrtOutliers > 0) {
      TRTOut = true;
    }
  }


  m_matchProb->Fill(prob);

  m_trackCategories->Fill(1);
  if (SiSd || Ambi) {
    m_trackCategories->Fill(2);
  }
  if (SiSd || Ambi || Ext) {
    m_trackCategories->Fill(3);
  }
  if ((SiSd || Ambi) && Ext) {
    m_trackCategories->Fill(4);
  }
  if (LRT) {
    m_trackCategories->Fill(5);
  }
  if (LRT && Ext) {
    m_trackCategories->Fill(6);
  }
  if (LRT && TRTHit) {
    m_trackCategories->Fill(7);
  }
  if (LRT && TRTOut) {
    m_trackCategories->Fill(8);
  }
  if (Ext && !TRTHit && !TRTOut) {
    m_trackCategories->Fill(9);
  }
  if ((SiSd || Ambi) && LRT) {
    m_trackCategories->Fill(10);
  }
  if (TRTHit && !Ext) {
    m_trackCategories->Fill(11);
  }
  if (TRTOut && !Ext) {
    m_trackCategories->Fill(12);
  }
  if (TRTOut && !Ext && !TRTSd && !TRTStdAl) {
    m_trackCategories->Fill(13);
  }
  if (TRTHit) {
    m_trackCategories->Fill(14);
  }
  if (TRTOut) {
    m_trackCategories->Fill(15);
  }

  if (LRT) {
    m_ld0TrackCategories->Fill(1);
  }
  if (LRT && TRTHit) {
    m_ld0TrackCategories->Fill(2);
  }
  if (LRT && TRTOut && !TRTHit) {
    m_ld0TrackCategories->Fill(3);
  }
  if (LRT && !TRTOut && !TRTHit) {
    m_ld0TrackCategories->Fill(4);
  }

  if (LRT) {
    m_pt_lrt->Fill(pt);
    m_eta_lrt->Fill(eta);
    m_phi_lrt->Fill(phi);
    m_d0_lrt->Fill(d0);
    m_z0_lrt->Fill(z0);
  }
  if (LRT && Ext) {
    m_pt_lrtExt->Fill(pt);
    m_eta_lrtExt->Fill(eta);
    m_phi_lrtExt->Fill(phi);
    m_d0_lrtExt->Fill(d0);
    m_z0_lrtExt->Fill(z0);
  }
  if (LRT && TRTHit && !TRTOut) {
    m_pt_lrtTrtHit->Fill(pt);
    m_eta_lrtTrtHit->Fill(eta);
    m_phi_lrtTrtHit->Fill(phi);
    m_d0_lrtTrtHit->Fill(d0);
    m_z0_lrtTrtHit->Fill(z0);
  }
  if (LRT && TRTHit && TRTOut) {
    m_pt_lrtTrtHitOut->Fill(pt);
    m_eta_lrtTrtHitOut->Fill(eta);
    m_phi_lrtTrtHitOut->Fill(phi);
    m_d0_lrtTrtHitOut->Fill(d0);
    m_z0_lrtTrtHitOut->Fill(z0);
  }
  if (LRT && !TRTHit && TRTOut) {
    m_pt_lrtTrtOutlier->Fill(pt);
    m_eta_lrtTrtOutlier->Fill(eta);
    m_phi_lrtTrtOutlier->Fill(phi);
    m_d0_lrtTrtOutlier->Fill(d0);
    m_z0_lrtTrtOutlier->Fill(z0);
  }
}

void
InDetPerfPlot_TrtTest::finalizePlots() {
}
