/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_AltNonprimTracking.cxx
 * @author nora pettersson
 **/


#include "InDetPhysValMonitoringUtilities.h"

#include "InDetPerfPlot_AltNonprimTracking.h"

#include <cmath>

using namespace IDPVM;

InDetPerfPlot_AltNonprimTracking::InDetPerfPlot_AltNonprimTracking(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir),
//  m_resolutionMethod(IDPVM::ResolutionHelper::iterRMS_convergence),
  m_effANT_eta{},
  m_effSTD_eta{},
  m_effBAT_eta{},
  m_effANT_pt{},
  m_effSTD_pt{},
  m_effBAT_pt{},
  m_effANT_phi{},
  m_effSTD_phi{},
  m_effBAT_phi{},
  m_effANT_d0{},
  m_effSTD_d0{},
  m_effBAT_d0{},
  m_effANT_z0{},
  m_effSTD_z0{},
  m_effBAT_z0{},
  m_effANT_radius{},
  m_effSTD_radius{},
  m_effBAT_radius{},
  m_effANT_mu{},
  m_effSTD_mu{},
  m_effBAT_mu{},
  m_effANT_nvertices{},
  m_effSTD_nvertices{},
  m_effBAT_nvertices{},

  m_fakeANT_eta{},
  m_fakeSTD_eta{},
  m_fakeBAT_eta{},
  m_fakeANT_pt{},
  m_fakeSTD_pt{},
  m_fakeBAT_pt{},
  m_fakeANT_phi{},
  m_fakeSTD_phi{},
  m_fakeBAT_phi{},
  m_fakeANT_d0{},
  m_fakeSTD_d0{},
  m_fakeBAT_d0{},
  m_fakeANT_z0{},
  m_fakeSTD_z0{},
  m_fakeBAT_z0{},
  m_fakeANT_radius{},
  m_fakeSTD_radius{},
  m_fakeBAT_radius{},
  m_fakeANT_mu{},
  m_fakeSTD_mu{},
  m_fakeBAT_mu{},
  m_fakeANT_nvertices{},
  m_fakeSTD_nvertices{},
  m_fakeBAT_nvertices{}


  {
}

