/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ClusterHistograms.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

using namespace egammaMonitoring;

StatusCode ClusterHistograms::initializePlots() {
  
  histo2DMap["Eraw_Etruth_vs_Etruth_2D"] = (new TH2D(Form("%s_%s",m_name.c_str(),"Eraw_Etruth_vs_Etruth_2D"), ";E^{truth};E^{raw}/E^{truth}", 100,0.,200.,50, 0.5, 1.5));
  histo2DMap["Eraw_Etruth_vs_eta_2D"] = (new TH2D(Form("%s_%s",m_name.c_str(),"Eraw_Etruth_vs_eta_2D"), ";truth #eta;E^{raw}/E^{truth}", 40,           -3,           3, 50, 0.5, 1.5));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"Eraw_Etruth_vs_Etruth_2D", histo2DMap["Eraw_Etruth_vs_Etruth_2D"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"Eraw_Etruth_vs_eta_2D", histo2DMap["Eraw_Etruth_vs_eta_2D"]));
  return StatusCode::SUCCESS;
  
} // initializePlots

void ClusterHistograms::fill(const xAOD::Egamma& egamma) {

  const xAOD::CaloCluster *cluster = egamma.caloCluster();
  const xAOD::TruthParticle *truth_egamma = xAOD::TruthHelpers::getTruthParticle(egamma);
  if (truth_egamma) {
    histo2DMap["Eraw_Etruth_vs_Etruth_2D"]->Fill(truth_egamma->e()/1000,cluster->rawE()/truth_egamma->e());
    histo2DMap["Eraw_Etruth_vs_eta_2D"]->Fill(truth_egamma->eta(),cluster->rawE()/truth_egamma->e());

  }
}
