/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_Pt.cxx
 * @author shaun roe
 **/

#include "InDetPerfPlot_duplicate.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"


InDetPerfPlot_duplicate::InDetPerfPlot_duplicate(InDetPlotBase *pParent, const std::string &sDir) : InDetPlotBase(
    pParent, sDir),
  m_duplicateDeltaPt{},
  m_duplicateDeltaPtZoomed{},
  m_duplicateDeltaEta{},
  m_duplicateDeltaPhi{},
  m_duplicateLPTvsHPT{},
  m_duplicateLEtavsHEta{},
  m_duplicateLPhivsHPhi{},
  m_duplicateDeltaPTvsTruthPT{},
  m_duplicateDeltaPTvsTruthPTZoomed{},

  m_duplicateLPT{},
  m_duplicateLEta{},
  m_duplicateLPhi{},
  m_duplicateHPT{},
  m_duplicateHEta{},
  m_duplicateHPhi{},
  m_duplicateTruthPT{},
  m_duplicateTruthEta{},
  m_duplicateTruthPhi{},

  m_duplicateResLPT{},
  m_duplicateResLEta{},
  m_duplicateResLPhi{},
  m_duplicateResHPT{},
  m_duplicateResHEta{},
  m_duplicateResHPhi{},

  m_duplicateLPTHoles{},
  m_duplicateHPTHoles{},
  m_singleMatchPTHoles{},
  m_duplicateProbSpectrum{},
  m_duplicateHPixelvsSCTShared{},
  m_duplicateLPixelvsSCTShared{},
  m_twoMatchHPixelvsSCTShared{},
  m_twoMatchLPixelvsSCTShared{},
  m_singleMatchPixelvsSCTShared{} {
}