void
InDetPerfPlot_AltNonprimTracking::initializePlots() {

  book(m_effANT_eta, "efficiencySplit_vs_eta", "efficiencyANT_vs_eta");
  book(m_effSTD_eta, "efficiencySplit_vs_eta", "efficiencySTD_vs_eta");
  book(m_effBAT_eta, "efficiencySplit_vs_eta", "efficiencyBAT_vs_eta");

  book(m_effANT_pt, "efficiencySplit_vs_pt", "efficiencyANT_vs_pt");
  book(m_effSTD_pt, "efficiencySplit_vs_pt", "efficiencySTD_vs_pt");
  book(m_effBAT_pt, "efficiencySplit_vs_pt", "efficiencyBAT_vs_pt");

  book(m_effANT_phi, "efficiencySplit_vs_phi", "efficiencyANT_vs_phi");
  book(m_effSTD_phi, "efficiencySplit_vs_phi", "efficiencySTD_vs_phi");
  book(m_effBAT_phi, "efficiencySplit_vs_phi", "efficiencyBAT_vs_phi");

  book(m_effANT_d0, "efficiencySplit_vs_d0", "efficiencyANT_vs_d0");
  book(m_effSTD_d0, "efficiencySplit_vs_d0", "efficiencySTD_vs_d0");
  book(m_effBAT_d0, "efficiencySplit_vs_d0", "efficiencyBAT_vs_d0");

  book(m_effANT_z0, "efficiencySplit_vs_z0", "efficiencyANT_vs_z0");
  book(m_effSTD_z0, "efficiencySplit_vs_z0", "efficiencySTD_vs_z0");
  book(m_effBAT_z0, "efficiencySplit_vs_z0", "efficiencyBAT_vs_z0");

  book(m_effANT_radius, "efficiencySplit_vs_radius", "efficiencyANT_vs_radius");
  book(m_effSTD_radius, "efficiencySplit_vs_radius", "efficiencySTD_vs_radius");
  book(m_effBAT_radius, "efficiencySplit_vs_radius", "efficiencyBAT_vs_radius");

  book(m_effANT_mu, "efficiencySplit_vs_mu", "efficiencyANT_vs_mu");
  book(m_effSTD_mu, "efficiencySplit_vs_mu", "efficiencySTD_vs_mu");
  book(m_effBAT_mu, "efficiencySplit_vs_mu", "efficiencyBAT_vs_mu");

  book(m_effANT_nvertices, "efficiencySplit_vs_nvertices", "efficiencyANT_vs_nvertices");
  book(m_effSTD_nvertices, "efficiencySplit_vs_nvertices", "efficiencySTD_vs_nvertices");
  book(m_effBAT_nvertices, "efficiencySplit_vs_nvertices", "efficiencyBAT_vs_nvertices");

  book(m_fakeANT_eta, "fakeSplit_vs_eta", "fakeANT_vs_eta");
  book(m_fakeSTD_eta, "fakeSplit_vs_eta", "fakeSTD_vs_eta");
  book(m_fakeBAT_eta, "fakeSplit_vs_eta", "fakeBAT_vs_eta");

  book(m_fakeANT_pt, "fakeSplit_vs_pt", "fakeANT_vs_pt");
  book(m_fakeSTD_pt, "fakeSplit_vs_pt", "fakeSTD_vs_pt");
  book(m_fakeBAT_pt, "fakeSplit_vs_pt", "fakeBAT_vs_pt");

  book(m_fakeANT_phi, "fakeSplit_vs_phi", "fakeANT_vs_phi");
  book(m_fakeSTD_phi, "fakeSplit_vs_phi", "fakeSTD_vs_phi");
  book(m_fakeBAT_phi, "fakeSplit_vs_phi", "fakeBAT_vs_phi");

  book(m_fakeANT_d0, "fakeSplit_vs_d0", "fakeANT_vs_d0");
  book(m_fakeSTD_d0, "fakeSplit_vs_d0", "fakeSTD_vs_d0");
  book(m_fakeBAT_d0, "fakeSplit_vs_d0", "fakeBAT_vs_d0");

  book(m_fakeANT_z0, "fakeSplit_vs_z0", "fakeANT_vs_z0");
  book(m_fakeSTD_z0, "fakeSplit_vs_z0", "fakeSTD_vs_z0");
  book(m_fakeBAT_z0, "fakeSplit_vs_z0", "fakeBAT_vs_z0");

  book(m_fakeANT_radius, "fakeSplit_vs_radius", "fakeANT_vs_radius");
  book(m_fakeSTD_radius, "fakeSplit_vs_radius", "fakeSTD_vs_radius");
  book(m_fakeBAT_radius, "fakeSplit_vs_radius", "fakeBAT_vs_radius");

  book(m_fakeANT_mu, "fakeSplit_vs_mu", "fakeANT_vs_mu");
  book(m_fakeSTD_mu, "fakeSplit_vs_mu", "fakeSTD_vs_mu");
  book(m_fakeBAT_mu, "fakeSplit_vs_mu", "fakeBAT_vs_mu");

  book(m_fakeANT_nvertices, "fakeSplit_vs_nvertices", "fakeANT_vs_nvertices");
  book(m_fakeSTD_nvertices, "fakeSplit_vs_nvertices", "fakeSTD_vs_nvertices");
  book(m_fakeBAT_nvertices, "fakeSplit_vs_nvertices", "fakeBAT_vs_nvertices");

}

void
InDetPerfPlot_AltNonprimTracking::fill(const xAOD::TrackParticle& particle) {

  double eta = particle.eta();
  double phi = particle.phi();
  double pt = particle.pt() / Gaudi::Units::GeV;
  double d0 = particle.d0();
  double z0 = particle.z0();
  float chi2 = particle.chiSquared();
  float ndof = particle.numberDoF();
  float chi2Overndof = ndof > 0 ? chi2 / ndof : 0;

  uint8_t iTrtHits = 0;
  particle.summaryValue(iTrtHits, xAOD::numberOfTRTHits);

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = particle.patternRecoInfo();
  bool isBAT = patternInfo.test(xAOD::TrackPatternRecoInfo::TRTSeededTrackFinder);
  bool isANT = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_LargeD0);
  bool isSTD = not isBAT and not isANT;

}

