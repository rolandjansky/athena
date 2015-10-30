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


InDet_BadMatchRate::InDet_BadMatchRate(InDetPlotBase* pParent, std::string sDir):InDetPlotBase(pParent, sDir)
{
  //nop
}


void 
InDet_BadMatchRate::initializePlots(){
  book(m_base,"base");
  book(m_badMatchRateNumerator, "badMatchRateNumerator");
  book(m_reallyFakeNumerator,"reallyFakeNumerator");
  book(m_badMatchRate,"badMatchRate");
  book(m_reallyFakeRate,"reallyFakeRate");
}


void
InDet_BadMatchRate::fillDenominator(const xAOD::TrackParticle& particle){
  const float & eta = particle.eta();
  m_base->Fill(eta, 1);
}

void
InDet_BadMatchRate::fillNumBMR(const xAOD::TrackParticle& particle){
  const float & eta = particle.eta();
  m_badMatchRateNumerator->Fill(eta, 1);
}

void
InDet_BadMatchRate::fillNumRF(const xAOD::TrackParticle& particle){
  const float & eta = particle.eta();
  m_reallyFakeNumerator->Fill(eta, 1);
}


void InDet_BadMatchRate::finalizePlots(){
  for(int i=0; i!=m_trackEtaBins; ++i){
    const double step = (m_etaMax - m_etaMin)/m_trackEtaBins;
    const double eta = m_etaMin + i*step;
    const double denom = m_base->GetBinEntries(i);
    const double num_bmr = m_badMatchRateNumerator->GetBinEntries(i);
    const double num_rf = m_reallyFakeNumerator->GetBinEntries(i);
    const double BMRate = num_bmr/denom;
    const double fakeRate = num_rf/denom;
    m_badMatchRate->Fill(eta, BMRate);
    m_reallyFakeRate->Fill(eta, fakeRate);
  }

  //Remove the intermediary histograms
  //Base->Delete();

  //delete Base;
  //NumBMR->Delete(); //Is this the proper command?
  //delete NumRF;     //Or is this one?

  //Both methods seem to cause seg-faults, but it looks like the problem is a bigger ROOT-issue
  //It doesn't seg-fault until after this script is completed, so something about removing histos
  //from the big file must be the cause of the problem
}
