/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_spectrum.cxx
 * @author Stewart Swift
 **/

#include "InDetPerfPlot_spectrum.h"
#include <math.h> // for M_PI


InDetPerfPlot_spectrum::InDetPerfPlot_spectrum(InDetPlotBase *pParent, const std::string &sDir) :
  InDetPlotBase(pParent, sDir),
  m_nSCTHits{},
  m_nPixHits{},
  m_nTotHits{},
  m_nSCTDeadSensors{},
  m_nPixDeadSensors{},
  m_nTotDeadSensors{},

  m_recoMatchProb{},
  m_recoPt{},
  m_recoEta{},
  m_recoPhi{},
  m_recod0{},
  m_recoz0{},
  m_recoz0sin{},

  m_recod0_TruthVtxR{},
  m_recoz0_TruthVtxZ{},
  m_recoz0_TruthVtxZsin{},
  m_TruthVtxR{},
  m_TruthVtxZ{},
  m_TruthVtxX{},
  m_TruthVtxY{},

  m_TVR_vs_Z{},
  m_recod0_vs_z0_good{},
  m_recod0_vs_z0_crazy{},

  m_truthPt{},
  m_truthEta{},
  m_truthPhi{},

  m_recod0_PrimVtxR{},
  m_recoz0_PrimVtxZ{},
  m_recoz0_PrimVtxZsin{},
  m_PrimVtxR{},
  m_PrimVtxZ{},
  m_PrimVtxX{},
  m_PrimVtxY{},

  m_PVR_vs_Z{},
  m_nSCTHits_vs_eta{},
  m_nPixHits_vs_eta{},
  m_nTotHits_vs_eta{},
  m_nSCTHits_phi_vs_eta{},
  m_nPixHits_phi_vs_eta{},
  m_nTotHits_phi_vs_eta{},
  m_nSCTDeadSensors_vs_eta{},
  m_nPixDeadSensors_vs_eta{},
  m_nTotDeadSensors_vs_eta{},
  m_ptvsEtaUNLINKED{},
  m_ptvsEtaLINKED{},

  m_recoMatchvsSiHitsUNLINKED{},
  m_recoMatchvsSCTHitsUNLINKED{},
  m_recoMatchvsPixHitsUNLINKED{},
  m_recoMatchvsSiHitsLINKED{},
  m_recoMatchvsSCTHitsLINKED{},
  m_recoMatchvsPixHitsLINKED{},

  m_nSCTHits_vs_etaUNLINKED{},
  m_nPixHits_vs_etaUNLINKED{},
  m_nTotHits_vs_etaUNLINKED{},
  m_nSCTHits_vs_etaLINKED{},
  m_nPixHits_vs_etaLINKED{},
  m_nTotHits_vs_etaLINKED{},
  m_recoMatchProbUNLINKED{},
  m_recoMatchProbLINKED{},

  m_ptvsEtaUnlinked{},
  m_probvsSCTUnlinked{},
  m_probvsPixUnlinked{},
  m_sharedHitsvsSCTUnlinked{},
  m_sharedHitsvsPixUnlinked{},
  m_pixholesvsPixUnlinked{},
  m_holesvsPixUnlinked{},
  m_sctholesvsSCTUnlinked{},
  m_holesvsSCTUnlinked{},
  m_outliersvsPixUnlinked{},
  m_pixoutliersvsPixUnlinked{},
  m_outliersvsSCTUnlinked{},
  m_sctoutliersvsSCTUnlinked{},

  m_hitsvsEtaUnlinked{},
  m_pixHolesvsEtaUnlinked{},
  m_sctHolesvsEtaUnlinked{},
  m_sctHitsvsPixHitsUnlinked{},
  m_sctHitsvsEtaUnlinked{},
  m_pixHitsvsEtaUnlinked{},
  m_pixOutliersvsEtaUnlinked{},
  m_sctOutliersvsEtaUnlinked{},
  m_ptvsEtaLinked{},
  m_probvsSCTLinked{},
  m_probvsPixLinked{},
  m_sharedHitsvsSCTLinked{},
  m_sharedHitsvsPixLinked{},
  m_pixholesvsPixLinked{},
  m_holesvsPixLinked{},
  m_sctholesvsSCTLinked{},
  m_holesvsSCTLinked{},
  m_outliersvsPixLinked{},
  m_pixoutliersvsPixLinked{},
  m_sctoutliersvsSCTLinked{},
  m_outliersvsSCTLinked{},

  m_hitsvsEtaLinked{},
  m_pixHolesvsEtaLinked{},
  m_sctHolesvsEtaLinked{},

  m_sctHitsvsPixHitsLinked{},

  m_sctHitsvsEtaLinked{},
  m_pixHitsvsEtaLinked{},
  m_pixOutliersvsEtaLinked{},
  m_sctOutliersvsEtaLinked{} {
  // nop
}

