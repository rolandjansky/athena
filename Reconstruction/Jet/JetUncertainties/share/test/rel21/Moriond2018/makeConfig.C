void makeConfig(){
  TFile* f1=new TFile("R10_AllComponents.root");
  TH3* validHist=(TH3D*)f1->Get("ValidRange_AntiKt10LCTopoTrimmedPtFrac5SmallR20");
  validHist->SetName("ValidRange_AntiKt10LCTopoTrimmedPtFrac5SmallR20");
  
  TFile* f2=new TFile("temp.root");
  TH2* hist=(TH2D*)f2->Get("weightHistogram_q_BkgModelling");
  hist->SetName("Syst1_single_q_SF_AntiKt10LCTopoTrimmedPtFrac5SmallR20");

  TFile* fout=new TFile("testSF.root", "RECREATE");
  gDirectory->Append(validHist);
  gDirectory->Append(hist);
  fout->Write();
}
//Rtrk_Baseline_pT_AntiKt10LCTopoTrimmedPtFrac5SmallR20