void
InDetPerfPlot_duplicate::initializePlots() {
  const bool prependDirectory(false);

  m_duplicateDeltaPt = Book1D("duplicateDeltaPt", "p_{T} difference of duplicate reco tracks (in GeV);p_{T}(GeV/c)", 50,
                              0., 50, prependDirectory);
  m_duplicateDeltaPtZoomed = Book1D("duplicateDeltaPtZoomed",
                                    "p_{T} difference of duplicate reco tracks (in GeV);p_{T}(GeV/c)", 50, 0., 5,
                                    prependDirectory);
  m_duplicateDeltaEta = Book1D("duplicateDeltaEta", "eta difference of duplicate reco tracks", 60, -3, 3,
                               prependDirectory);
  m_duplicateDeltaPhi = Book1D("duplicateDeltaPhi", "phi difference of duplicate reco tracks", 60, -3, 3,
                               prependDirectory);


  m_duplicateDeltaPTvsTruthPT = Book2D("duplicateDeltaPTvsTruthPT", "delta pt vs truth pt ", 100, 0, 50, 100, 0, 50,
                                       prependDirectory);
  m_duplicateDeltaPTvsTruthPTZoomed = Book2D("duplicateDeltaPTvsTruthPTZoomed", "delta pt vs truth pt ", 50, 0, 10, 50,
                                             0, 10, prependDirectory);



  // spectrum plots lpt = lower pt track, hpt = higher pt track, truth = truth track spectrum
  m_duplicateLPT = Book1D("duplicateLPT", "p_{T} of the duplicate lower pt reco track (in GeV);p_{T}(GeV/c)", 40, 0.,
                          40, prependDirectory);
  m_duplicateLEta = Book1D("duplicateLEta", "eta of the duplicate lower pt reco track", 60, -3, 3, prependDirectory);
  m_duplicateLPhi = Book1D("duplicateLPhi", "phi of the duplicate lower pt reco track", 60, -3, 3, prependDirectory);

  m_duplicateHPT = Book1D("duplicateHPT", "p_{T} of the duplicate higher pt reco track (in GeV);p_{T}(GeV/c)", 40, 0.,
                          40, prependDirectory);
  m_duplicateHEta = Book1D("duplicateHEta", "eta of the duplicate higher pt reco track", 60, -3, 3, prependDirectory);
  m_duplicateHPhi = Book1D("duplicateHPhi", "phi of the duplicate higher pt reco track", 60, -3, 3, prependDirectory);

  m_duplicateTruthPT = Book1D("duplicateAssociatedTruthPT", "p_{T} of the associated truth track (in GeV);p_{T}(GeV/c)",
                              40, 0., 40, prependDirectory);
  m_duplicateTruthEta = Book1D("duplicateAssociatedTruthEta", "eta of the associated truth track", 60, -3, 3,
                               prependDirectory);
  m_duplicateTruthPhi = Book1D("duplicateAssociatedTruthPhi", "phi of the associated truth track", 60, -3, 3,
                               prependDirectory);

  // resolutions hpt - truth, lpt - truth

  m_duplicateResLPT = Book1D("duplicateResLPT", "p_{T} of the lower pt track minus truth pt (in GeV);p_{T}(GeV/c)", 60,
                             -15., 15, prependDirectory);
  m_duplicateResLEta = Book1D("duplicateResLEta", "eta of the lower pt track minus truth eta", 60, -3, 3,
                              prependDirectory);
  m_duplicateResLPhi = Book1D("duplicateResLPhi", "phi of the lower pt track minus truth phi", 60, -3, 3,
                              prependDirectory);
  m_duplicateResHPT = Book1D("duplicateResHPT", "p_{T} of the higher pt track minus truth pt (in GeV);p_{T}(GeV/c)", 60,
                             -15., 15, prependDirectory);
  m_duplicateResHEta = Book1D("duplicateResHEta", "eta of the higher pt track minus truth eta", 60, -3, 3,
                              prependDirectory);
  m_duplicateResHPhi = Book1D("duplicateResHPhi", "phi of the higher pt track minus truth phi", 60, -3, 3,
                              prependDirectory);

  m_duplicateProbSpectrum = Book2D("duplicateProbSpectrum", "duplicate probability spectrum", 30, .8, 1.1, 30, .8, 1.1,
                                   prependDirectory);


  // chisq/dof
  m_duplicateLPTHoles = Book1D("duplicateLPTvsHoles", " # of holes of the lower pt track", 10, 0, 10, prependDirectory);
  m_duplicateHPTHoles =
    Book1D("duplicateHPTvsHoles", " # of holes of the higher pt track", 10, 0, 10, prependDirectory);
  m_singleMatchPTHoles = Book1D("singleMatchPTvsHoles", "# of holes of a single matched reco track", 10, 0, 10,
                                prependDirectory);

  m_duplicateHPixelvsSCTShared = Book2D("duplicateHPixelvsSCTShared",
                                        "# of Pixel Shared Hits vs # of SCT Shared Hits for the higher pt track", 20, 0,
                                        20, 20, 0, 20, prependDirectory);
  m_duplicateLPixelvsSCTShared = Book2D("duplicateLPixelvsSCTShared",
                                        "# of Pixel Shared Hits vs # of SCT Shared Hits for the lower pt track", 20, 0,
                                        20, 20, 0, 20, prependDirectory);
  m_singleMatchPixelvsSCTShared = Book2D("singleMatchPixelvsSCTShared",
                                         "# of Pixel Shared Hits vs # of SCT Shared Hits for a single matched track",
                                         20, 0, 20, 20, 0, 20, prependDirectory);
  m_twoMatchHPixelvsSCTShared = Book2D("twoMatchHPixelvsSCTShared",
                                       "# of Pixel Shared Hits vs # of SCT Shared Hits for the higher pt track", 20, 0,
                                       20, 20, 0, 20, prependDirectory);
  m_twoMatchLPixelvsSCTShared = Book2D("twoMatchLPixelvsSCTShared",
                                       "# of Pixel Shared Hits vs # of SCT Shared Hits for the lower pt track", 20, 0,
                                       20, 20, 0, 20, prependDirectory);
}

void
InDetPerfPlot_duplicate::fillSingleMatch(const xAOD::TrackParticle &trackParticle) {
  uint8_t iPixHoles, iSCTHoles, iPixShared, iSCTShared;

  int pixHoles = 0;
  int sctHoles = 0;

  int pixShared = 0;
  int sctShared = 0;

  if (trackParticle.summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
    pixHoles = iPixHoles;
  }
  if (trackParticle.summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
    sctHoles = iSCTHoles;
  }
  if (trackParticle.summaryValue(iPixShared, xAOD::numberOfPixelSharedHits)) {
    pixShared = iPixShared;
  }
  if (trackParticle.summaryValue(iSCTShared, xAOD::numberOfSCTSharedHits)) {
    sctShared = iSCTShared;
  }

  fillHisto(m_singleMatchPixelvsSCTShared,pixShared, sctShared);

  fillHisto(m_singleMatchPTHoles,sctHoles + pixHoles);
}

