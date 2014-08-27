/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPerfPlot_pull.h"
#include "xAODTracking/TrackingPrimitives.h"


InDetPerfPlot_pull::InDetPerfPlot_pull(PlotBase* pParent, const std::string & sDir)
   :PlotBase(pParent, sDir), 
   m_pullPlots(NPARAMS),
   m_pullPlotsWide(NPARAMS),
   m_paramNames{"d0","z0", "phi", "theta","qopt"}
   {
//
}


void 
InDetPerfPlot_pull::initializePlots() {
  //never prepend the directory path to the histo variable names
  const bool prependDirectory(false);
  //Initialize the pull histograms: 
  //number of bins for each parameter
  //upper and lower histogram limits, from existing RTT code
  typedef std::pair<float, float> Limits_t;
  unsigned int nBins(200);
  Limits_t allLimits = Limits_t(-5.0, 5.0);
  Limits_t allWideLimits = Limits_t(-20.0, 20.0);
  //

  for (unsigned int var(0);var!=NPARAMS;++var){
    std::string hName = formName(var);
    std::string tName = formTitle(var);
    TH1 * pHisto = Book1D(hName,tName,nBins, allLimits.first, allLimits.second, prependDirectory);
    std::string wName = hName + std::string("_widerange");
    TH1 * pwideHisto = Book1D(wName,tName,nBins, allWideLimits.first, allWideLimits.second, prependDirectory);
    //we already initialised the vector in the initialiser list, so don't push_back
    m_pullPlots[var] = pHisto;
    m_pullPlotsWide[var] = pwideHisto;
  } 
}

void 
InDetPerfPlot_pull::fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt) {
  //
  //get the track particle parameters needed into a simple array
  //can use 'defining parameters' ?
  float trkParticleParams[NPARAMS];
  trkParticleParams[D0] = trkprt.d0();
  trkParticleParams[Z0] = trkprt.z0();
  trkParticleParams[PHI] = trkprt.phi0();
  trkParticleParams[THETA] = trkprt.theta();
  
  //checked the following, at line 2519 of IDStandardPerformance.cxx
  //https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetValidation/InDetPerformanceRTT/tags/InDetPerformanceRTT-01-00-05/src
  trkParticleParams[QOVER_PT] = trkprt.qOverP();
  
  //
  //const float unfilledValue(0.0); //should be NaN?
  for (unsigned int var(0);var!=NPARAMS;++var){
    const std::string & varName = m_paramNames[var];
    const std::string errName = varName + std::string("err");
    const float trackParameter = trkParticleParams[var];
    const bool truthIsAvailable = truthprt.isAvailable<float>(varName);
    const bool sigmaAvailable = trkprt.isAvailable<float>(errName);
    
    
    // get the corresponding truth variable, dont fill if doesn't exist
    if (truthIsAvailable and sigmaAvailable and m_pullPlots[var]){
      const float truthParameter = (truthprt.auxdata< float >(varName));
      const float sigma = (trkprt.auxdata< float >(errName));
      (m_pullPlots[var])->Fill( (trackParameter - truthParameter) / sigma);
    } 
  }
}


void InDetPerfPlot_pull::finalizePlots(){
    //nop
}

std::string
InDetPerfPlot_pull::formName(const unsigned int p) const {
  return  std::string("pull")+m_paramNames[p] +std::string("all");
}


std::string
InDetPerfPlot_pull::formTitle(const unsigned int param) const {
  //should use formatting/regex
  static std::string titleComponents[NPARAMS][2] = {
    {"d_{0};", "(d^{rec}_{0}-d^{tru}_{0})/#sigma_{d_{0}}"},
    {"z_{0};", "(z^{rec}_{0}-z^{tru}_{0})/#sigma_{z_{0}}"},
    {"#phi;", "(#phi^{rec}-#phi^{tru})/#sigma_{#phi}"},
    {"#theta;","(#theta^{rec}-#theta^{tru})/#sigma_{#theta}"},
    {"q/p_{T};", "(q/p_{T}^{rec}-q/p_{T}^{tru})/#sigma_{q/p_{T}}"}
  };
  static std::string titlePrefix("Pull Distribution of ");

  return titlePrefix  + titleComponents[param][0] +  titleComponents[param][1];
}