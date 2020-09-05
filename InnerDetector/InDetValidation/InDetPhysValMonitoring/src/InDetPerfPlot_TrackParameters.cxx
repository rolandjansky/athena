/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_TrackParameters.cxx
 * @author shaun roe
 **/


#include "InDetPhysValMonitoringUtilities.h"

#include "InDetPerfPlot_TrackParameters.h"
#include <cmath>
using namespace IDPVM;

InDetPerfPlot_TrackParameters::InDetPerfPlot_TrackParameters(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir),
  m_reco_d0{},
  m_reco_z0{},
  m_reco_z0sin{},
  m_reco_phi{},
  m_reco_theta{},
  m_reco_eta{},
  m_reco_qoverp{},
  m_reco_pt{},
  m_reco_lowpt{},
  m_reco_chi2{},
  m_reco_ndof{},
  m_reco_chi2Overndof{},
  m_reco_author{},

  m_truth_d0{},
  m_truth_z0{},
  m_truth_z0sin{},
  m_truth_phi{},
  m_truth_theta{},
  m_truth_eta{},
  m_truth_qoverp{},
  m_truth_pt{},
  m_truth_lowpt{},
  m_truth_prodR{},
  m_truth_prodZ{},

  m_reco_pt_vs_eta{},
  m_reco_phi_vs_eta{},

  m_reco_d0_z0{},
  m_reco_d0_z0sin{},


  m_truth_pt_vs_eta{},
  m_truth_phi_vs_eta{} {
}

void
InDetPerfPlot_TrackParameters::initializePlots() {

  book(m_reco_d0,     "reco_d0");
  book(m_reco_z0,     "reco_z0");
  book(m_reco_z0sin,  "reco_z0sin");
  book(m_reco_phi,    "reco_phi");
  book(m_reco_theta,  "reco_theta");
  book(m_reco_eta,    "reco_eta");
  book(m_reco_qoverp, "reco_qoverp");
  book(m_reco_pt,     "reco_pt");
  book(m_reco_lowpt,     "reco_lowpt");
  book(m_reco_chi2,   "reco_chi2");
  book(m_reco_ndof,   "reco_ndof");
  book(m_reco_chi2Overndof, "reco_chi2Overndof");
  book(m_reco_author,  "reco_author");

  book(m_truth_d0,     "truth_d0");
  book(m_truth_z0,     "truth_z0");
  book(m_truth_z0sin,  "truth_z0sin");
  book(m_truth_phi,    "truth_phi");
  book(m_truth_theta,  "truth_theta");
  book(m_truth_eta,    "truth_eta");
  book(m_truth_qoverp, "truth_qoverp");
  book(m_truth_pt,     "truth_pt");
  book(m_truth_lowpt,     "truth_lowpt");
  book(m_truth_prodR,  "truth_prodR");
  book(m_truth_prodZ,  "truth_prodZ");

  book(m_reco_pt_vs_eta, "reco_pt_vs_eta");
  book(m_reco_phi_vs_eta, "reco_phi_vs_eta");

  book(m_reco_d0_z0, "reco_d0_vs_z0");
  book(m_reco_d0_z0sin, "reco_d0_vs_z0sin");

  book(m_truth_pt_vs_eta, "truth_pt_vs_eta");
  book(m_truth_phi_vs_eta, "truth_phi_vs_eta");

}

void
InDetPerfPlot_TrackParameters::fill(const xAOD::TruthParticle& particle) {

  // quantities with xAOD::TruthParticle accessors:
  float eta = particle.eta();
  float pt = particle.pt() / Gaudi::Units::GeV;

  float d0 = (particle.isAvailable<float>("d0")) ? particle.auxdata<float>("d0") : -9999.;
  float z0 = (particle.isAvailable<float>("z0")) ? particle.auxdata<float>("z0") : -9999.;
  float theta = (particle.isAvailable<float>("theta")) ? particle.auxdata<float>("theta") : -9999.;
  float phi = (particle.isAvailable<float>("phi")) ? particle.auxdata<float>("phi") : -9999.;
  float z0sin = (particle.isAvailable<float>("z0") && particle.auxdata<float>("theta")) ? z0 * std::sin(theta) : -9999.;
  float qOverP = (particle.isAvailable<float>("qOverP")) ? particle.auxdata<float>("qOverP") : -9999.;
  float prodR = (particle.isAvailable<float>("prodR")) ? particle.auxdata<float>("prodR") : -9999.;
  float prodZ = (particle.isAvailable<float>("prodZ")) ? particle.auxdata<float>("prodZ") : -9999.;

  if(d0 > -9000.) fillHisto(m_truth_d0, d0);
  if(z0 > -9000.) fillHisto(m_truth_z0, z0);
  if(theta > -9000.) fillHisto(m_truth_theta, theta);
  if(phi > -9000.) fillHisto(m_truth_phi, phi);
  if(z0sin > -9000.) fillHisto(m_truth_z0sin, z0sin);
  if(qOverP > -9000.) fillHisto(m_truth_qoverp, qOverP);
  if(prodR > -9000.) fillHisto(m_truth_prodR, prodR);
  if(prodZ > -9000.) fillHisto(m_truth_prodZ, prodZ);

  fillHisto(m_truth_eta, eta);
  fillHisto(m_truth_pt, pt);
  fillHisto(m_truth_lowpt, pt);

  fillHisto(m_truth_pt_vs_eta, pt, eta);
  fillHisto(m_truth_phi_vs_eta, phi, eta);

}

void
InDetPerfPlot_TrackParameters::fill(const xAOD::TrackParticle& particle) {

  float pt = particle.pt() / Gaudi::Units::GeV;
  float eta = particle.eta();
  float phi = particle.phi0();

  float chi2 = particle.chiSquared();
  float ndof = particle.numberDoF();
  float chi2Overndof = ndof > 0 ? chi2 / ndof : 0;


  fillHisto(m_reco_d0, particle.d0());
  fillHisto(m_reco_z0, particle.z0());
  fillHisto(m_reco_z0sin, particle.z0()* std::sin(particle.theta()));

  fillHisto(m_reco_phi, phi);
  fillHisto(m_reco_theta, particle.theta());
  fillHisto(m_reco_eta, eta);
  fillHisto(m_reco_qoverp, particle.qOverP());
  fillHisto(m_reco_pt, pt);
  fillHisto(m_reco_lowpt, pt);

  fillHisto(m_reco_pt_vs_eta, pt, eta);
  fillHisto(m_reco_phi_vs_eta, phi, eta);

  fillHisto(m_reco_d0_z0,    particle.d0(), particle.z0());
  fillHisto(m_reco_d0_z0sin, particle.d0(), particle.z0()*std::sin(particle.theta()));

  fillHisto(m_reco_chi2, chi2);
  fillHisto(m_reco_ndof, ndof);
  fillHisto(m_reco_chi2Overndof, chi2Overndof);

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = particle.patternRecoInfo();
  for(unsigned int i = 0; i < xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo; i++){
    if(patternInfo.test(i)) fillHisto(m_reco_author, i);
  }

}

void
InDetPerfPlot_TrackParameters::finalizePlots() {

  //
  //Pie
  //

}
