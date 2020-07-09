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
  histo2DMap["number_cells_vs_et_in_layer_0_2D"] = (new TH2D(Form("%s_%s",m_name.c_str(),"number_cells_vs_et_in_layer_0_2D"), "Number of cells;E_{T}", 60,0,300,50,   0,100.));
  histo2DMap["number_cells_vs_et_in_layer_1_2D"] = (new TH2D(Form("%s_%s",m_name.c_str(),"number_cells_vs_et_in_layer_1_2D"), "Number of cells;E_{T}", 60,0,300,50,   0,100.));
  histo2DMap["number_cells_vs_et_in_layer_2_2D"] = (new TH2D(Form("%s_%s",m_name.c_str(),"number_cells_vs_et_in_layer_2_2D"), "Number of cells;E_{T}", 60,0,300,50,   0,100.));
  histo2DMap["number_cells_vs_et_in_layer_3_2D"] = (new TH2D(Form("%s_%s",m_name.c_str(),"number_cells_vs_et_in_layer_3_2D"), "Number of cells;E_{T}", 60,0,300,50,   0,100.));
  
  histo2DMap["number_cell_in_layer"] = (new TH2D(Form("%s_%s",m_name.c_str(),"number_cell_in_layer"), ";Number of cells;Layer",100,0,200, 4,0,4));  
  histo2DMap["mu_energy_resolution_2D"] = (new TH2D(Form("%s_%s",m_name.c_str(),"mu_energy_resolution_2D"), ";<#mu>; Energy Resolution", 5,0,80,20,-1,1));

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"Eraw_Etruth_vs_Etruth_2D", histo2DMap["Eraw_Etruth_vs_Etruth_2D"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"Eraw_Etruth_vs_eta_2D", histo2DMap["Eraw_Etruth_vs_eta_2D"]));

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_cells_vs_et_in_layer_0_2D", histo2DMap["number_cells_vs_et_in_layer_0_2D"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_cells_vs_et_in_layer_1_2D", histo2DMap["number_cells_vs_et_in_layer_1_2D"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_cells_vs_et_in_layer_2_2D", histo2DMap["number_cells_vs_et_in_layer_2_2D"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_cells_vs_et_in_layer_3_2D", histo2DMap["number_cells_vs_et_in_layer_3_2D"]));
 
 
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"number_cell_in_layer", histo2DMap["number_cell_in_layer"]));
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

  histo2DMap["Eraw_Etruth_vs_Etruth_2D"]->Fill(truth_egamma->e()/1000,Ereco/Etruth);
  histo2DMap["Eraw_Etruth_vs_eta_2D"]->Fill(truth_egamma->eta(),Ereco/Etruth);
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

  for (auto const& x : cells_per_layer) {
   histo2DMap["number_cell_in_layer"]->Fill(x.second,x.first);
   histo2DMap["number_cells_vs_et_in_layer_"+std::to_string(x.first)+"_2D"]->Fill(truth_egamma->e()/1000, x.second);

  }
  
}
