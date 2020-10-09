/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_TrkInJet.cxx
 * @author Gabrel Facini <Gabriel.Facini@cern.ch>
 * Oct 23 15:20:45 CEST 2014
 **/

#include "InDetPerfPlot_TrkInJet.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <algorithm>
#include <numeric>
#include "InDetPhysValMonitoringUtilities.h"

using namespace IDPVM;


InDetPerfPlot_TrkInJet::InDetPerfPlot_TrkInJet(InDetPlotBase* pParent, std::string sDir, bool bookFakeAndEff) :
  InDetPlotBase(pParent, sDir),

  m_nInnerMostPixelHits_vs_dR{},
  m_nNextToInnerMostPixelHits_vs_dR{},
  m_nPixelHits_vs_dR{},
  m_nSCTHits_vs_dR{},
  m_nTRTHits_vs_dR{},
  m_nPixelHoles_vs_dR{},
  m_nSCTHoles_vs_dR{},
  m_nTRTHighThresholdHits_vs_dR{},
  m_nInnerMostPixelOutliers_vs_dR{},
  m_nInnerMostPixelSharedHits_vs_dR{},
  m_nInnerMostPixelSplitHits_vs_dR{},
  m_nExpectedInnerMostPixelHits_vs_dR{},
  m_nExpectedNextToInnerMostPixelHits_vs_dR{},
  m_nPixelOutliers_vs_dR{},
  m_nPixelContribLayers_vs_dR{},
  m_nPixelSharedHits_vs_dR{},
  m_nPixelSplitHits_vs_dR{},
  m_nPixelGangedHits_vs_dR{},
  m_nPixelGangedHitsFlaggedFakes_vs_dR{},
  m_nSCTOutliers_vs_dR{},
  m_nSCTDoubleHoles_vs_dR{},
  m_nSCTSharedHits_vs_dR{},
  m_nTRTOutliers_vs_dR{},
  m_nTRTHighThresholdOutliers_vs_dR{},
  m_nPixelDeadSensors_vs_dR{},
  m_nSCTDeadSensors_vs_dR{},


  m_reco_d0_vs_dR{},
  m_reco_z0_vs_dR{},
  m_reco_z0sin_vs_dR{},

  m_reco_phi_vs_dR{},
  m_reco_theta_vs_dR{},
  m_reco_eta_vs_dR{},
  m_reco_pt_vs_dR{},
  m_reco_lowpt_vs_dR{},

  m_reco_chi2Overndof_vs_dR{},

  m_efficiency_vs_dR{},
  m_efficiency_vs_smalldR{},
  m_efficiency_vs_jetpT{},

  m_fakerate_vs_dR{},
  m_fakerate_vs_smalldR{},
  m_fakerate_vs_jetpT{},
  m_doFakeAndEff(bookFakeAndEff) {

  //nop

}

