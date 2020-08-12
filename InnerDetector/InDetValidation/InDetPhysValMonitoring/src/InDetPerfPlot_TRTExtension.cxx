/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_TRTExtension.cxx
 * @author shaun roe
 **/


#include "InDetPhysValMonitoringUtilities.h"

#include "InDetPerfPlot_TRTExtension.h"

#include <cmath>

using namespace IDPVM;

InDetPerfPlot_TRTExtension::InDetPerfPlot_TRTExtension(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir),
  m_resolutionMethod(IDPVM::ResolutionHelper::iterRMS_convergence),
  m_fracTRTExtensions_vs_eta{},
  m_fracTRTExtensions_vs_pt{},
  m_fracTRTExtensions_vs_mu{},
  m_fracTRTExtensions_vs_nvertices{},
  m_fracTRTExtensions_matched_vs_eta{},
  m_fracTRTExtensions_matched_vs_pt{},
  m_chi2ndofTRTExtensions{},
  m_chi2ndofNoTRTExtensions{},
  m_ptresTRTExtensions_vs_eta{},
  m_ptresTRTExtensions_vs_pt{},
  m_ptresNoTRTExtensions_vs_eta{},
  m_ptresNoTRTExtensions_vs_pt{},
  m_reswidthTRTExtensions_vs_eta{},
  m_resmeanTRTExtensions_vs_eta{},
  m_reswidthTRTExtensions_vs_pt{},
  m_resmeanTRTExtensions_vs_pt{},
  m_reswidthNoTRTExtensions_vs_eta{},
  m_resmeanNoTRTExtensions_vs_eta{},
  m_reswidthNoTRTExtensions_vs_pt{},
  m_resmeanNoTRTExtensions_vs_pt{},
  m_ptpullTRTExtensions_vs_eta{},
  m_ptpullTRTExtensions_vs_pt{},
  m_ptpullNoTRTExtensions_vs_eta{},
  m_ptpullNoTRTExtensions_vs_pt{},
  m_pullwidthTRTExtensions_vs_eta{},
  m_pullmeanTRTExtensions_vs_eta{},
  m_pullwidthTRTExtensions_vs_pt{},
  m_pullmeanTRTExtensions_vs_pt{},
  m_pullwidthNoTRTExtensions_vs_eta{},
  m_pullmeanNoTRTExtensions_vs_eta{},
  m_pullwidthNoTRTExtensions_vs_pt{},
  m_pullmeanNoTRTExtensions_vs_pt{}


  {
}

void
InDetPerfPlot_TRTExtension::initializePlots() {

  book(m_fracTRTExtensions_vs_eta, "fracTRTExtensions_vs_eta");
  book(m_fracTRTExtensions_vs_pt, "fracTRTExtensions_vs_pt");
  book(m_fracTRTExtensions_vs_mu, "fracTRTExtensions_vs_mu");
  book(m_fracTRTExtensions_vs_nvertices, "fracTRTExtensions_vs_nvertices");

  book(m_fracTRTExtensions_matched_vs_eta, "fracTRTExtensions_matched_vs_eta");
  book(m_fracTRTExtensions_matched_vs_pt, "fracTRTExtensions_matched_vs_pt");

  book(m_chi2ndofTRTExtensions, "chi2ndofTRTExtensions");
  book(m_chi2ndofNoTRTExtensions, "chi2ndofNoTRTExtensions");

  book(m_ptresTRTExtensions_vs_eta, "ptresTRTExtensions_vs_eta");
  book(m_ptresTRTExtensions_vs_pt, "ptresTRTExtensions_vs_pt");
  book(m_ptresNoTRTExtensions_vs_eta, "ptresNoTRTExtensions_vs_eta");
  book(m_ptresNoTRTExtensions_vs_pt, "ptresNoTRTExtensions_vs_pt");

  book(m_reswidthTRTExtensions_vs_eta, "ptresolutionTRTExtensions_vs_eta");
  book(m_resmeanTRTExtensions_vs_eta, "ptresmeanTRTExtensions_vs_eta");
  book(m_reswidthTRTExtensions_vs_pt, "ptresolutionTRTExtensions_vs_pt");
  book(m_resmeanTRTExtensions_vs_pt, "ptresmeanTRTExtensions_vs_pt");

  book(m_reswidthNoTRTExtensions_vs_eta, "ptresolutionNoTRTExtensions_vs_eta");
  book(m_resmeanNoTRTExtensions_vs_eta, "ptresmeanNoTRTExtensions_vs_eta");
  book(m_reswidthNoTRTExtensions_vs_pt, "ptresolutionNoTRTExtensions_vs_pt");
  book(m_resmeanNoTRTExtensions_vs_pt, "ptresmeanNoTRTExtensions_vs_pt");


  book(m_ptpullTRTExtensions_vs_eta, "ptpullTRTExtensions_vs_eta");
  book(m_ptpullTRTExtensions_vs_pt, "ptpullTRTExtensions_vs_pt");
  book(m_ptpullNoTRTExtensions_vs_eta, "ptpullNoTRTExtensions_vs_eta");
  book(m_ptpullNoTRTExtensions_vs_pt, "ptpullNoTRTExtensions_vs_pt");

  book(m_pullwidthTRTExtensions_vs_eta, "ptpullwidthTRTExtensions_vs_eta");
  book(m_pullmeanTRTExtensions_vs_eta, "ptpullmeanTRTExtensions_vs_eta");
  book(m_pullwidthTRTExtensions_vs_pt, "ptpullwidthTRTExtensions_vs_pt");
  book(m_pullmeanTRTExtensions_vs_pt, "ptpullmeanTRTExtensions_vs_pt");

  book(m_pullwidthNoTRTExtensions_vs_eta, "ptpullwidthNoTRTExtensions_vs_eta");
  book(m_pullmeanNoTRTExtensions_vs_eta, "ptpullmeanNoTRTExtensions_vs_eta");
  book(m_pullwidthNoTRTExtensions_vs_pt, "ptpullwidthNoTRTExtensions_vs_pt");
  book(m_pullmeanNoTRTExtensions_vs_pt, "ptpullmeanNoTRTExtensions_vs_pt");

}

