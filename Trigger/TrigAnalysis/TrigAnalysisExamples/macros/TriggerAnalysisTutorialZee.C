/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


void TriggerAnalysisTutorialZee() {

   TFile *f = TFile::Open("TriggerAnalysisTutorial.root");
   if(f==0) {
      std::cout << "File TriggerAnalysisTutorial.root not found" << std::endl;
      return;
   }

   TTree *tr = (TTree*)f->Get("trigtr");

   TCanvas *c = new TCanvas("c","Electron Trigger Studies using Tag and Probe with Z-> ee",800,600);
   gStyle->SetPadBorderMode(0);

   c->Divide(3,3);

   c->cd(1);
   tr->Draw("Zmass>>Zmass(100,0,130)");

   c->cd(2);
   tr->Draw("e2P>>e2P(100,0,130000)"); e2P->SetLineColor(4);
   tr->Draw("e1P>>e1P(100,0,130000)","","same");

   c->cd(3);
   tr->Draw("e1P:e2P","","box");

   c->cd(4);
   tr->Draw("deltaR>>deltaR(100,0,0.005)");

   c->cd(5);
   tr->Draw("probeMatch>>probeMatch"); probeMatch->SetMinimum(0); probeMatch->SetTitle("Probe electron w/ matched trigger");

   c->cd(6);
   tr->Draw("probeP>>probeP(100,0,130000)"); probeP->SetTitle("Momentum of probe electron");
   TH1* probeP = (TH1*)gDirectory->Get("probeP");
   tr->Draw("probeP>>probePwM(100,0,130000)","probeMatch==1","same"); probePwM->SetLineColor(2);


   c->cd(7);
   tr->Draw("probeP>>probePwME(100,0,130000)","probeMatch==1");
   probePwME->Sumw2();
   probePwME->SetTitle("Electron trigger efficiency vs momentum");
   probePwME->Divide(probeP);
   
}
