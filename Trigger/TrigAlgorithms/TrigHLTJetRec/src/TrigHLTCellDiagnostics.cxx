/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTCellDiagnostics.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
// AUTHOR:   Valentinos Christodoulou
//
// Description: Hypo for plotting cell histograms
//
// ********************************************************************

#include "TrigHLTJetRec/TrigHLTCellDiagnostics.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"


TrigHLTCellDiagnostics::TrigHLTCellDiagnostics(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {
  declareProperty("chainName", m_chainName = "unknown" );
}


TrigHLTCellDiagnostics::~TrigHLTCellDiagnostics() { }


HLT::ErrorCode TrigHLTCellDiagnostics::hltInitialize() {
  ATH_MSG_INFO("Initializing " << name() << "..."); 
  // Add histograms to map
  addHist(hMap1D,"nCells",   200,   0.0,  200000.0);	 
  addHist(hMap1D,"Eta",      100,  -5.0,  5.0);	
  addHist(hMap1D,"Phi",       64,  -3.2,  3.2);	
  addHist(hMap1D,"Rapidity", 100,  -5.0,  5.0);	
  addHist(hMap1D,"Energy",   100,  -200.0,   200.0);	 
  addHist(hMap1D,"Pt",       100,  -200.0,   200.0);	 
  addHist(hMap1D,"Px",       100,  -200.0,   200.0);	 
  addHist(hMap1D,"Py",       100,  -200.0,   200.0);	 
  addHist(hMap1D,"Pz",       100,  -200.0,   200.0);	
  addHist(hMap2D,"Eta_vs_Phi",    64,  -3.2,  3.2,  100,  -5.0,  5.0);
  addHist(hMap2D,"Energy_vs_Pt",  100,  -200.0,  500.0,  100,  -500.0,  200.0);	  
  return HLT::OK; 
}


HLT::ErrorCode TrigHLTCellDiagnostics::hltFinalize(){
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  // Open output root file
  std::string fileName = "cells_"+m_chainName+"_.root";
  TFile* fOut = new TFile(fileName.c_str(), "RECREATE");
  std::string directory = "cells_"+m_chainName;
  TDirectory* dir = fOut->mkdir(directory.c_str());
  dir->cd(); 
  // Save histograms and close file 
  for (auto hist : hMap1D) {
    hist.second->Write();
  }
  for (auto hist : hMap2D) {
    hist.second->Write();
  }
  fOut->Write();
  fOut->Close();
  // Clear histogram maps
  hMap2D.clear();
  hMap1D.clear();
  return HLT::OK;
}


// Add 1D histograms to map
void TrigHLTCellDiagnostics::addHist(std::map<TString,TH1D*> &hMap, TString tag, 
			         const int &bins, double min, double max) {
  TString label = tag;
  hMap[label] = new TH1D(label, "", bins, min, max);
}


// Add 2D histograms to map
void TrigHLTCellDiagnostics::addHist(std::map<TString,TH2D*> &hMap, TString tag, 
	const int &binsX, double minX, double maxX, const int &binsY, double minY, double maxY) {
  TString label = tag;
  hMap[label] = new TH2D(label, "", binsX, minX, maxX, binsY, minY, maxY);
}


HLT::ErrorCode TrigHLTCellDiagnostics::hltExecute(const HLT::TriggerElement* inputTE, bool& pass) {
  ATH_MSG_DEBUG("Executing " << name() << "...");

  pass = true;
  // Get CellContainer from input trigger element
  const CaloCellContainer* cellContainer = 0;
  HLT::ErrorCode hltStatus = getFeature(inputTE, cellContainer);  

  if (hltStatus == HLT::OK) {
    ATH_MSG_DEBUG("Retrieved CellContainer, address" << cellContainer);
  } else {
    ATH_MSG_ERROR("Failed to retrieve CellContainer.");
    return hltStatus;
  }

  if (cellContainer == 0){
    ATH_MSG_INFO("CellContainer address = 0, giving up");
    return HLT::OK;
  }

  ATH_MSG_DEBUG("No of cells in the container: " << cellContainer->size());
  hMap1D["nCells"]->Fill(cellContainer->size());

  for (const auto &cell : *cellContainer) {
    // Get cell attributes
    double energy = cell->e();
    double eta = cell->eta();
    double phi = cell->phi();
    double rapidity = cell->rapidity();
    double pt = cell->pt();
    double px = cell->px();
    double py = cell->py();
    double pz = cell->pz();

    // Fill histograms
    hMap1D["Energy"]->Fill(energy);
    hMap1D["Eta"]->Fill(eta);
    hMap1D["Phi"]->Fill(phi);
    hMap1D["Rapidity"]->Fill(rapidity);
    hMap1D["Pt"]->Fill(pt);
    hMap1D["Px"]->Fill(px);
    hMap1D["Py"]->Fill(py);
    hMap1D["Pz"]->Fill(pz);
    hMap2D["Eta_vs_Phi"]->Fill(phi, eta);
    hMap2D["Energy_vs_Pt"]->Fill(pt, energy);
  }

  // Set axis labels
  hMap1D["nCells"]->GetXaxis()->SetTitle("nCells");
  hMap1D["Energy"]->GetXaxis()->SetTitle("E [MeV]");
  hMap1D["Eta"]->GetXaxis()->SetTitle("Eta");
  hMap1D["Phi"]->GetXaxis()->SetTitle("Phi");
  hMap1D["Rapidity"]->GetXaxis()->SetTitle("y");
  hMap1D["Pt"]->GetXaxis()->SetTitle("Pt [MeV]");
  hMap1D["Px"]->GetXaxis()->SetTitle("Px [MeV]");
  hMap1D["Py"]->GetXaxis()->SetTitle("Py [MeV]");
  hMap1D["Pz"]->GetXaxis()->SetTitle("Pz [MeV]");
  hMap2D["Eta_vs_Phi"]->GetYaxis()->SetTitle("Eta");
  hMap2D["Eta_vs_Phi"]->GetXaxis()->SetTitle("Phi");
  hMap2D["Energy_vs_Pt"]->GetYaxis()->SetTitle("E [Mev]");
  hMap2D["Energy_vs_Pt"]->GetXaxis()->SetTitle("Pt [Mev]");

  return HLT::OK;
}