void
InDetPerfPlot_TRTExtension::fill(const xAOD::TrackParticle& particle) {

  double eta = particle.eta();
  double pt = particle.pt() / Gaudi::Units::GeV;
  float chi2 = particle.chiSquared();
  float ndof = particle.numberDoF();
  float chi2Overndof = ndof > 0 ? chi2 / ndof : 0;

  uint8_t iTrtHits = 0;
  particle.summaryValue(iTrtHits, xAOD::numberOfTRTHits);

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = particle.patternRecoInfo();
  bool isTRTExtension = patternInfo.test(xAOD::TrackPatternRecoInfo::InDetExtensionProcessor) or iTrtHits > 0;

  fillHisto(m_fracTRTExtensions_vs_eta, eta, isTRTExtension);
  fillHisto(m_fracTRTExtensions_vs_pt, pt, isTRTExtension);

  if(isTRTExtension) fillHisto(m_chi2ndofTRTExtensions, chi2Overndof);
  else { fillHisto(m_chi2ndofNoTRTExtensions, chi2Overndof); }

}

void
InDetPerfPlot_TRTExtension::fill(const xAOD::TrackParticle& particle, const float mu, const unsigned int nvertices) {

  uint8_t iTrtHits = 0;
  particle.summaryValue(iTrtHits, xAOD::numberOfTRTHits);

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = particle.patternRecoInfo();
  bool isTRTExtension = patternInfo.test(xAOD::TrackPatternRecoInfo::InDetExtensionProcessor) or iTrtHits > 0;

  fillHisto(m_fracTRTExtensions_vs_mu, mu, isTRTExtension);
  fillHisto(m_fracTRTExtensions_vs_nvertices, nvertices, isTRTExtension);

}

void
InDetPerfPlot_TRTExtension::fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle) {

  //Fraction of extended for truth matched tracks

  uint8_t iTrtHits = 0;
  particle.summaryValue(iTrtHits, xAOD::numberOfTRTHits);

  std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = particle.patternRecoInfo();
  bool isTRTExtension = patternInfo.test(3) or iTrtHits > 0;
  
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


  if(isTRTExtension){
    fillHisto(m_ptresTRTExtensions_vs_eta, eta, ptres);
    fillHisto(m_ptresTRTExtensions_vs_pt, pt, ptres);

    fillHisto(m_ptpullTRTExtensions_vs_eta, eta, ptpull);
    fillHisto(m_ptpullTRTExtensions_vs_pt, pt, ptpull);
  } else {
    fillHisto(m_ptresNoTRTExtensions_vs_eta, eta, ptres);
    fillHisto(m_ptresNoTRTExtensions_vs_pt, pt, ptres);

    fillHisto(m_ptpullNoTRTExtensions_vs_eta, eta, ptpull);
    fillHisto(m_ptpullNoTRTExtensions_vs_pt, pt, ptpull);
  } 

  fillHisto(m_fracTRTExtensions_matched_vs_eta, eta, isTRTExtension);
  fillHisto(m_fracTRTExtensions_matched_vs_pt, pt, isTRTExtension);


}


void
InDetPerfPlot_TRTExtension::finalizePlots() {

  m_resolutionHelper.makeResolutions(m_ptresTRTExtensions_vs_eta, m_reswidthTRTExtensions_vs_eta, m_resmeanTRTExtensions_vs_eta, m_resolutionMethod); 
  m_resolutionHelper.makeResolutions(m_ptresTRTExtensions_vs_pt, m_reswidthTRTExtensions_vs_pt, m_resmeanTRTExtensions_vs_pt, m_resolutionMethod);
  m_resolutionHelper.makeResolutions(m_ptresNoTRTExtensions_vs_eta, m_reswidthNoTRTExtensions_vs_eta, m_resmeanNoTRTExtensions_vs_eta, m_resolutionMethod);
  m_resolutionHelper.makeResolutions(m_ptresNoTRTExtensions_vs_pt, m_reswidthNoTRTExtensions_vs_pt, m_resmeanNoTRTExtensions_vs_pt, m_resolutionMethod);


  m_resolutionHelper.makeResolutions(m_ptpullTRTExtensions_vs_eta, m_pullwidthTRTExtensions_vs_eta, m_pullmeanTRTExtensions_vs_eta, m_resolutionMethod); 
  m_resolutionHelper.makeResolutions(m_ptpullTRTExtensions_vs_pt, m_pullwidthTRTExtensions_vs_pt, m_pullmeanTRTExtensions_vs_pt, m_resolutionMethod);
  m_resolutionHelper.makeResolutions(m_ptpullNoTRTExtensions_vs_eta, m_pullwidthNoTRTExtensions_vs_eta, m_pullmeanNoTRTExtensions_vs_eta, m_resolutionMethod);
  m_resolutionHelper.makeResolutions(m_ptpullNoTRTExtensions_vs_pt, m_pullwidthNoTRTExtensions_vs_pt, m_pullmeanNoTRTExtensions_vs_pt, m_resolutionMethod);

}
