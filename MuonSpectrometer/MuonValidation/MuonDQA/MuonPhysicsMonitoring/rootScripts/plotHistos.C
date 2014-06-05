/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  TH1::AddDirectory(kFALSE);

  gROOT->ProcessLine("#include \"myFuncs.c\"");
  gROOT->Reset();
  
  gROOT->LoadMacro("AtlasStyle.C");
  SetAtlasStyle();

  TFile file1("result.C.pt1.MuonPhysMon-merged.root");

  string path = "Muon/MuonPhysicsMonitoring/Jpsimumu/Overview/NoTrigger/StacoMuonCollection/";

  TDirectory *dir = file1.GetDirectory(path.c_str());

  //Mass per region
  
  TCanvas* c1 = new TCanvas("c1","Jpsi Mass EEA region",0.,0.,800,600);
  TH1F *m_Jpsi_Mass_EEA = (TH1F*)dir->Get("m_Jpsi_Mass_EEA");
  int nJpsi; double mass;
  fitHistogram(m_Jpsi_Mass_EEA,nJpsi,mass,0.,0.);
  //m_Jpsi_Mass_EEA->GetXaxis()->SetTitle("#font[52]{mass} [MeV] ");
  //m_Jpsi_Mass_EEA->GetYaxis()->SetTitle("#font[52]{Entries/ } ");
  c1->Print("mass_EEA.eps");


  TCanvas* c2 = new TCanvas("c2","Jpsi Mass EEC region",0.,0.,800,600);
  TH1F *m_Jpsi_Mass_EEC = (TH1F*)dir->Get("m_Jpsi_Mass_EEC");
  int nJpsi; double mass;
  fitHistogram(m_Jpsi_Mass_EEC,nJpsi,mass,0.,0.);
  c2->Print("mass_EEC.eps";)


  TCanvas* c3 = new TCanvas("c3","Jpsi Mass BEA region",0.,0.,800,600);
  TH1F *m_Jpsi_Mass_BEA = (TH1F*)dir->Get("m_Jpsi_Mass_BEA");
  int nJpsi; double mass;
  fitHistogram(m_Jpsi_Mass_BEA,nJpsi,mass,0.,0.);
  c3->Print("mass_BEA.eps");


  TCanvas* c4 = new TCanvas("c4","Jpsi Mass BEC region",0.,0.,800,600);
  TH1F *m_Jpsi_Mass_BEC = (TH1F*)dir->Get("m_Jpsi_Mass_BEC");
  int nJpsi; double mass;
  fitHistogram(m_Jpsi_Mass_BEC,nJpsi,mass,0.,0.);
  c4->Print("mass_BEC.eps");


  TCanvas* c5 = new TCanvas("c5","Jpsi Mass BB region",0.,0.,800,600);
  TH1F *m_Jpsi_Mass_BB = (TH1F*)dir->Get("m_Jpsi_Mass_BB");
  int nJpsi; double mass;
  fitHistogram(m_Jpsi_Mass_BB,nJpsi,mass,0.,0.);
  c5->Print("mass_BB.eps");


  //Mass per muon type

  TCanvas* c6 = new TCanvas("c6","Jpsi Mass from CC muons",0.,0.,800,600);
  TH1F *m_Jpsi_Mass_CC = (TH1F*)dir->Get("m_Jpsi_Mass_CC");
  int nJpsi; double mass;
  fitHistogram(m_Jpsi_Mass_CC,nJpsi,mass,0.,0.);
  c6->Print("mass_CC.eps");


  TCanvas* c7 = new TCanvas("c7","Jpsi Mass from CT muons",0.,0.,800,600);
  TH1F *m_Jpsi_Mass_CT = (TH1F*)dir->Get("m_Jpsi_Mass_CT");
  int nJpsi; double mass;
  fitHistogram(m_Jpsi_Mass_CT,nJpsi,mass,0.,0.);
  c7->Print("mass_CT.eps");

  
  //Jpsi mass per trigger


  //number of Jpsis per trigger


  //file1.Close();
}


