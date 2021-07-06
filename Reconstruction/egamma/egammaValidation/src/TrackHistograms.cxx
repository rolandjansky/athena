/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackHistograms.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

using namespace egammaMonitoring;
using xAOD::EgammaHelpers::summaryValueFloat;

StatusCode TrackHistograms::initializePlots() {
  
  histoMap["pT"] = (new TH1D(Form("%s_%s",m_name.c_str(),"pT"), "Tracks ;p_{T} [GeV]", 100, 0., 100.));

  histoMap["PrecisionHitFraction"] = (new TH1D(Form("%s_%s",m_name.c_str(),"PrecisionHitFraction"), "Tracks ;Precision hit fraction", 20, 0., 1.));
  histoMap["PrecisionHitFraction_lowmu"] = (new TH1D(Form("%s_%s",m_name.c_str(),"PrecisionHitFraction_lowmu"), "Tracks ;Precision hit fraction", 20, 0., 1.));
  histoMap["PrecisionHitFraction_highmu"] = (new TH1D(Form("%s_%s",m_name.c_str(),"PrecisionHitFraction_highmu"), "Tracks ;Precision hit fraction", 20, 0., 1.));

  histoMap["eProbabilityHT"] = (new TH1D(Form("%s_%s",m_name.c_str(),"eProbabilityHT"), "Tracks ;eProbabilityHT", 20, 0., 1.));
  histoMap["eProbabilityHT_lowmu"] = (new TH1D(Form("%s_%s",m_name.c_str(),"eProbabilityHT_lowmu"), "Tracks ;eProbabilityHT", 20, 0., 1.));
  histoMap["eProbabilityHT_highmu"] = (new TH1D(Form("%s_%s",m_name.c_str(),"eProbabilityHT_highmu"), "Tracks ;eProbabilityHT", 20, 0., 1.));

  profileMap["PrecisionHitFractionvsmu"] = (new TProfile(Form("%s_%s",m_name.c_str(),"PrecisionHitFractionvsmu"), "Precision hit fraction;mu", 35, 0., 70., 0., 1.));
  profileMap["eProbabilityHTvsmu"] = (new TProfile(Form("%s_%s",m_name.c_str(),"eProbabilityHTvsmu"), "eProbabilityHT;mu", 35, 0., 70., 0., 1.));


  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"pT", histoMap["pT"]));
 
 ATH_CHECK(m_rootHistSvc->regHist(m_folder+"PrecisionHitFraction", histoMap["PrecisionHitFraction"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"PrecisionHitFraction_lowmu", histoMap["PrecisionHitFraction_lowmu"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"PrecisionHitFraction_highmu", histoMap["PrecisionHitFraction_highmu"]));

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"eProbabilityHT", histoMap["eProbabilityHT"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"eProbabilityHT_lowmu", histoMap["eProbabilityHT_lowmu"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"eProbabilityHT_highmu", histoMap["eProbabilityHT_highmu"]));

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"PrecisionHitFractionvsmu", profileMap["PrecisionHitFractionvsmu"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"eProbabilityHTvsmu", profileMap["eProbabilityHTvsmu"]));



  return StatusCode::SUCCESS;
  
} // initializePlots


void TrackHistograms::fill(const xAOD::IParticle& track) {
  TrackHistograms::fill(track,0.);
}

void TrackHistograms::fill(const xAOD::IParticle& track, float mu) {

  xAOD::TrackParticle* tp = dynamic_cast<xAOD::TrackParticle*>(const_cast<xAOD::IParticle*>(&track));
  
  int nTRTHits = getNumberOfHits(tp, xAOD::numberOfTRTHits);
  int nTRTTubeHits = getNumberOfHits(tp, xAOD::numberOfTRTTubeHits);
  float precHitFrac = (nTRTHits>0 && nTRTTubeHits>=0) ? (1. - ((float)nTRTTubeHits)/((float)nTRTHits)) : -999.;

  float eProbabilityHT = summaryValueFloat(*tp, xAOD::eProbabilityHT);

  histoMap["pT"]->Fill(tp->pt()/1000.);

  histoMap["PrecisionHitFraction"]->Fill(precHitFrac);
  histoMap["eProbabilityHT"]->Fill(eProbabilityHT);
  
  if(mu<25.){
    histoMap["PrecisionHitFraction_lowmu"]->Fill(precHitFrac);
    histoMap["eProbabilityHT_lowmu"]->Fill(eProbabilityHT);
  }

  if(mu>35.){
    histoMap["PrecisionHitFraction_highmu"]->Fill(precHitFrac);
    histoMap["eProbabilityHT_highmu"]->Fill(eProbabilityHT);
  }

    profileMap["PrecisionHitFractionvsmu"]->Fill(mu, precHitFrac);
    profileMap["eProbabilityHTvsmu"]->Fill(mu, eProbabilityHT);

}


int TrackHistograms::getNumberOfHits(const xAOD::TrackParticle* tp, xAOD::SummaryType info){

  uint8_t nHits = 0;
  return tp->summaryValue(nHits, info) ? nHits : -999;

}
