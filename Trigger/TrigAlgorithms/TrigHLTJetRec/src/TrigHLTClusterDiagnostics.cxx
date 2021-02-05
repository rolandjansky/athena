/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTClusterDiagnostics.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
// AUTHOR:   Valentinos Christodoulou
//
// Description: Hypo for plotting cluster histograms
//
// ********************************************************************

#include "TrigHLTJetRec/TrigHLTClusterDiagnostics.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"


TrigHLTClusterDiagnostics::TrigHLTClusterDiagnostics(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {
  declareProperty("chainName", m_chainName = "unknown" );
}


TrigHLTClusterDiagnostics::~TrigHLTClusterDiagnostics() { }


HLT::ErrorCode TrigHLTClusterDiagnostics::hltInitialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");
  // Add histograms to map
  addHist(m_hMap1D,"nClusters",  200,   0.0,  2000.0);	 
  addHist(m_hMap1D,"Eta",        100,  -5.0,  5.0);	
  addHist(m_hMap1D,"Phi",         64,  -3.2,  3.2);	
  addHist(m_hMap1D,"Rapidity",   100,  -5.0,  5.0);	
  addHist(m_hMap1D,"Energy",     200,  -10000.0,  20000.0);	
  addHist(m_hMap1D,"Et",         200,  -10000.0,  20000.0); 
  addHist(m_hMap1D,"Pt",         200,  -10000.0,  20000.0);	 
  addHist(m_hMap1D,"Px",         200,  -10000.0,  20000.0);	 
  addHist(m_hMap1D,"Py",         200,  -10000.0,  20000.0);	 
  addHist(m_hMap1D,"Pz",         200,  -10000.0,  20000.0);	
  addHist(m_hMap2D,"Eta_vs_Phi",    64,  -3.2,  3.2,  100,  -5.0,  5.0);
  addHist(m_hMap2D,"Energy_vs_Pt",  200,  -10000.0,  20000.0,  200,  -10000.0,  20000.0);
  return HLT::OK; 
}


HLT::ErrorCode TrigHLTClusterDiagnostics::hltFinalize(){
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  // Open output root file
  std::string fileName = "clusters_"+m_chainName+"_.root";
  TFile* fOut = new TFile(fileName.c_str(), "RECREATE");
  std::string directory = "clusters_"+m_chainName;
  TDirectory* dir = fOut->mkdir(directory.c_str());
  dir->cd();  
  // Save histograms and close file 
  for (auto hist : m_hMap1D) {
    hist.second->Write();
  }
  for (auto hist : m_hMap2D) {
    hist.second->Write();
  }
  fOut->Write();
  fOut->Close();
  // Clear histogram maps
  m_hMap2D.clear();
  m_hMap1D.clear();
  return HLT::OK;
}


// Add 1D histograms to map
void TrigHLTClusterDiagnostics::addHist(std::map<TString,TH1D*> &hMap, TString tag, 
			         const int &bins, double min, double max) {
  TString label = tag;
  hMap[label] = new TH1D(label, "", bins, min, max);
}


// Add 2D histograms to map
void TrigHLTClusterDiagnostics::addHist(std::map<TString,TH2D*> &hMap, TString tag, 
	const int &binsX, double minX, double maxX, const int &binsY, double minY, double maxY) {
  TString label = tag;
  hMap[label] = new TH2D(label, "", binsX, minX, maxX, binsY, minY, maxY);
}


HLT::ErrorCode TrigHLTClusterDiagnostics::hltExecute(const HLT::TriggerElement* inputTE, bool& pass) {
  ATH_MSG_DEBUG("Executing " << name() << "...");
  pass = true;

  // Get CaloClusterContainer from input trigger element
  const xAOD::CaloClusterContainer* clusterContainer = 0;
  HLT::ErrorCode hltStatus = getFeature(inputTE, clusterContainer);  

  if (hltStatus == HLT::OK) {
    ATH_MSG_DEBUG("Retrieved CaloClusterContainer, address" << clusterContainer);
  } else {
    ATH_MSG_ERROR("Failed to retrieve CaloClusterContainer.");
    return hltStatus;
  }

  if (clusterContainer == 0){
    ATH_MSG_INFO("CaloClusterContainer address = 0, giving up");
    return HLT::OK;
  }

  ATH_MSG_DEBUG("No of clusters in the container: " << clusterContainer->size());
  m_hMap1D["nClusters"]->Fill(clusterContainer->size());

  for (const auto cluster : *clusterContainer) {
    // Get cluster attributes
    double energy = (cluster->p4()).E();
    double et = (cluster->p4()).Et();
    double eta = (cluster->p4()).Eta();
    double phi = (cluster->p4()).Phi();
    double rapidity = (cluster->p4()).Rapidity();
    double pt = (cluster->p4()).Pt();
    double px = (cluster->p4()).Px();
    double py = (cluster->p4()).Py();
    double pz = (cluster->p4()).Pz();

    // Fill histograms
    m_hMap1D["Energy"]->Fill(energy);
    m_hMap1D["Et"]->Fill(et);
    m_hMap1D["Eta"]->Fill(eta);
    m_hMap1D["Phi"]->Fill(phi);
    m_hMap1D["Rapidity"]->Fill(rapidity);
    m_hMap1D["Pt"]->Fill(pt);
    m_hMap1D["Px"]->Fill(px);
    m_hMap1D["Py"]->Fill(py);
    m_hMap1D["Pz"]->Fill(pz);
    m_hMap2D["Eta_vs_Phi"]->Fill(phi, eta);
    m_hMap2D["Energy_vs_Pt"]->Fill(pt, energy);
  }

  // Set axis labels
  m_hMap1D["nClusters"]->GetXaxis()->SetTitle("nClusters");
  m_hMap1D["Energy"]->GetXaxis()->SetTitle("E [MeV]");
  m_hMap1D["Et"]->GetXaxis()->SetTitle("Et [MeV]");
  m_hMap1D["Eta"]->GetXaxis()->SetTitle("Eta");
  m_hMap1D["Phi"]->GetXaxis()->SetTitle("Phi");
  m_hMap1D["Rapidity"]->GetXaxis()->SetTitle("y");
  m_hMap1D["Pt"]->GetXaxis()->SetTitle("Pt [MeV]");
  m_hMap1D["Px"]->GetXaxis()->SetTitle("Px [MeV]");
  m_hMap1D["Py"]->GetXaxis()->SetTitle("Py [MeV]");
  m_hMap1D["Pz"]->GetXaxis()->SetTitle("Pz [MeV]");
  m_hMap2D["Eta_vs_Phi"]->GetYaxis()->SetTitle("Eta");
  m_hMap2D["Eta_vs_Phi"]->GetXaxis()->SetTitle("Phi");
  m_hMap2D["Energy_vs_Pt"]->GetYaxis()->SetTitle("E [Mev]");
  m_hMap2D["Energy_vs_Pt"]->GetXaxis()->SetTitle("Pt [Mev]");

  return HLT::OK;
}
