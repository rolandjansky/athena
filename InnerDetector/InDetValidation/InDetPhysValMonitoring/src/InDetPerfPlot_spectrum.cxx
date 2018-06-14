/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file InDetPerfPlot_spectrum.cxx
 * @author Stewart Swift
 **/

#include "InDetPerfPlot_spectrum.h"
#include <cmath> // for M_PI
#include "InDetPhysValMonitoringUtilities.h"

using namespace IDPVM;


InDetPerfPlot_spectrum::InDetPerfPlot_spectrum(InDetPlotBase* pParent, const std::string& sDir) :
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

  m_fillPreSelTruth(false),
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
  m_nSCTHoles_vs_eta{},
  m_nPixHoles_vs_eta{},
  m_nTotHoles_vs_eta{},
  m_nSCTOutliers_vs_eta{},
  m_nPixOutliers_vs_eta{},
  m_nTotOutliers_vs_eta{},
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
  book(m_nSCTHoles_vs_eta, "nSCTHoles_vs_eta");
  book(m_nPixHoles_vs_eta, "nPixHoles_vs_eta");
  book(m_nTotHoles_vs_eta, "nTotHoles_vs_eta");
  book(m_nSCTOutliers_vs_eta, "nSCTOutliers_vs_eta");
  book(m_nPixOutliers_vs_eta, "nPixOutliers_vs_eta");
  book(m_nTotOutliers_vs_eta, "nTotOutliers_vs_eta");

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
  if ( NULL!=m_truthEta || NULL!=m_truthPt || NULL!=m_truthPhi )
    m_fillPreSelTruth=true;
  
  book(m_recod0_TruthVtxR, "recod0TVRSpectrum");
  book(m_recoz0_TruthVtxZ, "recoz0TVZSpectrum");
  book(m_recoz0_TruthVtxZsin, "recoz0TVZsinSpectrum");

  book(m_TruthVtxX, "TVXspectrum");
  book(m_TruthVtxY, "TVYspectrum");
  book(m_TruthVtxZ, "TVZspectrum");
  book(m_TruthVtxR, "TVRspectrum");


  book(m_TVR_vs_Z, "TVR_vs_Z");
  book(m_recod0_vs_z0_good, "recod0_vs_z0_good");
  book(m_recod0_vs_z0_crazy, "recod0_vs_z0_crazy");

  book(m_recod0_PrimVtxR, "recod0PVRSpectrum");
  book(m_recoz0_PrimVtxZ, "recoz0PVZSpectrum");
  book(m_recoz0_PrimVtxZsin, "recoz0PVZsinSpectrum");

  book(m_PrimVtxX, "PVXspectrum");
  book(m_PrimVtxY, "PVYspectrum");
  book(m_PrimVtxZ, "PVZspectrum");
  book(m_PrimVtxR, "PVRspectrum");

  book(m_PVR_vs_Z, "PVR_vs_Z");
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
InDetPerfPlot_spectrum::fillSpectrum(const xAOD::TrackParticle& trkprt, Float_t prob) {
  double pt = trkprt.pt() * 1_GeV;
  double eta = 0.0;

  if (trkprt.pt() > 0.1) {
    eta = trkprt.eta();
  }
  double phi(trkprt.phi());
  double d0(trkprt.d0());
  double z0(trkprt.z0());
  double sinth = sin(trkprt.theta());
  uint8_t iPixHits, iSCTHits, iPixDead, iSCTDead, iPixHoles, iSCTHoles, iPixOutliers, iSCTOutliers;
  /*  int pixHits = 0;
  int sctHits = 0;
  int pixDead = 0;
  int sctDead = 0;
  */
  int pixHits(0), sctHits(0), pixDead(0), sctDead(0), pixOutliers(0), sctOutliers(0), pixHoles(0), sctHoles(0);
  if (trkprt.summaryValue(iPixHits, xAOD::numberOfPixelHits)) {
    pixHits = iPixHits;
  }
  if (trkprt.summaryValue(iSCTHits, xAOD::numberOfSCTHits)) {
    sctHits = iSCTHits;
  }
  if (trkprt.summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
    pixHoles = iPixHoles;
  }
  if (trkprt.summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
    sctHoles = iSCTHoles;
  }
  if (trkprt.summaryValue(iPixOutliers, xAOD::numberOfPixelOutliers)) {
    pixOutliers = iPixOutliers;
  }
  if (trkprt.summaryValue(iSCTOutliers, xAOD::numberOfSCTOutliers)) {
    sctOutliers = iSCTOutliers;
  }
  if (trkprt.summaryValue(iPixDead, xAOD::numberOfPixelDeadSensors)) {
    pixDead = iPixDead;
  }
  if (trkprt.summaryValue(iSCTDead, xAOD::numberOfSCTDeadSensors)) {
    sctDead = iSCTDead;
  }
  fillHisto(m_recoMatchProb, prob);
  fillHisto(m_recoPt, pt);
  fillHisto(m_recoEta, eta);
  fillHisto(m_recoPhi, phi);
  fillHisto(m_recod0, d0);
  fillHisto(m_recoz0, z0);
  fillHisto(m_recoz0sin, z0 * sinth);

  fillHisto(m_nSCTHits, sctHits);
  fillHisto(m_nPixHits, pixHits);
  fillHisto(m_nTotHits, sctHits + pixHits);
  fillHisto(m_nSCTDeadSensors, sctDead);
  fillHisto(m_nPixDeadSensors, pixDead);
  fillHisto(m_nTotDeadSensors, sctDead + pixDead);


  fillHisto(m_nSCTHits_vs_eta, eta, sctHits);
  fillHisto(m_nPixHits_vs_eta, eta, pixHits);
  fillHisto(m_nTotHits_vs_eta, eta, sctHits + pixHits);
  fillHisto(m_nSCTHoles_vs_eta, eta, sctHoles);
  fillHisto(m_nPixHoles_vs_eta, eta, pixHoles);
  fillHisto(m_nTotHoles_vs_eta, eta, sctHoles + pixHoles);
  fillHisto(m_nSCTOutliers_vs_eta, eta, sctOutliers);
  fillHisto(m_nPixOutliers_vs_eta, eta, pixOutliers);
  fillHisto(m_nTotOutliers_vs_eta, eta, sctOutliers + pixOutliers);
  fillHisto(m_nSCTDeadSensors_vs_eta, eta, sctDead);
  fillHisto(m_nPixDeadSensors_vs_eta, eta, pixDead);
  fillHisto(m_nTotDeadSensors_vs_eta, eta, sctDead + pixDead);
}

