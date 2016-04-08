#include "TPostScript.h"
#include "TH1F.h"
#include "TFile.h"

int testHist(){
  TH1F* h1 = new TH1F("h1", "TestHist", 7, 0.., 7.);
  h1->Fill(0, 0.);
  h1->Fill(1, 1.);
  h1->Fill(2, 2.);
  h1->Fill(3, 3.);
  h1->Fill(4, 2.);
  h1->Fill(5, 1.);
  h1->Fill(6, 0.);

  // h1->Draw();
  

  TFile* f1 = new TFile("testHist.root", "RECREATE");
  h1->Write();
  f1->Close();


  //TFile* ff1 = new TFile("refFile_testHist.root");
  //TH1F* g1 = (TH1F*)ff1->Get("h1");

  //g1->SetLineColor(kRed);

  TPostScript ps("RTTtest.ps",112);
  h1->Draw();
  //g1->Scale(0.9);
  //g1->Draw("SAME");
  ps.Close();
  

}
