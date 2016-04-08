#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"


void TrigMuonValidation_drawEfficiencies(TString inputFile, TString saveName){

  TCanvas c1;

  TFile *f1 = TFile::Open(inputFile);
  TH1 *h1 = (TH1*)f1->Get("Muons/All/matched/AllMuons/Muons_All_matched_AllMuons_Eff_eta");
  TH1 *h2 = (TH1*)f1->Get("Muons/All/matched/AllMuons/Muons_All_matched_AllMuons_Eff_phi");
  TH1 *h3 = (TH1*)f1->Get("Muons/All/matched/AllMuons/Muons_All_matched_AllMuons_Eff_pt");

  TH1 *htruth = (TH1*)f1->Get("Muons/All/truth/all/Muons_All_truth_all_pt");
  TH1 *hmatched = (TH1*)f1->Get("Muons/All/matched/AllMuons/Muons_All_matched_AllMuons_nphiLayers");

  double Ntruth = (double)htruth->Integral();
  double Nmatch = (double)hmatched->Integral();
  double eff = 0.0;
  if(Ntruth>0) eff = Nmatch/Ntruth;
  cout<<"Efficiency: "<<eff<<"\n";

  TString name = saveName+"eta.eps";
  if( h1!=NULL ) h1->Draw();
  c1.SaveAs(name);

  name = saveName+"phi.eps";
  if( h2!=NULL ) h2->Draw();
  c1.SaveAs(name);

  name = saveName+"pt.eps";
  if( h3!=NULL ) h3->Draw();
  c1.SaveAs(name);

}
