/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ClusterHistograms.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

using namespace egammaMonitoring;

StatusCode ClusterHistograms::initializePlots() {
  
  profileMap["Eraw_Etruth_vs_Etruth_profile"] = (new TProfile(Form("%s_%s",m_name.c_str(),"Eraw_Etruth_vs_Etruth_profile"), ";E^{truth};E^{raw}/E^{truth}", 100,0.,200., 0.5,1.5));
  profileMap["Eraw_Etruth_vs_eta_profile"] = (new TProfile(Form("%s_%s",m_name.c_str(),"Eraw_Etruth_vs_eta_profile"), ";truth #eta;E^{raw}/E^{truth}", 40,-3,3, 0.5,1.5));
  profileMap["number_cells_vs_et_in_layer_0_profile"] = (new TProfile(Form("%s_%s",m_name.c_str(),"number_cells_vs_et_in_layer_0_profile"), "Number of cells;E_{T}", 60,0,300, 0,100.));
  profileMap["number_cells_vs_et_in_layer_1_profile"] = (new TProfile(Form("%s_%s",m_name.c_str(),"number_cells_vs_et_in_layer_1_profile"), "Number of cells;E_{T}", 60,0,300, 0,100.));
  profileMap["number_cells_vs_et_in_layer_2_profile"] = (new TProfile(Form("%s_%s",m_name.c_str(),"number_cells_vs_et_in_layer_2_profile"), "Number of cells;E_{T}", 60,0,300, 0,100.));
  profileMap["number_cells_vs_et_in_layer_3_profile"] = (new TProfile(Form("%s_%s",m_name.c_str(),"number_cells_vs_et_in_layer_3_profile"), "Number of cells;E_{T}", 60,0,300, 0,100.));

  profileMap["number_cells_vs_eta_in_layer_0_profile"] = (new TProfile(Form("%s_%s",m_name.c_str(),"number_cells_vs_eta_in_layer_0_profile"), "Number of cells;truth #eta", 40,-3,3, 0,100));
  profileMap["number_cells_vs_eta_in_layer_1_profile"] = (new TProfile(Form("%s_%s",m_name.c_str(),"number_cells_vs_eta_in_layer_1_profile"), "Number of cells;truth #eta", 40,-3,3, 0,100));
  profileMap["number_cells_vs_eta_in_layer_2_profile"] = (new TProfile(Form("%s_%s",m_name.c_str(),"number_cells_vs_eta_in_layer_2_profile"), "Number of cells;truth #eta", 40,-3,3, 0,100));
  profileMap["number_cells_vs_eta_in_layer_3_profile"] = (new TProfile(Form("%s_%s",m_name.c_str(),"number_cells_vs_eta_in_layer_3_profile"), "Number of cells;truth #eta", 40,-3,3, 0,100));

  profileMap["number_topocluster_vs_et_profile"] = (new TProfile(Form("%s_%s",m_name.c_str(),"number_topocluster_vs_et_profile"), "Number of topocluster;E_{T}", 60,0,300, -0.5,14.5));
  profileMap["number_topocluster_vs_eta_profile"] = (new TProfile(Form("%s_%s",m_name.c_str(),"number_topocluster_vs_eta_profile"), "Number of topocluster;truth #eta", 40,-3,3, -0.5,14.5));

  profileMap["number_cell_in_layer"] = (new TProfile(Form("%s_%s",m_name.c_str(),"number_cell_in_layer"), ";Number of cells;Layer",100,0,200, 0,4));  
  histo2DMap["mu_energy_resolution_2D"] = (new TH2D(Form("%s_%s",m_name.c_str(),"mu_energy_resolution_2D"), ";<#mu>; Energy Resolution", 5,0,80, 20,-1,1));

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"Eraw_Etruth_vs_Etruth_profile", profileMap["Eraw_Etruth_vs_Etruth_profile"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"Eraw_Etruth_vs_eta_profile", profileMap["Eraw_Etruth_vs_eta_profile"]));

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_cells_vs_et_in_layer_0_profile", profileMap["number_cells_vs_et_in_layer_0_profile"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_cells_vs_et_in_layer_1_profile", profileMap["number_cells_vs_et_in_layer_1_profile"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_cells_vs_et_in_layer_2_profile", profileMap["number_cells_vs_et_in_layer_2_profile"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_cells_vs_et_in_layer_3_profile", profileMap["number_cells_vs_et_in_layer_3_profile"]));
 
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_cells_vs_eta_in_layer_0_profile", profileMap["number_cells_vs_eta_in_layer_0_profile"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_cells_vs_eta_in_layer_1_profile", profileMap["number_cells_vs_eta_in_layer_1_profile"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_cells_vs_eta_in_layer_2_profile", profileMap["number_cells_vs_eta_in_layer_2_profile"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_cells_vs_eta_in_layer_3_profile", profileMap["number_cells_vs_eta_in_layer_3_profile"]));

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_topocluster_vs_et_profile", profileMap["number_topocluster_vs_et_profile"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_topocluster_vs_eta_profile", profileMap["number_topocluster_vs_eta_profile"]));

 
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_cell_in_layer", profileMap["number_cell_in_layer"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"mu_energy_resolution_2D", histo2DMap["mu_energy_resolution_2D"]));



  return StatusCode::SUCCESS;
  
} // initializePlots

void ClusterHistograms::fill(const xAOD::Egamma& egamma) {
  fill(egamma,0.);
}

void ClusterHistograms::fill(const xAOD::Egamma& egamma, float mu = 0) {

  const xAOD::CaloCluster *cluster = egamma.caloCluster();
  
  const xAOD::TruthParticle *truth_egamma = xAOD::TruthHelpers::getTruthParticle(egamma);

  
  if ( !truth_egamma ) return;

  const auto Ereco = cluster->rawE();
  const auto Etruth = truth_egamma->e();
  const auto Eres = (Ereco - Etruth)/Etruth;

  profileMap["Eraw_Etruth_vs_Etruth_profile"]->Fill(truth_egamma->e()/1000,Ereco/Etruth);
  profileMap["Eraw_Etruth_vs_eta_profile"]->Fill(truth_egamma->eta(),Ereco/Etruth);
  histo2DMap["mu_energy_resolution_2D"]->Fill(mu,Eres);

  const CaloClusterCellLink* cellLinks = cluster->getCellLinks();
  
  if ( !cellLinks ) return;

  std::map<int, int > cells_per_layer;

  CaloClusterCellLink::const_iterator it_cell=cellLinks->begin();
  CaloClusterCellLink::const_iterator it_cell_e=cellLinks->end();
  for(;it_cell!=it_cell_e;++it_cell) {
    

     const CaloCell* cell = (*it_cell);

     if (cell) {
        int layer = cell->caloDDE()->getLayer();
        cells_per_layer[layer]++;
        
     }

  }  

  auto associatedTopoCluster = xAOD::EgammaHelpers::getAssociatedTopoClusters(cluster);
  profileMap["number_topocluster_vs_et_profile"]->Fill(truth_egamma->e()/1000, associatedTopoCluster.size());
  profileMap["number_topocluster_vs_eta_profile"]->Fill(truth_egamma->eta(), associatedTopoCluster.size());

  for (auto const& x : cells_per_layer) {
   profileMap["number_cell_in_layer"]->Fill(x.second,x.first);
   profileMap["number_cells_vs_et_in_layer_"+std::to_string(x.first)+"_profile"]->Fill(truth_egamma->e()/1000, x.second);
   profileMap["number_cells_vs_eta_in_layer_"+std::to_string(x.first)+"_profile"]->Fill(truth_egamma->eta(), x.second);

  }
  
}
