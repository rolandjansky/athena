/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_res.cxx
 * @author shaun roe
**/

#include "InDetPerfPlot_res.h"
#include <iostream>



InDetPerfPlot_res::InDetPerfPlot_res(PlotBase* pParent, const std::string & sDir)  :PlotBase(pParent, sDir), 
   m_trackEtaBins(20),
   m_etaMin(-2.5),
   m_etaMax(2.5)   
   {
//
}


void InDetPerfPlot_res::initializePlots() {
  m_mean_vs_eta_d0 = BookTProfile("mean_vs_eta_d0", "Track Measurement Bias: d_{0}versus #eta", m_trackEtaBins, m_etaMin, m_etaMax, -1.0, 1.0, false);
  m_mean_vs_eta_z0 = BookTProfile("mean_vs_eta_z0", "Track Measurement Bias: z_{0}versus #eta", m_trackEtaBins, m_etaMin, m_etaMax, -1.5, 1.5, false); 
  m_mean_vs_eta_phi = BookTProfile("mean_vs_eta_phi", "Track Measurement Bias: #phiversus #eta", m_trackEtaBins, m_etaMin, m_etaMax, -0.03, 0.03, false); 
  m_mean_vs_eta_theta = BookTProfile("mean_vs_eta_theta", "Track Measurement Bias: #thetaversus #eta", m_trackEtaBins, m_etaMin, m_etaMax, -0.1, 0.1, false);
  m_mean_vs_eta_z0st = BookTProfile("mean_vs_eta_z0st", "Track Measurement Bias: z_{0}*sin(#theta)versus #eta", m_trackEtaBins, m_etaMin, m_etaMax, -1.5, 1.5, false);
  m_mean_vs_eta_qopt = BookTProfile("mean_vs_eta_qOverP", "Track Measurement Bias: 1/p_{T}versus #eta", m_trackEtaBins, m_etaMin, m_etaMax, -0.4, 0.4, false);


  m_mean_vs_eta_d0->SetYTitle("d^{rec}_{0}-d^{tru}_{0}");
  m_mean_vs_eta_z0->SetYTitle("z^{rec}_{0}-z^{tru}_{0}");
  m_mean_vs_eta_phi->SetYTitle("#phi^{rec}-#phi^{tru}");
  m_mean_vs_eta_theta->SetYTitle("#theta^{rec}-#theta^{tru}");
  m_mean_vs_eta_z0st->SetYTitle("z_{0}sin(#theta)^{rec}-z_{0}sin(#theta)^{tru}");
  m_mean_vs_eta_qopt->SetYTitle("1-(q/p^{tru})/(q/p^{rec})");



//  m_res_vs_eta_d0 = BookTProfile("res_vs_eta_d0", "Track Resolution: d_{0}versus #eta", m_trackEtaBins, m_etaMin, m_etaMax, -1.0, 1.0, false); 
//  m_res_vs_eta_z0 = BookTProfile("res_vs_eta_z0", "Track Resolution: z_{0}versus #eta", m_trackEtaBins, m_etaMin, m_etaMax, -1.5, 1.5, false); 
//  m_res_vs_eta_phi = BookTProfile("res_vs_eta_phi", "Track Resolution: #phiversus #eta", m_trackEtaBins, m_etaMin, m_etaMax, -0.03, 0.03, false);
//  m_res_vs_eta_theta = BookTProfile("res_vs_eta_theta", "Track Resolution: #thetaversus #eta", m_trackEtaBins, m_etaMin, m_etaMax, -0.1, 0.1, false);
//  m_res_vs_eta_z0st = BookTProfile("res_vs_eta_z0st", "Track Resolution: z_{0}*sin(#theta)versus #eta", m_trackEtaBins, m_etaMin, m_etaMax, -1.5, 1.5, false); 
//  m_res_vs_eta_qopt = BookTProfile("res_vs_eta_qopt", "Track Resolution: 1/p_{T}versus #eta", m_trackEtaBins, m_etaMin, m_etaMax, -0.4, 0.4, false); 

}