void
InDetPerfPlot_spectrum::initializePlots() {
  //
  const bool prependDirectory(false);

  book(m_nSCTHits, "nSCTHits");
  book(m_nPixHits, "nPixHits");
  book(m_nTotHits, "nTotHits");

  // masking plots hits+deadsensors
  book(m_nSCTDeadSensors, "nSCTDeadSensors");
  book(m_nPixDeadSensors, "nPixDeadSensors");
  book(m_nTotDeadSensors, "nTotDeadSensors");
  //
  book(m_nSCTHits_vs_eta, "nSCTHits_vs_eta");
  book(m_nPixHits_vs_eta, "nPixHits_vs_eta");
  book(m_nTotHits_vs_eta, "nTotHits_vs_eta");

  book(m_nSCTDeadSensors_vs_eta, "nSCTDeadSensors_vs_eta");
  book(m_nPixDeadSensors_vs_eta, "nPixDeadSensors_vs_eta");
  book(m_nTotDeadSensors_vs_eta, "nTotDeadSensors_vs_eta");

  book(m_recoEta, "recoEtaSpectrum");
  book(m_recod0, "recod0Spectrum");
  book(m_recoz0, "recoz0Spectrum");
  book(m_recoz0sin, "recoz0sinSpectrum");
  book(m_recoPt, "recoPtSpectrum");
  book(m_recoPhi, "recoPhiSpectrum");
  book(m_recoMatchProb, "recoMatchProbabilitySpectrum");

  book(m_truthEta, "truthEtaSpectrum");
  book(m_truthPt, "truthPtSpectrum");
  book(m_truthPhi, "truthPhiSpectrum");
  book(m_recod0_TruthVtxR, "recod0TVRSpectrum");
  book(m_recoz0_TruthVtxZ, "recoz0TVZSpectrum");
  book(m_recoz0_TruthVtxZsin, "recoz0TVZsinSpectrum");

  book(m_TruthVtxX, "TVXspectrum");
  book(m_TruthVtxY, "TVYspectrum");
  book(m_TruthVtxZ, "TVZspectrum");
  book(m_TruthVtxR, "TVRspectrum");


  m_TVR_vs_Z = Book2D("TVR_vs_Z", "Truth Vtx Rad vs. Z", 200, -4000, 4000, 200, 0., 1000, prependDirectory);
  m_recod0_vs_z0_good = Book2D("recod0_vs_z0_good", "reco d0 vs. z0 - good TruthVert", 200, -4000., 4000, 200, -1000.,
                               1000., prependDirectory);
  m_recod0_vs_z0_crazy = Book2D("recod0_vs_z0_crazy", "reco d0 vs. z0 - crazy TruthVert", 200, -4000., 4000, 200,
                                -1000., 1000., prependDirectory);

  book(m_recod0_PrimVtxR, "recod0PVRSpectrum");
  book(m_recoz0_PrimVtxZ, "recoz0PVZSpectrum");
  book(m_recoz0_PrimVtxZsin, "recoz0PVZsinSpectrum");

  book(m_PrimVtxX, "PVXspectrum");
  book(m_PrimVtxY, "PVYspectrum");
  book(m_PrimVtxZ, "PVZspectrum");
  book(m_PrimVtxR, "PVRspectrum");

  m_PVR_vs_Z = Book2D("PVR_vs_Z", "Prim Vtx Rad vs. Z", 200, -4000, 4000, 100, 0., 0.5, prependDirectory);
  book(m_ptvsEtaUnlinked, "ptvsEtaUnlinked_postSelect");
  book(m_probvsSCTUnlinked, "probvsSCTUnlinked_postSelect");
  book(m_probvsPixUnlinked, "probvsPixUnlinked_postSelect");
  book(m_sharedHitsvsSCTUnlinked, "sharedHitsvsSCTUnlinked_postSelect");
  book(m_sharedHitsvsPixUnlinked, "sharedHitsvsPixUnlinked_postSelect");
  book(m_holesvsPixUnlinked, "holesvsPixUnlinked_postSelect");
  book(m_pixholesvsPixUnlinked, "pixholesvsPixUnlinked_postSelect");
  book(m_holesvsSCTUnlinked, "holesvsSCTUnlinked_postSelect");
  book(m_sctholesvsSCTUnlinked, "sctholesvsSCTUnlinked_postSelect");
  book(m_outliersvsPixUnlinked, "outliersvsPixUnlinked_postSelect");
  book(m_pixoutliersvsPixUnlinked, "pixoutliersvsPixUnlinked_postSelect");
  book(m_outliersvsSCTUnlinked, "outliersvsSCTUnlinked_postSelect");
  book(m_sctoutliersvsSCTUnlinked, "sctoutliersvsSCTUnlinked_postSelect");
  book(m_hitsvsEtaUnlinked, "hitsvsEtaUnlinked_postSelect");
  book(m_pixHolesvsEtaUnlinked, "pixHolesvsEtaUnlinked_postSelect");
  book(m_sctHolesvsEtaUnlinked, "sctHolesvsEtaUnlinked_postSelect");
  book(m_sctHitsvsPixHitsUnlinked, "sctHitsvsPixHitsUnlinked_postSelect");
  book(m_sctHitsvsEtaUnlinked, "sctHitsvsEtaUnlinked_postSelect");
  book(m_pixHitsvsEtaUnlinked, "pixHitsvsEtaUnlinked_postSelect");
  book(m_pixOutliersvsEtaUnlinked, "pixOutliersvsEtaUnlinked_postSelect");
  book(m_sctOutliersvsEtaUnlinked, "sctOutliersvsEtaUnlinked_postSelect");

  book(m_ptvsEtaLinked, "ptvsEtaLinked_postSelect");
  book(m_probvsSCTLinked, "probvsSCTLinked_postSelect");
  book(m_probvsPixLinked, "probvsPixLinked_postSelect");
  book(m_sharedHitsvsSCTLinked, "sharedHitsvsSCTLinked_postSelect");
  book(m_sharedHitsvsPixLinked, "sharedHitsvsPixLinked_postSelect");
  book(m_holesvsPixLinked, "holesvsPixLinked_postSelect");
  book(m_pixholesvsPixLinked, "pixholesvsPixLinked_postSelect");
  book(m_holesvsSCTLinked, "holesvsSCTLinked_postSelect");
  book(m_sctholesvsSCTLinked, "sctholesvsSCTLinked_postSelect");
  book(m_pixoutliersvsPixLinked, "pixoutliersvsPixLinked_postSelect");
  book(m_outliersvsPixLinked, "outliersvsPixLinked_postSelect");
  book(m_sctoutliersvsSCTLinked, "sctoutliersvsSCTLinked_postSelect");
  book(m_outliersvsSCTLinked, "outliersvsSCTLinked_postSelect");
  book(m_hitsvsEtaLinked, "hitsvsEtaLinked_postSelect");
  book(m_pixHolesvsEtaLinked, "pixHolesvsEtaLinked_postSelect");
  book(m_sctHolesvsEtaLinked, "sctHolesvsEtaLinked_postSelect");
  book(m_sctHitsvsPixHitsLinked, "sctHitsvsPixHitsLinked_postSelect");
  book(m_sctHitsvsEtaLinked, "sctHitsvsEtaLinked_postSelect");
  book(m_pixHitsvsEtaLinked, "pixHitsvsEtaLinked_postSelect");
  book(m_pixOutliersvsEtaLinked, "pixOutliersvsEtaLinked_postSelect");
  book(m_sctOutliersvsEtaLinked, "sctOutliersvsEtaLinked_postSelect");
}