void
InDetPerfPlot_AltNonprimTracking::fill(const xAOD::TrackParticle& particle, const float mu, const unsigned int nvertices) {

  uint8_t iTrtHits = 0;
  particle.summaryValue(iTrtHits, xAOD::numberOfTRTHits);

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = particle.patternRecoInfo();
  bool isBAT = patternInfo.test(xAOD::TrackPatternRecoInfo::TRTSeededTrackFinder);
  bool isANT = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_LargeD0);
  bool isSTD = not isBAT and not isANT;


}

void
InDetPerfPlot_AltNonprimTracking::fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle) {

  //Fraction of extended for truth matched tracks

  uint8_t iTrtHits = 0;
  particle.summaryValue(iTrtHits, xAOD::numberOfTRTHits);

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = particle.patternRecoInfo();
  bool isAltNonprimTracking = patternInfo.test(3) or iTrtHits > 0;
  
  //Get pT resolution for TRT extensions versus without
  const float undefinedValue = -9999;
  const float smallestAllowableTan = 1e-8; 
  const float sinTheta{std::sin(particle.theta())};
  const bool saneSineValue = (std::abs(sinTheta) > 1e-8);
  const float inverseSinTheta = saneSineValue ? 1./sinTheta : undefinedValue;
  float track_qopt = saneSineValue ? particle.qOverP()*inverseSinTheta : undefinedValue;
  const float qopterr = std::sqrt(particle.definingParametersCovMatrix()(4, 4)) * inverseSinTheta;

  const float truth_qop = truthParticle.isAvailable<float>("qOverP") ? truthParticle.auxdata<float>("qOverP") : undefinedValue;
  const float truth_theta = truthParticle.isAvailable<float>("theta") ? truthParticle.auxdata<float>("theta") : undefinedValue;
  float truth_qopt = std::abs(truth_theta) > 0 ? truth_qop * 1/(std::sin(truth_theta)) : undefinedValue;

  float ptres = (track_qopt - truth_qopt) * ( 1 / truth_qopt);
  float ptpull = qopterr > smallestAllowableTan ? (track_qopt - truth_qopt) / qopterr : undefinedValue;
  float pt = truthParticle.pt() / Gaudi::Units::GeV;
  const float tanHalfTheta = std::tan(truth_theta * 0.5);
  const bool tanThetaIsSane = std::abs(tanHalfTheta) > smallestAllowableTan;
  float eta = undefinedValue;
  if (tanThetaIsSane) eta = -std::log(tanHalfTheta);

}

