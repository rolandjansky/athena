/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


void TriggerAnalysisTutorialJets() {

   TFile *f = TFile::Open("TriggerAnalysisTutorial.root");
   if(f==0) {
      std::cout << "File TriggerAnalysisTutorial.root not found" << std::endl;
      return;
   }

   TTree *tr = (TTree*)f->Get("trigtr");

   TCanvas *c = new TCanvas("c","Jet Trigger Reconstruction Studies",500,700);
   gStyle->SetPadBorderMode(0);

   c->Divide(2,3);

   c->cd(1);
   tr->Draw("Trig_l2Jet_et:Trig_l1Jet_et88","","box");

   c->cd(2);
   tr->Draw("(Trig_l2Jet_et-Trig_l1Jet_et88)/Trig_l2Jet_et");

   c->cd(3);
   tr->Draw("Trig_l2Jet_eta:Trig_l1Jet_eta","","box");

   c->cd(4);
   tr->Draw("Trig_l2Jet_eta-Trig_l1Jet_eta");

   c->cd(5);
   tr->Draw("Trig_l2Jet_phi:Trig_l1Jet_phi","","box");

   c->cd(6);
   tr->Draw("Trig_l2Jet_phi-Trig_l1Jet_phi");

}