void
InDetPerfPlot_spectrum::fillSpectrum(const xAOD::TrackParticle &trkprt, Float_t prob) {
  double pt = trkprt.pt() / 1000.;
  double eta = 0.0;

  if (trkprt.pt() > 1e-7) {
    eta = trkprt.eta();
  }
  double phi(trkprt.phi());
  double d0(trkprt.d0());
  double z0(trkprt.z0());
  double sinth = sin(trkprt.theta());
  uint8_t iPixHits, iSCTHits, iPixDead, iSCTDead;
  int pixHits = 0;
  int sctHits = 0;
  int pixDead = 0;
  int sctDead = 0;

  if (trkprt.summaryValue(iPixHits, xAOD::numberOfPixelHits)) {
    pixHits = iPixHits;
  }
  if (trkprt.summaryValue(iSCTHits, xAOD::numberOfSCTHits)) {
    sctHits = iSCTHits;
  }
  if (trkprt.summaryValue(iPixDead, xAOD::numberOfPixelDeadSensors)) {
    pixDead = iPixDead;
  }
  if (trkprt.summaryValue(iSCTDead, xAOD::numberOfSCTDeadSensors)) {
    sctDead = iSCTDead;
  }
  m_recoMatchProb->Fill(prob);
  m_recoPt->Fill(pt);
  m_recoEta->Fill(eta);
  m_recoPhi->Fill(phi);
  m_recod0->Fill(d0);
  m_recoz0->Fill(z0);
  m_recoz0sin->Fill(z0 * sinth);

  m_nSCTHits->Fill(sctHits);
  m_nPixHits->Fill(pixHits);
  m_nTotHits->Fill(sctHits + pixHits);
  m_nSCTDeadSensors->Fill(sctDead);
  m_nPixDeadSensors->Fill(pixDead);
  m_nTotDeadSensors->Fill(sctDead + pixDead);


  m_nSCTHits_vs_eta->Fill(eta, sctHits);
  m_nPixHits_vs_eta->Fill(eta, pixHits);
  m_nTotHits_vs_eta->Fill(eta, sctHits + pixHits);
  m_nSCTDeadSensors_vs_eta->Fill(eta, sctDead);
  m_nPixDeadSensors_vs_eta->Fill(eta, pixDead);
  m_nTotDeadSensors_vs_eta->Fill(eta, sctDead + pixDead);
}