void
InDetPerfPlot_spectrum::fillSpectrum(const xAOD::TruthParticle& particle) {

  // these histograms are time-consuming for large mu and full pu-truth
  // they will not be filled by default,
  // but only filled if the histogram defs are included in the .xml
  if (!m_fillPreSelTruth) {
    return;  
  }
  
  double pt = particle.pt() * 1_GeV;
  // double eta = particle.eta();
  double eta = 0.0;

  if (particle.pt() > 0.1) {
    eta = particle.eta();
  }

  double phi = particle.phi();

  if (NULL!=m_truthPt) fillHisto(m_truthPt, pt);
  if (NULL!=m_truthEta) fillHisto(m_truthEta, eta);
  if (NULL!=m_truthPhi) fillHisto(m_truthPhi, phi);
}

void
InDetPerfPlot_spectrum::fillSpectrum(const xAOD::TrackParticle& trkprt, const xAOD::TruthVertex& truthVrt) {
//ATH_MSG_INFO("filling with truth vtx");
  double d0(trkprt.d0());
  double z0(trkprt.z0());
  double sinth = std::sin(trkprt.theta());
  //
  double vtxX = truthVrt.x();
  double vtxY = truthVrt.y();
  double vtxZ = truthVrt.z();

  double vtxR = std::sqrt(vtxX * vtxX + vtxY * vtxY);

  fillHisto(m_recod0_TruthVtxR, d0 - vtxR);
  fillHisto(m_recoz0_TruthVtxZ, z0 - vtxZ);
  fillHisto(m_recoz0_TruthVtxZsin, (z0 - vtxZ) * sinth);
  //
  
  fillHisto(m_TruthVtxX, vtxX);
  fillHisto(m_TruthVtxY, vtxY);
  fillHisto(m_TruthVtxZ, vtxZ);
  fillHisto(m_TruthVtxR, vtxR);

  fillHisto(m_TVR_vs_Z, vtxZ, vtxR);
  
  if (vtxR > 0.1) {
    /*
       std::cout<<" "<<std::endl;
       std::cout<<"truth vtx. radius > 100 microns"<<std::endl;
       std::cout<<"truth vtx. X/Y/R/Z "<<vtxX<<","<<vtxY<<","<<vtxR<<","<<vtxZ<<std::endl;
       std::cout<<"trk d0/z0 "<<d0<<","<<z0<<std::endl;
     */
    fillHisto(m_recod0_vs_z0_crazy, z0, d0);
  } else {
    fillHisto(m_recod0_vs_z0_good, z0, d0);
  }
}

void
InDetPerfPlot_spectrum::fillSpectrum(const xAOD::TrackParticle& trkprt, const xAOD::Vertex& vertex, bool fill) {
  double d0(trkprt.d0());
  double z0(trkprt.z0());
  double sinth = std::sin(trkprt.theta());
  //
  double vtxX = vertex.x();
  double vtxY = vertex.y();
  double vtxZ = vertex.z();

  double vtxR = std::hypot(vtxX, vtxY);

  fillHisto(m_recod0_PrimVtxR, d0 - vtxR);
  fillHisto(m_recoz0_PrimVtxZ, z0 - vtxZ);
  fillHisto(m_recoz0_PrimVtxZsin, (z0 - vtxZ) * sinth);
  //
  if (fill) {
    fillHisto(m_PrimVtxX, vtxX);
    fillHisto(m_PrimVtxY, vtxY);
    fillHisto(m_PrimVtxZ, vtxZ);
    fillHisto(m_PrimVtxR, vtxR);

    fillHisto(m_PVR_vs_Z, vtxZ, vtxR);
  }
  if (vtxR > 0.1) {
    /*
       std::cout<<" "<<std::endl;
       std::cout<<"truth vtx. radius > 100 microns"<<std::endl;
       std::cout<<"truth vtx. X/Y/R/Z "<<vtxX<<","<<vtxY<<","<<vtxR<<","<<vtxZ<<std::endl;
       std::cout<<"trk d0/z0 "<<d0<<","<<z0<<std::endl;
     */
    fillHisto(m_recod0_vs_z0_crazy, z0, d0);
  } else {
    fillHisto(m_recod0_vs_z0_good, z0, d0);
  }
}

