/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJetDiagnostics.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
// AUTHOR:   Valentinos Christodoulou
//
// Description: Hypo for plotting jet histograms
//
// ********************************************************************

#include "TrigHLTJetRec/TrigHLTJetDiagnostics.h"
#include "xAODJet/JetContainer.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"


TrigHLTJetDiagnostics::TrigHLTJetDiagnostics(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {
  declareProperty("chainName", m_chainName = "unknown" );
}


TrigHLTJetDiagnostics::~TrigHLTJetDiagnostics() { }


HLT::ErrorCode TrigHLTJetDiagnostics::hltInitialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");  
  // Add histograms to map
  addHist(hMap1D,"nJets",      200,   0.0,  200.0);	  
  addHist(hMap1D,"Eta",        100,  -5.0,  5.0);	
  addHist(hMap1D,"Phi",         64,  -3.2,  3.2);	
  addHist(hMap1D,"Rapidity",   100,  -5.0,  5.0);	
  addHist(hMap1D,"Mass",       500,   0.0,  500.0);
  addHist(hMap1D,"Energy",     500,   0.0,  500.0);	
  addHist(hMap1D,"Et",         500,   0.0,  500.0);	
  addHist(hMap1D,"Pt",         500,   0.0,  500.0);	 
  addHist(hMap1D,"Px",         500,   0.0,  500.0);	 
  addHist(hMap1D,"Py",         500,   0.0,  500.0);	 
  addHist(hMap1D,"Pz",         500,   0.0,  500.0);	
  addHist(hMap1D,"LeadingEt",  100,   0.0,  500.0);	
  addHist(hMap1D,"LeadingPt",  100,   0.0,  500.0);	
  addHist(hMap2D,"Eta_vs_Phi",    64,  -3.2,  3.2,  100,  -5.0,  5.0);
  addHist(hMap2D,"Energy_vs_Pt",  100,  0.0,  100.0,  100,  0.0,  100.0);	
  return HLT::OK; 
}


HLT::ErrorCode TrigHLTJetDiagnostics::hltFinalize(){
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  // Open output root file
  std::string fileName = "jets_"+m_chainName+"_.root";
  TFile* fOut = new TFile(fileName.c_str(), "RECREATE");
  std::string directory = "jets_"+m_chainName;
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
void TrigHLTJetDiagnostics::addHist(std::map<TString,TH1D*> &hMap, TString tag, 
			         const int &bins, double min, double max) {
  TString label = tag;
  hMap[label] = new TH1D(label, "", bins, min, max);
}


// Add 2D histograms to map
void TrigHLTJetDiagnostics::addHist(std::map<TString,TH2D*> &hMap, TString tag, 
	const int &binsX, double minX, double maxX, const int &binsY, double minY, double maxY) {
  TString label = tag;
  hMap[label] = new TH2D(label, "", binsX, minX, maxX, binsY, minY, maxY);
}


HLT::ErrorCode TrigHLTJetDiagnostics::hltExecute(const HLT::TriggerElement* inputTE, bool& pass) {
  ATH_MSG_DEBUG("Executing " << name() << "...");
  pass = true;

  // Get JetContainer from input trigger element
  const xAOD::JetContainer* j_container = 0;
  HLT::ErrorCode hltStatus = getFeature(inputTE, j_container);  

  if (hltStatus == HLT::OK) {
    ATH_MSG_DEBUG("Retrieved JetContainer, address" << j_container);
  } else {
    ATH_MSG_ERROR("Failed to retrieve JetContainer.");
    return hltStatus;
  }

  if (j_container == 0){
    ATH_MSG_INFO("JetContainer address = 0, giving up");
    return HLT::OK;
  }

  ATH_MSG_DEBUG("No of jets in the container: " << j_container->size());
  hMap1D["nJets"]->Fill(j_container->size());

  double leadingPt = 0;
  double leadingEt = 0;
  for (const auto &jet : *j_container) {
    // Get jet attributes
    double energy = jet->e();
    double et = (jet->p4()).Et();
    double mass = jet->m();
    double eta = jet->eta();
    double phi = jet->phi();
    double rapidity = jet->rapidity();
    double pt = jet->pt();
    double px = jet->px();
    double py = jet->py();
    double pz = jet->pz();

    if (pt > leadingPt) leadingPt = pt;
    if (et > leadingEt) leadingEt = et;

    // Fill histograms
    hMap1D["Energy"]->Fill(energy*0.001);
    hMap1D["Et"]->Fill(et*0.001);
    hMap1D["Mass"]->Fill(mass*0.001);
    hMap1D["Eta"]->Fill(eta);
    hMap1D["Phi"]->Fill(phi);
    hMap1D["Rapidity"]->Fill(rapidity);
    hMap1D["Pt"]->Fill(pt*0.001);
    hMap1D["Px"]->Fill(px*0.001);
    hMap1D["Py"]->Fill(py*0.001);
    hMap1D["Pz"]->Fill(pz*0.001);
    hMap2D["Eta_vs_Phi"]->Fill(phi, eta);
    hMap2D["Energy_vs_Pt"]->Fill(pt*0.001, energy*0.001);
  }

  hMap1D["LeadingEt"]->Fill(leadingEt*0.001);
  hMap1D["LeadingPt"]->Fill(leadingPt*0.001);

  // Set axis labels
  hMap1D["nJets"]->GetXaxis()->SetTitle("nJets");
  hMap1D["Energy"]->GetXaxis()->SetTitle("E [GeV]");
  hMap1D["Et"]->GetXaxis()->SetTitle("Et [GeV]");
  hMap1D["Mass"]->GetXaxis()->SetTitle("M [GeV]");
  hMap1D["Eta"]->GetXaxis()->SetTitle("Eta");
  hMap1D["Phi"]->GetXaxis()->SetTitle("Phi");
  hMap1D["Rapidity"]->GetXaxis()->SetTitle("y");
  hMap1D["Pt"]->GetXaxis()->SetTitle("Pt [GeV]");
  hMap1D["Px"]->GetXaxis()->SetTitle("Px [GeV]");
  hMap1D["Py"]->GetXaxis()->SetTitle("Py [GeV]");
  hMap1D["Pz"]->GetXaxis()->SetTitle("Pz [GeV]");
  hMap1D["LeadingEt"]->GetXaxis()->SetTitle("Et [GeV]");
  hMap1D["LeadingPt"]->GetXaxis()->SetTitle("Pt [GeV]");
  hMap2D["Eta_vs_Phi"]->GetYaxis()->SetTitle("Eta");
  hMap2D["Eta_vs_Phi"]->GetXaxis()->SetTitle("Phi");
  hMap2D["Energy_vs_Pt"]->GetYaxis()->SetTitle("E [Gev]");
  hMap2D["Energy_vs_Pt"]->GetXaxis()->SetTitle("Pt [Gev]");

  return HLT::OK;
}