void
InDetPerfPlot_spectrum::fillSpectrum(const xAOD::TruthParticle &particle) {
  double pt = particle.pt() / 1000;
  // double eta = particle.eta();
  double eta = 0.0;

  if (particle.pt() > 1e-7) {
    eta = particle.eta();
  }

  double phi = particle.phi();

  m_truthPt->Fill(pt);
  m_truthEta->Fill(eta);
  m_truthPhi->Fill(phi);
}

void
InDetPerfPlot_spectrum::fillSpectrum(const xAOD::TrackParticle &trkprt, const xAOD::TruthVertex &truthVrt) {
  double d0(trkprt.d0());
  double z0(trkprt.z0());
  double sinth = sin(trkprt.theta());
  //
  double vtxX = truthVrt.x();
  double vtxY = truthVrt.y();
  double vtxZ = truthVrt.z();

  double vtxR = sqrt(vtxX * vtxX + vtxY * vtxY);

  m_recod0_TruthVtxR->Fill(d0 - vtxR);
  m_recoz0_TruthVtxZ->Fill(z0 - vtxZ);
  m_recoz0_TruthVtxZsin->Fill((z0 - vtxZ) * sinth);
  //
  m_TruthVtxX->Fill(vtxX);
  m_TruthVtxY->Fill(vtxY);
  m_TruthVtxZ->Fill(vtxZ);
  m_TruthVtxR->Fill(vtxR);

  m_TVR_vs_Z->Fill(vtxZ, vtxR);

  if (vtxR > 0.1) {
    /*
       std::cout<<" "<<std::endl;
       std::cout<<"truth vtx. radius > 100 microns"<<std::endl;
       std::cout<<"truth vtx. X/Y/R/Z "<<vtxX<<","<<vtxY<<","<<vtxR<<","<<vtxZ<<std::endl;
       std::cout<<"trk d0/z0 "<<d0<<","<<z0<<std::endl;
     */
    m_recod0_vs_z0_crazy->Fill(z0, d0);
  }else {
    m_recod0_vs_z0_good->Fill(z0, d0);
  }
}