void
InDetPerfPlot_spectrum::fillSpectrumUnlinked2(const xAOD::TrackParticle& trkprt, double prob) {
  // unused constexpr double degreesPerRadian(180./M_PI);
  double pt = trkprt.pt() * 1_GeV;
  double eta(0);

  if (trkprt.pt() > 0.1) {
    eta = trkprt.eta();
  }
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

  fillHisto(m_ptvsEtaUnlinked, eta, pt);
  fillHisto(m_probvsSCTUnlinked, SCTHits, prob);
  fillHisto(m_probvsPixUnlinked, pixHits, prob);
  fillHisto(m_sharedHitsvsPixUnlinked, pixHits, pixSharedHits + SCTSharedHits);
  fillHisto(m_sharedHitsvsSCTUnlinked, SCTHits, pixSharedHits + SCTSharedHits);
  fillHisto(m_pixholesvsPixUnlinked, pixHits, pixHoles);
  fillHisto(m_holesvsPixUnlinked, pixHits, pixHoles + SCTHoles);
  fillHisto(m_sctholesvsSCTUnlinked, SCTHits, SCTHoles);
  fillHisto(m_holesvsSCTUnlinked, SCTHits, pixHoles + SCTHoles);
  fillHisto(m_outliersvsPixUnlinked, pixHits, pixOutliers + SCTOutliers);
  fillHisto(m_pixoutliersvsPixUnlinked, pixHits, pixOutliers);
  fillHisto(m_outliersvsSCTUnlinked, SCTHits, pixOutliers + SCTOutliers);
  fillHisto(m_sctoutliersvsSCTUnlinked, SCTHits, SCTOutliers);
  fillHisto(m_hitsvsEtaUnlinked, eta, pixHits + SCTHits);
  fillHisto(m_pixHolesvsEtaUnlinked, eta, pixHoles);
  fillHisto(m_sctHolesvsEtaUnlinked, eta, SCTHoles);
  fillHisto(m_sctHitsvsPixHitsUnlinked, pixHits, SCTHits);
  fillHisto(m_sctHitsvsEtaUnlinked, eta, SCTHits);
  fillHisto(m_pixHitsvsEtaUnlinked, eta, pixHits);
  fillHisto(m_sctOutliersvsEtaUnlinked, eta, SCTOutliers);
  fillHisto(m_pixOutliersvsEtaUnlinked, eta, pixOutliers);
}

void
InDetPerfPlot_spectrum::fillSpectrumLinked(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& /* particle*/,
                                           double prob) {
  // unused constexpr double degreesPerRadian(180./M_PI);
  double pt = trkprt.pt() * 1_GeV;
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

  fillHisto(m_ptvsEtaLinked, eta, pt);
  fillHisto(m_probvsSCTLinked, SCTHits, prob);
  fillHisto(m_probvsPixLinked, pixHits, prob);
  fillHisto(m_sharedHitsvsPixLinked, pixHits, pixSharedHits + SCTSharedHits);
  fillHisto(m_sharedHitsvsSCTLinked, SCTHits, pixSharedHits + SCTSharedHits);
  fillHisto(m_pixholesvsPixLinked, pixHits, pixHoles);
  fillHisto(m_holesvsPixLinked, pixHits, pixHoles + SCTHoles);
  fillHisto(m_sctholesvsSCTLinked, SCTHits, SCTHoles);
  fillHisto(m_holesvsSCTLinked, SCTHits, pixHoles + SCTHoles);
  fillHisto(m_outliersvsPixLinked, pixHits, pixOutliers + SCTOutliers);
  fillHisto(m_pixoutliersvsPixLinked, pixHits, pixOutliers);
  fillHisto(m_outliersvsSCTLinked, SCTHits, pixOutliers + SCTOutliers);
  fillHisto(m_sctoutliersvsSCTLinked, SCTHits, SCTOutliers);
  fillHisto(m_hitsvsEtaLinked, eta, pixHits + SCTHits);
  fillHisto(m_sctHitsvsPixHitsLinked, pixHits, SCTHits);
  fillHisto(m_pixHolesvsEtaLinked, eta, pixHoles);
  fillHisto(m_sctHolesvsEtaLinked, eta, SCTHoles);
  fillHisto(m_sctHitsvsEtaLinked, eta, SCTHits);
  fillHisto(m_pixHitsvsEtaLinked, eta, pixHits);
  fillHisto(m_sctOutliersvsEtaLinked, eta, SCTOutliers);
  fillHisto(m_pixOutliersvsEtaLinked, eta, pixOutliers);
}
