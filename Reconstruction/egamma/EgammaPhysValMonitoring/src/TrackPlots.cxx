/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackPlots.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
 
namespace Egamma{

TrackPlots::TrackPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir), 
										       m_sParticleType(sParticleType),
										       deta(0),
										       dphi(0),
										       blayer(0), 
										       pixel(0),
										       sct(0),
										       si(0),
										       trt(0),
										       trtratio(0)

{}	

void TrackPlots::initializePlots(){
  deta    = Book1D("deta", "#Delta#eta_{1} of "+m_sParticleType+";#Delta#eta_{1};Electrons", 100,-0.01, 0.01);           
  dphi    = Book1D("dphi", "#Delta#varphi_{2} of "+m_sParticleType+";#Delta#varphi_{2};Electrons", 100,-0.01, 0.01);           
  blayer  = Book1D("blayer", "#blayer hits of "+m_sParticleType+";# blayer hits;Electrons", 5,0, 5);
  pixel   = Book1D("pixel", "#pixel hits of "+m_sParticleType+";# pixel hits;Electrons", 10, 0, 10);
  sct     = Book1D("sct", "#sct hits of "+m_sParticleType+";# sct hits;Electrons", 25, 0, 25);
  si      = Book1D("si", "#pixel+sct hits of "+m_sParticleType+";# pixel+sct hits;Electrons", 25, 0, 25);
  trt     = Book1D("trt", "#trt hits of "+m_sParticleType+";# trt hits;Electrons", 50, 0, 50);
  trtratio = Book2D("trtratio", "TRT ratio vs eta of"+m_sParticleType+";#eta; TRT ratio", 50, -2.5, 2.5,100,0.,1.);
}


void TrackPlots::fill(const xAOD::Electron& electron){

  float deta1(0), dphi2 (0);
  if(electron.trackCaloMatchValue(deta1, xAOD::EgammaParameters::deltaEta1 )){
    deta->Fill(deta1);
  }
  if(electron.trackCaloMatchValue(dphi2, xAOD::EgammaParameters::deltaPhi2 )){
    dphi->Fill(dphi2*electron.trackParticle()->charge());
  }

  if(!electron.trackParticle())  return;

  blayer->Fill(RetrieveHitInfo(electron, xAOD::numberOfInnermostPixelLayerHits));
  pixel->Fill(RetrieveHitInfo(electron, xAOD::numberOfPixelHits));
  sct->Fill(RetrieveHitInfo(electron, xAOD::numberOfSCTHits));
  si->Fill(RetrieveHitInfo(electron, xAOD::numberOfPixelHits) + RetrieveHitInfo(electron, xAOD::numberOfSCTHits));

  uint8_t numTotalTRTHits = RetrieveHitInfo(electron, xAOD::numberOfTRTHits) + RetrieveHitInfo(electron, xAOD::numberOfTRTOutliers);
  uint8_t numTotalTRTHighThresholdHits = RetrieveHitInfo(electron, xAOD::numberOfTRTHighThresholdHits) + RetrieveHitInfo(electron, xAOD::numberOfTRTHighThresholdOutliers);
  float totalTRTratio= (numTotalTRTHits != 0) ? float(numTotalTRTHighThresholdHits)/float(numTotalTRTHits) : 0;
  trt->Fill(numTotalTRTHits);
  trtratio->Fill(electron.trackParticle()->eta(),totalTRTratio);
}

}