void
InDetPerfPlot_TrkInJet::initializePlots() {

  //Hits vs dR
  book(m_nInnerMostPixelHits_vs_dR, "nInnerMostPixelHits_vs_dR");
  book(m_nNextToInnerMostPixelHits_vs_dR, "nNextToInnerMostPixelHits_vs_dR");
  book(m_nPixelHits_vs_dR, "nPixelHits_vs_dR");
  book(m_nSCTHits_vs_dR, "nSCTHits_vs_dR");
  book(m_nTRTHits_vs_dR, "nTRTHits_vs_dR");
  book(m_nPixelHoles_vs_dR, "nPixelHoles_vs_dR");
  book(m_nSCTHoles_vs_dR, "nSCTHoles_vs_dR");
  book(m_nTRTHighThresholdHits_vs_dR, "nTRTHighThresholdHits_vs_dR");
  if (m_iDetailLevel >= 100){
    book(m_nInnerMostPixelOutliers_vs_dR, "nInnerMostPixelOutliers_vs_dR");
    book(m_nInnerMostPixelSharedHits_vs_dR, "nInnerMostPixelSharedHits_vs_dR");
    book(m_nInnerMostPixelSplitHits_vs_dR, "nInnerMostPixelSplitHits_vs_dR");
    book(m_nExpectedInnerMostPixelHits_vs_dR, "nExpectedInnerMostPixelHits_vs_dR");
    book(m_nExpectedNextToInnerMostPixelHits_vs_dR, "nExpectedNextToInnerMostPixelHits_vs_dR");
    book(m_nPixelOutliers_vs_dR, "nPixelOutliers_vs_dR");
    book(m_nPixelContribLayers_vs_dR, "nPixelContribLayers_vs_dR");
    book(m_nPixelSharedHits_vs_dR, "nPixelSharedHits_vs_dR");
    book(m_nPixelSplitHits_vs_dR, "nPixelSplitHits_vs_dR");
    book(m_nPixelGangedHits_vs_dR, "nPixelGangedHits_vs_dR");
    book(m_nPixelGangedHitsFlaggedFakes_vs_dR, "nPixelGangedHitsFlaggedFakes_vs_dR");
    book(m_nSCTOutliers_vs_dR, "nSCTOutliers_vs_dR");
    book(m_nSCTDoubleHoles_vs_dR, "nSCTDoubleHoles_vs_dR");
    book(m_nSCTSharedHits_vs_dR, "nSCTSharedHits_vs_dR");
    book(m_nTRTOutliers_vs_dR, "nTRTOutliers_vs_dR");
    book(m_nTRTHighThresholdOutliers_vs_dR, "nTRTHighThresholdOutliers_vs_dR");
    book(m_nPixelDeadSensors_vs_dR, "nPixelDeadSensors_vs_dR");
    book(m_nSCTDeadSensors_vs_dR, "nSCTDeadSensors_vs_dR");
  }


  book(m_reco_d0_vs_dR, "reco_d0_vs_dR");
  book(m_reco_z0_vs_dR, "reco_z0_vs_dR");
  book(m_reco_z0sin_vs_dR, "reco_z0sin_vs_dR");

  book(m_reco_phi_vs_dR, "reco_phi_vs_dR");
  book(m_reco_theta_vs_dR, "reco_theta_vs_dR");
  book(m_reco_eta_vs_dR, "reco_eta_vs_dR");
  book(m_reco_pt_vs_dR, "reco_pt_vs_dR");
  book(m_reco_lowpt_vs_dR, "reco_lowpt_vs_dR");

  book(m_reco_chi2Overndof_vs_dR, "reco_chi2Overndof_vs_dR");
  if (m_doFakeAndEff){
    book(m_efficiency_vs_dR, "efficiency_vs_dR");
    book(m_efficiency_vs_smalldR, "efficiency_vs_smalldR");
    book(m_efficiency_vs_jetpT, "efficiency_vs_jetpT");

    book(m_fakerate_vs_dR, "fakerate_vs_dR");
    book(m_fakerate_vs_smalldR, "fakerate_vs_smalldR");
    book(m_fakerate_vs_jetpT, "fakerate_vs_jetpT");
  }

}