void
InDetPerfPlot_spectrum::fillSpectrum(const xAOD::TrackParticle &trkprt, const xAOD::Vertex &vertex, bool fill) {
  double d0(trkprt.d0());
  double z0(trkprt.z0());
  double sinth = sin(trkprt.theta());
  //
  double vtxX = vertex.x();
  double vtxY = vertex.y();
  double vtxZ = vertex.z();

  double vtxR = sqrt(vtxX * vtxX + vtxY * vtxY);

  m_recod0_PrimVtxR->Fill(d0 - vtxR);
  m_recoz0_PrimVtxZ->Fill(z0 - vtxZ);
  m_recoz0_PrimVtxZsin->Fill((z0 - vtxZ) * sinth);
  //
  if (fill) {
    m_PrimVtxX->Fill(vtxX);
    m_PrimVtxY->Fill(vtxY);
    m_PrimVtxZ->Fill(vtxZ);
    m_PrimVtxR->Fill(vtxR);

    m_PVR_vs_Z->Fill(vtxZ, vtxR);
  }
  if (vtxR > 0.1) {
    /*
       std::cout<<" "<<std::endl;
       std::cout<<"truth vtx. radius > 100 microns"<<std::endl;
       std::cout<<"truth vtx. X/Y/R/Z "<<vtxX<<","<<vtxY<<","<<vtxR<<","<<vtxZ<<std::endl;
       std::cout<<"trk d0/z0 "<<d0<<","<<z0<<std::endl;
     */
    m_recod0_vs_z0_crazy->Fill(z0, d0);
  }else {
    m_recod0_vs_z0_good->Fill(z0, d0);
  }
}

void
InDetPerfPlot_spectrum::fillSpectrumUnlinked2(const xAOD::TrackParticle &trkprt, double prob) {
  // unused constexpr double degreesPerRadian(180./M_PI);
  double pt = trkprt.pt() / 1000.;
  double eta(trkprt.eta());
  // unused double phi(trkprt.phi());

  uint8_t iPixOutliers, iSCTOutliers, iPixHoles, iSCTHoles, iPixHits, iSCTHits, iPixSharedHits, iSCTSharedHits;
  int pixOutliers(0), SCTOutliers(0), pixHoles(0), SCTHoles(0), pixHits(0), SCTHits(0), pixSharedHits(0), SCTSharedHits(
    0);

  if (trkprt.summaryValue(iPixOutliers, xAOD::numberOfPixelOutliers)) {
    pixOutliers = iPixOutliers;
  }
  if (trkprt.summaryValue(iSCTOutliers, xAOD::numberOfSCTOutliers)) {
    SCTOutliers = iSCTOutliers;
  }
  if (trkprt.summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
    pixHoles = iPixHoles;
  }
  if (trkprt.summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
    SCTHoles = iSCTHoles;
  }
  if (trkprt.summaryValue(iPixHits, xAOD::numberOfPixelHits)) {
    pixHits = iPixHits;
  }
  if (trkprt.summaryValue(iSCTHits, xAOD::numberOfSCTHits)) {
    SCTHits = iSCTHits;
  }
  if (trkprt.summaryValue(iPixSharedHits, xAOD::numberOfPixelSharedHits)) {
    pixSharedHits = iPixSharedHits;
  }
  if (trkprt.summaryValue(iSCTSharedHits, xAOD::numberOfSCTSharedHits)) {
    SCTSharedHits = iSCTSharedHits;
  }

  m_ptvsEtaUnlinked->Fill(eta, pt);
  m_probvsSCTUnlinked->Fill(SCTHits, prob);
  m_probvsPixUnlinked->Fill(pixHits, prob);
  m_sharedHitsvsPixUnlinked->Fill(pixHits, pixSharedHits + SCTSharedHits);
  m_sharedHitsvsSCTUnlinked->Fill(SCTHits, pixSharedHits + SCTSharedHits);
  m_pixholesvsPixUnlinked->Fill(pixHits, pixHoles);
  m_holesvsPixUnlinked->Fill(pixHits, pixHoles + SCTHoles);
  m_sctholesvsSCTUnlinked->Fill(SCTHits, SCTHoles);
  m_holesvsSCTUnlinked->Fill(SCTHits, pixHoles + SCTHoles);
  m_outliersvsPixUnlinked->Fill(pixHits, pixOutliers + SCTOutliers);
  m_pixoutliersvsPixUnlinked->Fill(pixHits, pixOutliers);
  m_outliersvsSCTUnlinked->Fill(SCTHits, pixOutliers + SCTOutliers);
  m_sctoutliersvsSCTUnlinked->Fill(SCTHits, SCTOutliers);
  m_hitsvsEtaUnlinked->Fill(eta, pixHits + SCTHits);
  m_pixHolesvsEtaUnlinked->Fill(eta, pixHoles);
  m_sctHolesvsEtaUnlinked->Fill(eta, SCTHoles);
  m_sctHitsvsPixHitsUnlinked->Fill(pixHits, SCTHits);
  m_sctHitsvsEtaUnlinked->Fill(eta, SCTHits);
  m_pixHitsvsEtaUnlinked->Fill(eta, pixHits);
  m_sctOutliersvsEtaUnlinked->Fill(eta, SCTOutliers);
  m_pixOutliersvsEtaUnlinked->Fill(eta, pixOutliers);
}

