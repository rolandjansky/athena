#define FastSimulationHistMaker_cxx

#include <iostream>

#include "TLorentzVector.h"
#include "TMath.h"
#include "TCanvas.h"

#include "FastSimulationHistMaker.h"


void FastSimulationHistMaker::Loop(const TString outputFileName, const Int_t maxEvents)
{
   if (fChain == 0) return;

   fOutputFileName = outputFileName;
   fMaxEvents = maxEvents;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;

   m_verbose = kFALSE;

   initialize();

   if(fMaxEvents!=-1){
     if(nentries>fMaxEvents){
       nentries = fMaxEvents;
     }
   }

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      initEachEvent();
      execute();
   }
   finalize();
}


void FastSimulationHistMaker::initialize() {
  std::cout << "beginning of initialize()" << std::endl;

  h_el_pt_el_truth_pt_deltaRatio = new TH1F("h_el_pt_el_truth_pt_deltaRatio", "h_el_pt_el_truth_pt_deltaRatio", 200, -2, 2);
  h_el_pt_el_truth_pt_deltaRatio->GetXaxis()->SetTitle("(p_{T}^{reco electron} - p_{T}^{truth electron}) / p_{T}^{truth electron}");
  h_el_pt_el_truth_pt_deltaRatio->GetYaxis()->SetTitle("Events");

  h_ph_E_ph_truth_E_deltaRatio = new TH1F("h_ph_E_ph_truth_E_deltaRatio", "h_ph_E_ph_truth_E_deltaRatio", 200, -2, 2);
  h_ph_E_ph_truth_E_deltaRatio->GetXaxis()->SetTitle("(E^{reco photon} - E^{truth photon}) / E^{truth photon}");
  h_ph_E_ph_truth_E_deltaRatio->GetYaxis()->SetTitle("Events");

  h_jet_AntiKt4TopoEM_pt_jet_AntiKt4Truth_pt_deltaRatio = new TH1F("h_jet_AntiKt4TopoEM_pt_jet_AntiKt4Truth_pt_deltaRatio", "h_jet_AntiKt4TopoEM_pt_jet_AntiKt4Truth_pt_deltaRatio", 200, -2, 2);
  h_jet_AntiKt4TopoEM_pt_jet_AntiKt4Truth_pt_deltaRatio->GetXaxis()->SetTitle("(p_{T}^{reco jet} - p_{T}^{truth jet}) / p_{T}^{truth jet}");
  h_jet_AntiKt4TopoEM_pt_jet_AntiKt4Truth_pt_deltaRatio->GetYaxis()->SetTitle("Events");
  
  h_MET_LocHadTopo_et = new TH1F("h_MET_LocHadTopo_et", "h_MET_LocHadTopo_et", 200, 0, 200);
  h_MET_LocHadTopo_et->GetXaxis()->SetTitle("E_{T}^{miss} [GeV]");
  h_MET_LocHadTopo_et->GetYaxis()->SetTitle("Events");

  if(m_verbose) std::cout << "end of initilize()" << std::endl;
}

void FastSimulationHistMaker::initEachEvent(){
}

void FastSimulationHistMaker::execute() {
  if(m_verbose) std::cout << "beggning of execute()" << std::endl;
  
  if(m_verbose) std::cout << "  photon" << std::endl;

  for(Int_t i=0; i<ph_n; i++) {
    if(ph_truth_E->at(i)>0.0 && ph_truth_matched->at(i)){
      h_ph_E_ph_truth_E_deltaRatio->Fill( (ph_E->at(i)-ph_truth_E->at(i)) / ph_truth_E->at(i) );
    }
  }

  if(m_verbose) std::cout << "  electron" << std::endl;

  for(Int_t i=0; i<el_n; i++){
    if(el_truth_pt->at(i)>0.0 && el_truth_matched->at(i)){
      h_el_pt_el_truth_pt_deltaRatio->Fill( (el_pt->at(i) - el_truth_pt->at(i)) / el_truth_pt->at(i));
    }
  }
    
  if(m_verbose) std::cout << "  jet" << std::endl;
  for(Int_t i=0; i<jet_AntiKt4TopoEM_n; i++){
    TLorentzVector jet;
    jet.SetPtEtaPhiE(jet_AntiKt4TopoEM_pt->at(i), jet_AntiKt4TopoEM_eta->at(i), jet_AntiKt4TopoEM_phi->at(i), jet_AntiKt4TopoEM_E->at(i));

    Double_t deltaR = TMath::Pi();

    for(Int_t j=0; j<jet_AntiKt4Truth_n; j++){
      TLorentzVector jet_truth;
      jet_truth.SetPtEtaPhiE(jet_AntiKt4Truth_pt->at(j), jet_AntiKt4Truth_eta->at(j), jet_AntiKt4Truth_phi->at(j), jet_AntiKt4Truth_E->at(j));
      
      deltaR = jet.DeltaR(jet_truth);
      if(deltaR<0.4){
	if(jet_truth.Pt()>0.0){
	  h_jet_AntiKt4TopoEM_pt_jet_AntiKt4Truth_pt_deltaRatio->Fill( (jet.Pt()-jet_truth.Pt())/jet_truth.Pt());
	}
	break;
      }
    }
  }

  if(m_verbose) std::cout << "  MET_LocHadTopo" << std::endl;

  h_MET_LocHadTopo_et->Fill( MET_LocHadTopo_et / 1000. );

  if(m_verbose) std::cout << "end of execte()" << std::endl;
}



void FastSimulationHistMaker::finalize() {
  if(m_verbose) std::cout << "beggning of finalize()" << std::endl;

  fOutputFile = new TFile(fOutputFileName.Data(), "RECREATE");
  fOutputFile->cd();

  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  h_ph_E_ph_truth_E_deltaRatio->Write();
  h_ph_E_ph_truth_E_deltaRatio->Draw();
  c1->SaveAs("ph_E_ph_truth_E_deltaRatio.png");

  h_el_pt_el_truth_pt_deltaRatio->Write();
  h_el_pt_el_truth_pt_deltaRatio->Draw();
  c1->SaveAs("el_pt_el_truth_pt_deltaRatio.png");

  h_jet_AntiKt4TopoEM_pt_jet_AntiKt4Truth_pt_deltaRatio->Write();
  h_jet_AntiKt4TopoEM_pt_jet_AntiKt4Truth_pt_deltaRatio->Draw();
  c1->SaveAs("jet_AntiKt4TopoEM_pt_jet_AntiKt4Truth_pt_deltaRatio.png");
  
  h_MET_LocHadTopo_et->Write();
  h_MET_LocHadTopo_et->Draw();
  c1->SaveAs("MET_LocHadTopo_et.png");

  fOutputFile->Close();

  if(m_verbose) std::cout << "end of finalize()" << std::endl;
}
  