void
InDetPerfPlot_TrkInJet::fill(const xAOD::TrackParticle& trk, const xAOD::Jet& jet) {
  
  float dR = jet.p4().DeltaR(trk.p4());

  uint8_t iInnerMostPixelHits(0), iNextToInnerMostPixelHits(0), iPixHits(0), iSctHits(0), iTrtHits(0);
  uint8_t iPixHoles(0), iSCTHoles(0), iTrtHTHits(0);
  if (trk.summaryValue(iInnerMostPixelHits, xAOD::numberOfInnermostPixelLayerHits)) {
    fillHisto(m_nInnerMostPixelHits_vs_dR, dR, iInnerMostPixelHits);
  }
  if (trk.summaryValue(iNextToInnerMostPixelHits, xAOD::numberOfNextToInnermostPixelLayerHits)) {
    fillHisto(m_nNextToInnerMostPixelHits_vs_dR, dR, iNextToInnerMostPixelHits);
  }
  if (trk.summaryValue(iPixHits, xAOD::numberOfPixelHits)) {
    fillHisto(m_nPixelHits_vs_dR, dR, iPixHits);
  }
  if (trk.summaryValue(iSctHits, xAOD::numberOfSCTHits)) {
    fillHisto(m_nSCTHits_vs_dR, dR, iSctHits);
  }
  if (trk.summaryValue(iTrtHits, xAOD::numberOfTRTHits)) {
    fillHisto(m_nTRTHits_vs_dR, dR, iTrtHits);
  }
  if (trk.summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
    fillHisto(m_nPixelHoles_vs_dR, dR, iPixHoles);
  }
  if (trk.summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
    fillHisto(m_nSCTHoles_vs_dR, dR, iSCTHoles);
  }
  if (trk.summaryValue(iTrtHTHits, xAOD::numberOfTRTHighThresholdHits)) {
    fillHisto(m_nTRTHighThresholdHits_vs_dR, dR, iTrtHTHits);
  }
  // expert plots
  if (m_iDetailLevel >= 100) {
    uint8_t iInnerMostPixelOutliers(0), iInnerMostPixelShared(0), iInnerMostPixelSplit(0);
    uint8_t iExpectedInnerMostPixelHits(0), iExpectedNextToInnerMostPixelHits(0);
    uint8_t iPixelOutliers(0), iPixelContribLayers(0), iPixelShared(0), iPixelSplit(0), iPixelGanged(0), iPixelGangedFakes(0);
    uint8_t iSCTOutliers(0), iSCTDoubleHoles(0), iSCTShared(0);
    uint8_t iTRTOutliers(0), iTRTHTOutliers(0);
    uint8_t iPixelDeadSensors(0), iSCTDeadSensors(0);

    if (trk.summaryValue(iInnerMostPixelOutliers, xAOD::numberOfInnermostPixelLayerOutliers)) {
      fillHisto(m_nInnerMostPixelOutliers_vs_dR, dR, iInnerMostPixelOutliers);
    }
    if (trk.summaryValue(iInnerMostPixelShared, xAOD::numberOfInnermostPixelLayerSharedHits)) {
      fillHisto(m_nInnerMostPixelSharedHits_vs_dR, dR, iInnerMostPixelShared);
    }
    if (trk.summaryValue(iInnerMostPixelSplit, xAOD::numberOfInnermostPixelLayerSplitHits)) {
      fillHisto(m_nInnerMostPixelSplitHits_vs_dR, dR, iInnerMostPixelSplit);
    }
    if (trk.summaryValue(iExpectedInnerMostPixelHits, xAOD::expectInnermostPixelLayerHit)) {
      fillHisto(m_nExpectedInnerMostPixelHits_vs_dR, dR, iExpectedInnerMostPixelHits);
    }
    if (trk.summaryValue(iExpectedNextToInnerMostPixelHits, xAOD::expectNextToInnermostPixelLayerHit)) {
      fillHisto(m_nExpectedNextToInnerMostPixelHits_vs_dR, dR, iExpectedNextToInnerMostPixelHits);
    }
    if (trk.summaryValue(iPixelOutliers, xAOD::numberOfPixelOutliers)) {
      fillHisto(m_nPixelOutliers_vs_dR, dR, iPixelOutliers);
    }
    if (trk.summaryValue(iPixelContribLayers, xAOD::numberOfContribPixelLayers)) {
      fillHisto(m_nPixelContribLayers_vs_dR, dR, iPixelContribLayers);
    }
    if (trk.summaryValue(iPixelShared, xAOD::numberOfPixelSharedHits)) {
      fillHisto(m_nPixelSharedHits_vs_dR, dR, iPixelShared);
    }
    if (trk.summaryValue(iPixelSplit, xAOD::numberOfPixelSplitHits)) {
      fillHisto(m_nPixelSplitHits_vs_dR, dR, iPixelSplit);
    }
    if (trk.summaryValue(iPixelGanged, xAOD::numberOfGangedPixels)) {
      fillHisto(m_nPixelGangedHits_vs_dR, dR, iPixelGanged);
    }
    if (trk.summaryValue(iPixelGangedFakes, xAOD::numberOfGangedFlaggedFakes)) {
      fillHisto(m_nPixelGangedHitsFlaggedFakes_vs_dR, dR, iPixelGangedFakes);
    }
    if (trk.summaryValue(iSCTOutliers, xAOD::numberOfSCTOutliers)) {
      fillHisto(m_nSCTOutliers_vs_dR, dR, iSCTOutliers);
    }
    if (trk.summaryValue(iSCTDoubleHoles, xAOD::numberOfSCTDoubleHoles)) {
      fillHisto(m_nSCTDoubleHoles_vs_dR, dR, iSCTDoubleHoles);
    }
    if (trk.summaryValue(iSCTShared, xAOD::numberOfSCTSharedHits)) {
      fillHisto(m_nSCTSharedHits_vs_dR, dR, iSCTShared);
    }
    if (trk.summaryValue(iTRTOutliers, xAOD::numberOfTRTOutliers)) {
      fillHisto(m_nTRTOutliers_vs_dR, dR, iTRTOutliers);
    }
    if (trk.summaryValue(iTRTHTOutliers, xAOD::numberOfTRTHighThresholdOutliers)) {
      fillHisto(m_nTRTHighThresholdOutliers_vs_dR, dR, iTRTHTOutliers);
    }
    if (trk.summaryValue(iPixelDeadSensors, xAOD::numberOfPixelDeadSensors)) {
      fillHisto(m_nPixelDeadSensors_vs_dR, dR, iPixelDeadSensors);
    }
    if (trk.summaryValue(iSCTDeadSensors, xAOD::numberOfSCTDeadSensors)) {
      fillHisto(m_nSCTDeadSensors_vs_dR, dR, iSCTDeadSensors);
    }

  }

  //Track Parameters
  float pt = trk.pt() / Gaudi::Units::GeV;
  float eta = trk.eta();
  float phi = trk.phi0();
  
  float chi2 = trk.chiSquared();
  float ndof = trk.numberDoF();
  float chi2Overndof = ndof > 0 ? chi2 / ndof : 0;
  

  fillHisto(m_reco_d0_vs_dR, dR, trk.d0());
  fillHisto(m_reco_z0_vs_dR, dR, trk.z0());
  fillHisto(m_reco_z0sin_vs_dR, dR, trk.z0()* trk.theta());
  fillHisto(m_reco_phi_vs_dR, dR, phi);
  fillHisto(m_reco_theta_vs_dR, dR, trk.theta());
  fillHisto(m_reco_eta_vs_dR, dR, eta);
  fillHisto(m_reco_pt_vs_dR, dR, pt);
  fillHisto(m_reco_lowpt_vs_dR, dR, pt);
  fillHisto(m_reco_chi2Overndof_vs_dR, dR, chi2Overndof);

}

void
InDetPerfPlot_TrkInJet::fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::Jet& jet, const bool isGood) {
  if (!m_doFakeAndEff) return; 
  float dR = jet.p4().DeltaR(truth.p4());
  float jetpT = jet.pt() / Gaudi::Units::GeV;
  fillHisto(m_efficiency_vs_dR, dR, isGood);
  fillHisto(m_efficiency_vs_smalldR, dR, isGood);
  fillHisto(m_efficiency_vs_jetpT, jetpT, isGood);
}


void
InDetPerfPlot_TrkInJet::fillFakeRate(const xAOD::TrackParticle& trk, const xAOD::Jet& jet, const bool isFake) {
  if (!m_doFakeAndEff) return; 
  float dR = jet.p4().DeltaR(trk.p4());
  float jetpT = jet.pt() / Gaudi::Units::GeV;

  fillHisto(m_fakerate_vs_dR, dR, isFake);
  fillHisto(m_fakerate_vs_smalldR, dR, isFake);
  fillHisto(m_fakerate_vs_jetpT, jetpT, isFake);
}
