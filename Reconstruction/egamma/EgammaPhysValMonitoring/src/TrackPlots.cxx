/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "TrackPlots.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
 
namespace Egamma{

TrackPlots::TrackPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType):PlotBase(pParent, sDir), 
										       m_sParticleType(std::move(sParticleType)),
										       deta(nullptr),
										       dphi(nullptr),
										       d0(nullptr),
										       z0(nullptr),
										       d0significance(nullptr),
										       blayer(nullptr), 
										       pixel(nullptr),
										       sct(nullptr),
										       si(nullptr),
										       trt(nullptr),
										       trt_xe(nullptr),
										       trt_total(nullptr),
										       trt_ht(nullptr),
										       trt_ht_total(nullptr),
										       dphirescaled(nullptr),
										       eProbHT(nullptr),
										       deltaPoverP(nullptr),
										       EoverP(nullptr),
										       trtratio(nullptr),
                           trtvseta(nullptr),
                           trthtvseta(nullptr)

{}	

void TrackPlots::initializePlots(){
  deta    = Book1D("deta", "#Delta#eta_{1} of "+m_sParticleType+";#Delta#eta_{1};Electrons", 100,-0.01, 0.01);           
  dphi    = Book1D("dphi", "#Delta#varphi_{2} of "+m_sParticleType+";#Delta#varphi_{2};Electrons", 100,-0.01, 0.01);
  dphirescaled = Book1D("dphirescaled", "#Delta#varphiRescaled_{2} of "+m_sParticleType+";#Delta#varphiRescaled_{2};Electrons",100,-0.01,0.01);
  d0           = Book1D("d0", "d0 of "+m_sParticleType+"; d0;Electrons", 250, 0, 25);
  z0           = Book1D("z0", "z0 of "+m_sParticleType+"; z0;Electrons", 250, 0, 25);
  d0significance = Book1D("d0significance", "d0significance of "+m_sParticleType+"; d0significance;Electrons", 250, 0, 25);
  blayer  = Book1D("blayer", "#blayer hits of "+m_sParticleType+";# blayer hits;Electrons", 5,0, 5);
  pixel   = Book1D("pixel", "#pixel hits of "+m_sParticleType+";# pixel hits;Electrons", 10, 0, 10);
  sct     = Book1D("sct", "#sct hits of "+m_sParticleType+";# sct hits;Electrons", 25, 0, 25);
  si      = Book1D("si", "#pixel+sct hits of "+m_sParticleType+";# pixel+sct hits;Electrons", 25, 0, 25);
  trt     = Book1D("trt", "#trt hits of "+m_sParticleType+";# trt hits;Electrons", 50, 0, 50);
  trt_xe       = Book1D("trt_xe", "#trt xe hits of "+m_sParticleType+";# trt xe hits;Electrons", 50, 0, 50);
  trt_total    = Book1D("trt_total", "#total trt hits of "+m_sParticleType+";# total trt hits;Electrons", 50, 0, 50);
  trt_ht       = Book1D("trt_ht", "#HT trt hits of "+m_sParticleType+";# HT trt hits;Electrons", 50, 0, 50);
  trt_ht_total = Book1D("trt_ht_total", "#HT total trt hits of "+m_sParticleType+";# total HT trt hits;Electrons", 50, 0, 50);
  eProbHT      = Book1D("eProbabilityHT", "#eProbablity HT of "+m_sParticleType+";#eProbablity HT;Electrons", 50, 0, 5);
  deltaPoverP  = Book1D("deltaPoverP", "#DeltaP/P of "+m_sParticleType+";#DeltaP/P;Electrons", 100,-1.1, 1.1);           
  EoverP       = Book1D("EoverP", "E/P of "+m_sParticleType+";E/P;Electrons", 100, 0, 10);
  trtratio = Book2D("trtratio", "TRT ratio vs eta of"+m_sParticleType+";#eta; TRT ratio", 50, -2.5, 2.5,100,0.,1.);
  trtvseta = Book2D("trtvseta", "#trt hits vs eta of"+m_sParticleType+";#eta; #trt hits", 50, -2.5, 2.5,50,0.,50.);
  trthtvseta = Book2D("trthtvseta", "#HT trt hits vs eta of"+m_sParticleType+";#eta; #HT trt hits", 50, -2.5, 2.5,50,0.,50.);
}


  void TrackPlots::fill(const xAOD::Electron& electron, const xAOD::EventInfo& eventInfo) const{

  float deta1(0);
  float dphi2 (0);
  float dphirescaled2(0);
  float eprobht(0);
  float trackqoverp(0);
  float eoverp(0);
  double dpOverp(0);

  float weight = 1.;
  weight = eventInfo.beamSpotWeight();

  if(electron.trackCaloMatchValue(deta1, xAOD::EgammaParameters::deltaEta1 )){
    deta->Fill(deta1, weight);
  }
  if(electron.trackCaloMatchValue(dphi2, xAOD::EgammaParameters::deltaPhi2 )){
    dphi->Fill(dphi2*electron.trackParticle()->charge(), weight);
  }
  if(electron.trackCaloMatchValue(dphirescaled2, xAOD::EgammaParameters::deltaPhiRescaled2 )){
    dphirescaled->Fill(dphirescaled2, weight);
  }
  if(!electron.trackParticle())  return;
  trackqoverp = electron.trackParticle()->qOverP();

  if(electron.caloCluster()){
    const double energy =  electron.caloCluster()->e();
    eoverp=std::abs(trackqoverp)*energy;
    EoverP->Fill(eoverp, weight);
  }
  unsigned int index;
  if( electron.trackParticle()->indexOfParameterAtPosition(index, xAOD::LastMeasurement) ) {
	  
    double refittedTrack_LMqoverp  = 
      electron.trackParticle()->charge() / std::sqrt(std::pow(electron.trackParticle()->parameterPX(index), 2) +
						std::pow(electron.trackParticle()->parameterPY(index), 2) +
						std::pow(electron.trackParticle()->parameterPZ(index), 2));
	  
    dpOverp = 1 - trackqoverp/(refittedTrack_LMqoverp);
    deltaPoverP->Fill(dpOverp, weight);
  }
	
  blayer->Fill(RetrieveHitInfo(electron, xAOD::numberOfInnermostPixelLayerHits), weight);
  pixel->Fill(RetrieveHitInfo(electron, xAOD::numberOfPixelHits), weight);
  sct->Fill(RetrieveHitInfo(electron, xAOD::numberOfSCTHits), weight);
  si->Fill(RetrieveHitInfo(electron, xAOD::numberOfPixelHits) + RetrieveHitInfo(electron, xAOD::numberOfSCTHits), weight);

  eprobht=electron.trackParticleSummaryValue(eprobht,xAOD::eProbabilityHT);
  eProbHT->Fill(eprobht, weight);

  uint8_t numTotalTRTHits = RetrieveHitInfo(electron, xAOD::numberOfTRTHits) + RetrieveHitInfo(electron, xAOD::numberOfTRTOutliers);
  uint8_t numTotalTRTHighThresholdHits = RetrieveHitInfo(electron, xAOD::numberOfTRTHighThresholdHits) + RetrieveHitInfo(electron, xAOD::numberOfTRTHighThresholdOutliers);
  float totalTRTratio= (numTotalTRTHits != 0) ? float(numTotalTRTHighThresholdHits)/float(numTotalTRTHits) : 0;
  trt->Fill(RetrieveHitInfo(electron, xAOD::numberOfTRTHits), weight);
  trt_xe->Fill(RetrieveHitInfo(electron, xAOD::numberOfTRTXenonHits), weight);
  trt_ht->Fill(RetrieveHitInfo(electron, xAOD::numberOfTRTHighThresholdHits), weight);
  trt_total->Fill(numTotalTRTHits, weight);
  trt_ht_total->Fill(numTotalTRTHighThresholdHits, weight);
  trtratio->Fill(electron.trackParticle()->eta(),totalTRTratio, weight);
  trtvseta->Fill(electron.trackParticle()->eta(),RetrieveHitInfo(electron, xAOD::numberOfTRTHits), weight);
  trthtvseta->Fill(electron.trackParticle()->eta(),RetrieveHitInfo(electron, xAOD::numberOfTRTHighThresholdHits), weight);

  d0->Fill(electron.trackParticle()->d0(), weight);
  z0->Fill(electron.trackParticle()->z0(), weight);

  double d0sig = xAOD::TrackingHelpers::d0significance( electron.trackParticle() , eventInfo.beamPosSigmaX(), eventInfo.beamPosSigmaY(), eventInfo.beamPosSigmaXY() );
  d0significance->Fill(d0sig, weight);
}

}