void
InDetPerfPlot_spectrum::fillSpectrumLinked(const xAOD::TrackParticle &trkprt, const xAOD::TruthParticle & /* particle*/,
                                           double prob) {
  // unused constexpr double degreesPerRadian(180./M_PI);
  double pt = trkprt.pt() / 1000.;
  double eta(trkprt.eta());
  // unused double phi(trkprt.phi());

  uint8_t iPixOutliers, iSCTOutliers, iPixHoles, iSCTHoles, iPixHits, iSCTHits, iPixSharedHits, iSCTSharedHits;
  int pixOutliers(0), SCTOutliers(0), pixHoles(0), SCTHoles(0), pixHits(0), SCTHits(0), pixSharedHits(0), SCTSharedHits(
    0);

  if (trkprt.summaryValue(iPixOutliers, xAOD::numberOfPixelOutliers)) {
    pixOutliers = iPixOutliers;
  }
  if (trkprt.summaryValue(iSCTOutliers, xAOD::numberOfSCTOutliers)) {
    SCTOutliers = iSCTOutliers;
  }
  if (trkprt.summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
    pixHoles = iPixHoles;
  }
  if (trkprt.summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
    SCTHoles = iSCTHoles;
  }
  if (trkprt.summaryValue(iPixHits, xAOD::numberOfPixelHits)) {
    pixHits = iPixHits;
  }
  if (trkprt.summaryValue(iSCTHits, xAOD::numberOfSCTHits)) {
    SCTHits = iSCTHits;
  }
  if (trkprt.summaryValue(iPixSharedHits, xAOD::numberOfPixelSharedHits)) {
    pixSharedHits = iPixSharedHits;
  }
  if (trkprt.summaryValue(iSCTSharedHits, xAOD::numberOfSCTSharedHits)) {
    SCTSharedHits = iSCTSharedHits;
  }

  m_ptvsEtaLinked->Fill(eta, pt);
  m_probvsSCTLinked->Fill(SCTHits, prob);
  m_probvsPixLinked->Fill(pixHits, prob);
  m_sharedHitsvsPixLinked->Fill(pixHits, pixSharedHits + SCTSharedHits);
  m_sharedHitsvsSCTLinked->Fill(SCTHits, pixSharedHits + SCTSharedHits);
  m_pixholesvsPixLinked->Fill(pixHits, pixHoles);
  m_holesvsPixLinked->Fill(pixHits, pixHoles + SCTHoles);
  m_sctholesvsSCTLinked->Fill(SCTHits, SCTHoles);
  m_holesvsSCTLinked->Fill(SCTHits, pixHoles + SCTHoles);
  m_outliersvsPixLinked->Fill(pixHits, pixOutliers + SCTOutliers);
  m_pixoutliersvsPixLinked->Fill(pixHits, pixOutliers);
  m_outliersvsSCTLinked->Fill(SCTHits, pixOutliers + SCTOutliers);
  m_sctoutliersvsSCTLinked->Fill(SCTHits, SCTOutliers);
  m_hitsvsEtaLinked->Fill(eta, pixHits + SCTHits);
  m_sctHitsvsPixHitsLinked->Fill(pixHits, SCTHits);
  m_pixHolesvsEtaLinked->Fill(eta, pixHoles);
  m_sctHolesvsEtaLinked->Fill(eta, SCTHoles);
  m_sctHitsvsEtaLinked->Fill(eta, SCTHits);
  m_pixHitsvsEtaLinked->Fill(eta, pixHits);
  m_sctOutliersvsEtaLinked->Fill(eta, SCTOutliers);
  m_pixOutliersvsEtaLinked->Fill(eta, pixOutliers);
}