void
InDetPerfPlot_duplicate::fillTwoMatchDuplicate(const float prob1, const float prob2,
                                               const xAOD::TrackParticle &trackParticle,
                                               const xAOD::TrackParticle &particle, const xAOD::TruthParticle &tp) {
  float pt1(trackParticle.pt() / 1000.);
  float pt2(particle.pt() / 1000.);
  float truthPt(tp.pt() / 1000);
  float eta1(trackParticle.eta());
  float eta2(particle.eta());
  float truthEta((truthPt>1e-7)? tp.eta(): std::nanf(""));
  float phi1(trackParticle.phi());
  float phi2(particle.phi());
  float truthPhi(tp.phi());

  uint8_t iPixHoles, iSCTHoles;
  int pixHoles1 = 0;
  int sctHoles1 = 0;
  int totalHoles1 = 0;
  int pixHoles2 = 0;
  int sctHoles2 = 0;
  int totalHoles2 = 0;

  uint8_t iPixShared, iSCTShared;
  int pixShared1 = 0;
  int pixShared2 = 0;
  int sctShared1 = 0;
  int sctShared2 = 0;

  if (trackParticle.summaryValue(iPixShared, xAOD::numberOfPixelSharedHits)) {
    pixShared1 = iPixShared;
  }
  if (trackParticle.summaryValue(iSCTShared, xAOD::numberOfSCTSharedHits)) {
    sctShared1 = iSCTShared;
  }
  if (particle.summaryValue(iPixShared, xAOD::numberOfPixelSharedHits)) {
    pixShared2 = iPixShared;
  }
  if (particle.summaryValue(iSCTShared, xAOD::numberOfSCTSharedHits)) {
    sctShared2 = iSCTShared;
  }

  if (trackParticle.summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
    pixHoles1 = iPixHoles;
  }
  if (trackParticle.summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
    sctHoles1 = iSCTHoles;
  }
  if (particle.summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
    pixHoles2 = iPixHoles;
  }
  if (particle.summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
    sctHoles2 = iSCTHoles;
  }
  totalHoles1 = pixHoles1 + sctHoles1;
  totalHoles2 = pixHoles2 + sctHoles2;
  if (prob1 == prob2) {
    if (pt1 > pt2) {
      fillHisto(m_duplicateLPTHoles,totalHoles2);
      fillHisto(m_duplicateHPTHoles,totalHoles1);
      fillHisto(m_duplicateHPixelvsSCTShared,pixShared1, sctShared1);
      fillHisto(m_duplicateLPixelvsSCTShared,pixShared2, sctShared2);
      fillHisto(m_duplicateHPTHoles,totalHoles1);

      float deltaPT1 = pt1 - pt2;
      float deltaEta1 = eta1 - eta2;
      float deltaPhi1 = phi1 - phi2;

      fillHisto(m_duplicateDeltaPTvsTruthPT,truthPt, deltaPT1);
      fillHisto(m_duplicateDeltaPTvsTruthPTZoomed,truthPt, deltaPT1);

      // resolution plots
      float resHPT1 = pt1 - truthPt;
      float resHEta1 = eta1 - truthEta;
      float resHPhi1 = phi1 - truthPhi;
      float resLPT1 = pt2 - truthPt;
      float resLEta1 = eta2 - truthEta;
      float resLPhi1 = phi2 - truthPhi;

      fillHisto(m_duplicateResLPT,resLPT1);
      fillHisto(m_duplicateResLEta,resLEta1);
      fillHisto(m_duplicateResLPhi,resLPhi1);
      fillHisto(m_duplicateResHPT,resHPT1);
      fillHisto(m_duplicateResHEta,resHEta1);
      fillHisto(m_duplicateResHPhi,resHPhi1);


      // spectrum plots
      fillHisto(m_duplicateLPT,pt2);
      fillHisto(m_duplicateLEta,eta2);
      fillHisto(m_duplicateLPhi,phi2);
      fillHisto(m_duplicateHPT,pt1);
      fillHisto(m_duplicateHEta,eta1);
      fillHisto(m_duplicateHPhi,phi1);
      fillHisto(m_duplicateTruthPT,truthPt);
      fillHisto(m_duplicateTruthEta,truthEta);
      fillHisto(m_duplicateTruthPhi,truthPhi);
      // delta plots
      fillHisto(m_duplicateDeltaPt,deltaPT1);
      fillHisto(m_duplicateDeltaPtZoomed,deltaPT1);
      fillHisto(m_duplicateDeltaEta,deltaEta1);
      fillHisto(m_duplicateDeltaPhi,deltaPhi1);
      // lpt vs hpt

      // hpt & lpt vs truth

      fillHisto(m_duplicateProbSpectrum,prob2, prob1);
    }
    if (pt2 > pt1 || pt1 == pt2) {
      fillHisto(m_duplicateHPTHoles,totalHoles2);
      fillHisto(m_duplicateLPTHoles,totalHoles1);
      fillHisto(m_duplicateLPixelvsSCTShared,pixShared1, sctShared1);
      fillHisto(m_duplicateHPixelvsSCTShared,pixShared2, sctShared2);
      float deltaPT2 = pt2 - pt1;
      float deltaEta2 = eta2 - eta1;
      float deltaPhi2 = phi2 - phi1;
      // pt2 - pt1 vs truth


      fillHisto(m_duplicateDeltaPTvsTruthPT,truthPt, deltaPT2);
      fillHisto(m_duplicateDeltaPTvsTruthPTZoomed,truthPt, deltaPT2);




      // resolution plots
      float resLPT2 = pt1 - truthPt;
      float resLEta2 = eta1 - truthEta;
      float resLPhi2 = phi1 - truthPhi;
      float resHPT2 = pt2 - truthPt;
      float resHEta2 = eta2 - truthEta;
      float resHPhi2 = phi2 - truthPhi;
      fillHisto(m_duplicateResLPT,resLPT2);
      fillHisto(m_duplicateResLEta,resLEta2);
      fillHisto(m_duplicateResLPhi,resLPhi2);
      fillHisto(m_duplicateResHPT,resHPT2);
      fillHisto(m_duplicateResHEta,resHEta2);
      fillHisto(m_duplicateResHPhi,resHPhi2);

      // spectrum plots
      fillHisto(m_duplicateLPT,pt1);
      fillHisto(m_duplicateLEta,eta1);
      fillHisto(m_duplicateLPhi,phi1);
      fillHisto(m_duplicateHPT,pt2);
      fillHisto(m_duplicateHEta,eta2);
      fillHisto(m_duplicateHPhi,phi2);
      fillHisto(m_duplicateTruthPT,truthPt);
      fillHisto(m_duplicateTruthEta,truthEta);
      fillHisto(m_duplicateTruthPhi,truthPhi);
      // delta (hpt - lpt plots) & lpt vs hpt plts
      fillHisto(m_duplicateDeltaPt,deltaPT2);
      fillHisto(m_duplicateDeltaPtZoomed,deltaPT2);
      fillHisto(m_duplicateDeltaEta,deltaEta2);
      fillHisto(m_duplicateDeltaPhi,deltaPhi2);

      // lpt & hpt vs truth

      fillHisto(m_duplicateProbSpectrum,prob1, prob2);
    }
  }
  if (prob1 != prob2) {
    if (pt1 > pt2) {
      fillHisto(m_twoMatchHPixelvsSCTShared,pixShared1, sctShared1);
      fillHisto(m_twoMatchLPixelvsSCTShared,pixShared2, sctShared2);
      fillHisto(m_duplicateProbSpectrum,prob2, prob1);
    } else {
      fillHisto(m_twoMatchLPixelvsSCTShared,pixShared1, sctShared1);
      fillHisto(m_twoMatchHPixelvsSCTShared,pixShared2, sctShared2);

      fillHisto(m_duplicateProbSpectrum,prob1, prob2);
    }
  }
}
