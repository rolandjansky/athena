/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *
 author: Max Baugh
 15/9/15
 *
 */

#include "InDet_BadMatchRate.h"
#include "xAODTracking/TrackingPrimitives.h"
#include <utility>

using namespace TMath;

InDet_BadMatchRate::InDet_BadMatchRate(InDetPlotBase* pParent, std::string sDir):InDetPlotBase(pParent, sDir)
  {										
  //
}


void 
InDet_BadMatchRate::initializePlots(){
  //Bad Match Rate plots, Truth Matching Probability < 80%
  book(m_BadMatchRate, "BadMatchRate");
  book(m_BMR_vs_logpt, "BadMatchRate_vs_logpt");

  //Fake Rate plots, TMP < 50%
  //book(m_trackeff_vs_eta, "trackeff_vs_eta");

  //Really Fake Rate plots, Truth Matching Probability < 20%
  book(m_ReallyFakeRate, "ReallyFakeRate");

}

void
InDet_BadMatchRate::fillBMR(const xAOD::TrackParticle& particle, float weight){
  float trketa = particle.eta();
  float trkpt = particle.pt();
  float logpt = Log10(trkpt) - 3.0; //-3 converts from MeV to GeV

  m_BadMatchRate->Fill(trketa, weight);
  m_BMR_vs_logpt->Fill(logpt, weight);
}

void
InDet_BadMatchRate::fillRF(const xAOD::TrackParticle& particle, Float_t weight){
  float trketa = particle.eta();
  //float trkpt = particle.pt();//unused

  m_ReallyFakeRate->Fill(trketa, weight);
}

/*
void InDet_BadMatchRate::finalizePlots(){
}
*/ 