void
InDetPerfPlot_AltNonprimTracking::fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::TrackParticle& track, const bool isGood, const float mu, const unsigned int nVtx) {

  const float undefinedValue = -9999;

  double eta = truth.eta();
  double pt = truth.pt() / Gaudi::Units::GeV; // convert MeV to GeV
  double phi = truth.phi();

  double d0 = truth.isAvailable<float>("d0") ? truth.auxdata<float>("d0") : undefinedValue;
  double z0 = truth.isAvailable<float>("z0") ? truth.auxdata<float>("z0") : undefinedValue;
  double radius =  truth.isAvailable<float>("prodR") ? truth.auxdata<float>("prodR") : undefinedValue;
 
  std::cout << track.pt() << std::endl;
  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo> patternInfo = track.patternRecoInfo();
  bool isBAT = patternInfo.test(3); //xAOD::TrackPatternRecoInfo::TRTSeededTrackFinder);
  bool isANT = patternInfo.test(49); //xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_LargeD0);
  bool isSTD = not isBAT and not isANT;

  if(isANT){
    fillHisto(m_effANT_eta, eta, isGood);
    fillHisto(m_effANT_pt, pt, isGood);
    fillHisto(m_effANT_phi, phi, isGood);
    fillHisto(m_effANT_d0, d0, isGood);
    fillHisto(m_effANT_z0, z0, isGood);
    fillHisto(m_effANT_radius, radius, isGood);
    fillHisto(m_effANT_mu, mu, isGood);
    fillHisto(m_effANT_nvertices, nVtx, isGood);
  } else if(isSTD){
    fillHisto(m_effSTD_eta, eta, isGood);
    fillHisto(m_effSTD_pt, pt, isGood);
    fillHisto(m_effSTD_phi, phi, isGood);
    fillHisto(m_effSTD_d0, d0, isGood);
    fillHisto(m_effSTD_z0, z0, isGood);
    fillHisto(m_effSTD_radius, radius, isGood);
    fillHisto(m_effSTD_mu, mu, isGood);
    fillHisto(m_effSTD_nvertices, nVtx, isGood);
  } else if(isBAT){
    fillHisto(m_effBAT_eta, eta, isGood);
    fillHisto(m_effBAT_pt, pt, isGood);
    fillHisto(m_effBAT_phi, phi, isGood);
    fillHisto(m_effBAT_d0, d0, isGood);
    fillHisto(m_effBAT_z0, z0, isGood);
    fillHisto(m_effBAT_radius, radius, isGood);
    fillHisto(m_effBAT_mu, mu, isGood);
    fillHisto(m_effBAT_nvertices, nVtx, isGood);
  }



}

void
InDetPerfPlot_AltNonprimTracking::fillFakeRate(const xAOD::TrackParticle& track, const bool isFake, const float mu, const unsigned int nVtx){

  double eta = track.eta();
  double phi = track.phi();
  double pt = track.pt() / Gaudi::Units::GeV;
  double d0 = track.d0();
  double z0 = track.z0();

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = track.patternRecoInfo();
  bool isBAT = patternInfo.test(xAOD::TrackPatternRecoInfo::TRTSeededTrackFinder);
  bool isANT = patternInfo.test(xAOD::TrackPatternRecoInfo::SiSpacePointsSeedMaker_LargeD0);
  bool isSTD = not isBAT and not isANT;


  if(isANT){
    fillHisto(m_fakeANT_eta, eta, isFake);
    fillHisto(m_fakeANT_pt, pt, isFake);
    fillHisto(m_fakeANT_phi, phi, isFake);
    fillHisto(m_fakeANT_d0, d0, isFake);
    fillHisto(m_fakeANT_z0, z0, isFake);
    fillHisto(m_fakeANT_mu, mu, isFake);
    fillHisto(m_fakeANT_nvertices, nVtx, isFake);
  } else if(isSTD){
    fillHisto(m_fakeSTD_eta, eta, isFake);
    fillHisto(m_fakeSTD_pt, pt, isFake);
    fillHisto(m_fakeSTD_phi, phi, isFake);
    fillHisto(m_fakeSTD_d0, d0, isFake);
    fillHisto(m_fakeSTD_z0, z0, isFake);
    fillHisto(m_fakeSTD_mu, mu, isFake);
    fillHisto(m_fakeSTD_nvertices, nVtx, isFake);
  } else if(isBAT){
    fillHisto(m_fakeBAT_eta, eta, isFake);
    fillHisto(m_fakeBAT_pt, pt, isFake);
    fillHisto(m_fakeBAT_phi, phi, isFake);
    fillHisto(m_fakeBAT_d0, d0, isFake);
    fillHisto(m_fakeBAT_z0, z0, isFake);
    fillHisto(m_fakeBAT_mu, mu, isFake);
    fillHisto(m_fakeBAT_nvertices, nVtx, isFake);
  }



}

void
InDetPerfPlot_AltNonprimTracking::fillUnlinked(const xAOD::TrackParticle& track, const bool isAssociatedTruth, const float mu, const unsigned int nVtx){

}


void
InDetPerfPlot_AltNonprimTracking::fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle, const float mu, const unsigned int nvertices) {

}

void
InDetPerfPlot_AltNonprimTracking::finalizePlots() {


}
