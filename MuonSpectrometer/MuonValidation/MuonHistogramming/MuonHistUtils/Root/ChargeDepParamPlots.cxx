/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "MuonHistUtils/ChargeDepParamPlots.h"

namespace Muon{

ChargeDepParamPlots::ChargeDepParamPlots(PlotBase* pParent, const std::string& sDir, std::string sType)
  :PlotBase(pParent, sDir)  
  , m_sType(std::move(sType))
{}

void ChargeDepParamPlots::initializePlots() {
  q_times_eta = Book1D("qTimesEta_"+m_sType,"qTimesEta_"+m_sType+";charge*Pseudo-Rapidity;Entries", 128, -3.2, 3.2);
  q_over_pt   = Book1D("qOverPt_"+m_sType ,"qOverPt_"+m_sType+";charge over Transverse Momentum [GeV^{-1}];Entries",200,-0.5,0.5);
  q_over_p   = Book1D("qOverP_"+m_sType, "qOverP_"+m_sType+";charge over Momentum [GeV^{-1}];Entries",200,-0.5,0.5);

  //charge comparison of ID, MS, ME tracks
  qMS_times_qCB = Book1D("qMS_times_qCB_"+m_sType,"qMS_times_qCB_"+m_sType+";qMS*qCB;Entries",3,-1.5,1.5);
  qID_times_qCB = Book1D("qID_times_qCB_"+m_sType,"qID_times_qCB_"+m_sType+";qID*qCB;Entries",3,-1.5,1.5);
}


  void ChargeDepParamPlots::fill(const xAOD::Muon& mu, float weight)
{
  const xAOD::TrackParticle* trkprt = mu.trackParticle(xAOD::Muon::Primary);
  if (!trkprt) return;
    
  q_times_eta->Fill(trkprt->charge()*trkprt->eta(),weight);
  q_over_pt->Fill(trkprt->charge()/trkprt->pt()/0.001, weight);
  q_over_p->Fill(trkprt->qOverP()/0.001, weight);
     
  const xAOD::TrackParticle* trkprtID = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  const xAOD::TrackParticle* trkprtMS = mu.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);

  if (trkprtID)
    qID_times_qCB->Fill(trkprtID->charge()*trkprt->charge(),weight);
  
  if (trkprtMS) 
    qMS_times_qCB->Fill(trkprtMS->charge()*trkprt->charge(),weight);  
}

}
