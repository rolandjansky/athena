/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int checkTTP(const char * filename, const char * psname="TTP.eps")
{
  TFile file(filename);
  TTree * tree = (TTree*)file.Get("eventTree");
  TCanvas * c1 = (TCanvas*)gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new  TCanvas("c1","",10,10,800,600);

  // decision
  TH1F * hh1 = TrigTauFlags::makeHist("hdec1","L1 decision");
  TH1F * hh2 = TrigTauFlags::makeHist("hdec2","L2 decision");
  TH1F * hh3 = TrigTauFlags::makeHist("hdec3","EF decision");
  hh2->SetLineColor(4);
  hh3->SetLineColor(2);
  
  tree->Draw("EventBranch.decision.fillHist(1,\"hdec1\")");
  tree->Draw("EventBranch.decision.fillHist(2,\"hdec2\")");
  tree->Draw("EventBranch.decision.fillHist(3,\"hdec3\")");


  c1->Clear();
  c1->Print(psname+TString("["));
  c1->Divide(2,2);

  c1->cd(1); drawMy(tree, "EventBranch.etMissL1.m_EtMiss","EventBranch.etMissL2.m_EtMiss","EventBranch.etMissEF.m_EtMiss","etmis");
  c1->cd(2); hh1->Draw(); hh2->Draw("same"); hh3->Draw("same");

  c1->cd(3); drawMy(tree, "EventBranch.tauL1.m_roiWord","EventBranch.tauL2.m_roiWord","EventBranch.tauEF.m_roiWord","roi");

  c1->cd(4);  tree->Draw("EventBranch.tauL1.m_TauEnergy");

  c1->Print(psname);
  
  c1->cd(1); tree->Draw("EventBranch.tauL2.Eta()");
  c1->cd(2); tree->Draw("EventBranch.tauL2.m_EMRadius");
  c1->cd(3); tree->Draw("EventBranch.tauL2.m_nCoreTracks");
  :wq
c1->cd(4); tree->Draw("EventBranch.tauL2.nTracks()");

  c1->Print(psname);
  
  c1->cd(1); tree->Draw("EventBranch.tauEF.Eta()");
  c1->cd(2); tree->Draw("EventBranch.tauEF.m_EMRadius");
  c1->cd(3); tree->Draw("EventBranch.tauEF.m_EMRadius_1p3p");
  c1->cd(4); tree->Draw("EventBranch.tauEF.nTracks()");
  c1->Print(psname);


  c1->Print(psname+TString("]"));
  
}

void drawMy(TTree * tree, const char * var1, const char * var2, const char * var3, TString hname)
{
  TString base1 = hname+TString("1");
  TString base2 = hname+TString("2");
  TString base3 = hname+TString("3");
  
  tree->Draw(var1+TString(">>"+base1));
  TH1 * h2 = (TH1*)gDirectory->Get(base1)->Clone(base2);
  TH1 * h3 = (TH1*)gDirectory->Get(base1)->Clone(base3);
  h2->SetLineColor(4);
  h3->SetLineColor(2);
  tree->Draw(var2+TString(">>"+base2),"","same");
  tree->Draw(var3+TString(">>"+base3),"","same");    
}
