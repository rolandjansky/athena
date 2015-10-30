/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPerfPlot_pull.h"
#include "xAODTracking/TrackingPrimitives.h"


InDetPerfPlot_pull::InDetPerfPlot_pull(InDetPlotBase* pParent, const std::string & sDir)
   :InDetPlotBase(pParent, sDir), 
   m_pullPlots(NPARAMS),
   m_pullPlotsWide(NPARAMS),
   m_paramNames{"d0","z0", "phi", "theta","qOverP"}
   {
//
}


void 
InDetPerfPlot_pull::initializePlots() {
  //Initialize the pull histograms: 
  //number of bins for each parameter
  //upper and lower histogram limits, from existing RTT code now in hdef file
  for (unsigned int var(0);var!=NPARAMS;++var){
    //no need to 'push_back'; the vectors were default filled at instantiation of the class
    book(m_pullPlots[var],std::string("pull")+m_paramNames[var]+std::string("all"));
    book(m_pullPlotsWide[var],std::string("pull")+m_paramNames[var]+std::string("all_wide"));
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
  trkParticleParams[QOVERP] = trkprt.qOverP();
  
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
      (m_pullPlotsWide[var])->Fill( (trackParameter - truthParameter)/sigma);
    } 
  }
}


void InDetPerfPlot_pull::finalizePlots(){
    //nop
}