void InDetPerfPlot_res::fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt) {
 
  float eta = trkprt.eta();

  float trkd0 = trkprt.d0();
  float trkz0 = trkprt.z0();
  float trkPhi0 = trkprt.phi0();
  float trkTheta = trkprt.theta();
  float trkZ0Sin = trkprt.z0() * std::sin(trkprt.theta()); //! CHECK!
  float trkqOverp = trkprt.qOverP();

  float trueTrkd0 = -9999.0;
  float trueTrkz0 = -9999.0;
  float trueTrkPhi0 = -9999.0;
  float trueTrkTheta = -9999.0;
  float trueTrkZ0Sin = -9999.0; //! CHECK!
  float trueTrkqOverp = -9999.0;


  float m_track_truth_eta = truthprt.eta();
//  int ieta = int(m_track_truth_eta*4.0 + 10.0);
  if (fabs(m_track_truth_eta)>=m_truthEtaCut)  return;

  if (truthprt.isAvailable<float>("d0")) {
    trueTrkd0 =  (truthprt.auxdata< float >("d0")); 
    m_mean_vs_eta_d0 ->Fill(eta, trkd0-trueTrkd0); 
  }
  
  if (truthprt.isAvailable<float>("z0")) {
    trueTrkz0 =  (truthprt.auxdata< float >("z0")); 
    m_mean_vs_eta_z0 ->Fill(eta, trkz0-trueTrkz0); 
  }

  if (truthprt.isAvailable<float>("phi")) {
    trueTrkPhi0 =  (truthprt.auxdata< float >("phi")); 
    m_mean_vs_eta_phi ->Fill(eta, trkPhi0-trueTrkPhi0); 
  }
  if (truthprt.isAvailable<float>("theta")) {
    trueTrkTheta =  (truthprt.auxdata< float >("theta")); 
    m_mean_vs_eta_theta ->Fill(eta, trkTheta-trueTrkTheta); 
  }
  if (truthprt.isAvailable<float>("theta") && truthprt.isAvailable<float>("z0")) {
    trueTrkZ0Sin = trueTrkz0 * std::sin(trueTrkTheta);
    m_mean_vs_eta_z0st ->Fill(eta, trkZ0Sin-trueTrkZ0Sin); 
  }
  if (truthprt.isAvailable<float>("qOverP") ) {
    trueTrkqOverp = (truthprt.auxdata< float >("qOverP"));
    m_mean_vs_eta_qopt ->Fill(eta, 1.0 - (trueTrkqOverp/trkqOverp)); 
    //std::cout<< "qoverp "<< 1.0 - (trueTrkqOverp/trkqOverp)<<std::endl;
  } 

/*
//  for (unsigned int var(0);var!=NPARAMS;++var){
    const std::string & varName = m_paramNames[var];
    const float trackParameter = trkParticleParams[var];
    const bool truthIsAvailable = truthprt.isAvailable<float>(varName);
    // get the corresponding truth variable, dont fill if doesn't exist
    if (truthIsAvailable and m_resPlots[var][ieta]){
      const float truthParameter = (truthprt.auxdata< float >(varName));
      if( var == QOVER_PT ) {
        (m_resPlots[var][ieta])->Fill( 1.0 - (truthParameter / trackParameter) );
      } else {
        (m_resPlots[var][ieta])->Fill( trackParameter - truthParameter);
      }
    }
//  }
*/

}

/*
void InDetPerfPlot_res::finalizePlots(){
    int fold= 0;
    projectStandardProfileY (m_resPlots[D0],    m_projectedPlots[D0].first,    m_projectedPlots[D0].second,    fold, 0, -3.0);
    projectStandardProfileY (m_resPlots[Z0],    m_projectedPlots[Z0].first,    m_projectedPlots[Z0].second,    fold, 0, -3.0);
    projectStandardProfileY (m_resPlots[Z0SIN_THETA],  m_projectedPlots[Z0SIN_THETA].first,  m_projectedPlots[Z0SIN_THETA].second,  fold, 0, -3.0);
    projectStandardProfileY (m_resPlots[PHI],  m_projectedPlots[PHI].first,    m_projectedPlots[PHI].second,   fold, 0, -3.0);
    projectStandardProfileY (m_resPlots[QOVER_PT],  m_projectedPlots[QOVER_PT].first,    m_projectedPlots[QOVER_PT].second,  fold, 0, -3.0);
    projectStandardProfileY (m_resPlots[THETA], m_projectedPlots[THETA].first,    m_projectedPlots[THETA].second, fold, 0, -3.0);
}
*/


