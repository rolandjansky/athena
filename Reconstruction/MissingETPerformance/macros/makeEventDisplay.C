/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void addBadFEB(TH2F *badFEBS,float eta_min,float eta_max,float phi_min,float phi_max) {

  float eta = eta_min;
  while (eta < eta_max) {

    float phi = phi_min;
    while (phi < phi_max) {

    badFEBS->Fill(eta+0.001,phi+0.001,0.1);
    phi += 0.1;
    }//phi
    eta += 0.1;
  }//eta
}


//void makeEventDisplay(TString run, TString event, TString met_w1,TString met_w2,TString met_w3, TString cand_num, float _met_phi) {
void makeEventDisplay(TString run, TString event, TString met_w1,TString met_w2,TString met_w3, TString cand_num, TString cand_num_forfile) {

  TFile *_file0 = TFile::Open("METPerformance.minsuk.root");
  //TFile *_file0 = TFile::Open("METPerformance.zee.root");
  gDirectory->cd("MissingETComposition");

  TString runevt_string = TString("eta_V_phi_run_") + TString(run) + TString ("event_") + TString(event);

  TString muon_name = TString("muon") + runevt_string;
  //TString muid_name = TString("muid") + runevt_string;
  TString refmuon_name = TString("refmuon") + runevt_string;
  TString jet_name = TString("jet") + runevt_string;
  TString ele_name = TString("ele") + runevt_string;
  TString clus_name = TString("clus") + runevt_string;
  //TString gamma_name = TString("gamma") + runevt_string;
  //TString tau_name = TString("tau") + runevt_string;

  TH2F *muon = (TH2F *)(gROOT->FindObject(muon_name));
  //TH2F *muid = (TH2F *)(gROOT->FindObject(muid_name));
  TH2F *refmuon = (TH2F *)(gROOT->FindObject(refmuon_name));
  TH2F *jet = (TH2F *)(gROOT->FindObject(jet_name));
  TH2F *ele = (TH2F *)(gROOT->FindObject(ele_name));
  TH2F *clus = (TH2F *)(gROOT->FindObject(clus_name));
  //TH2F *gamma= (TH2F *)(gROOT->FindObject(gamma_name));
  //TH2F *tau = (TH2F *)(gROOT->FindObject(tau_name));

  float min = 0;
  float max = 50;

  muon->SetFillColor(9); //9 = purple
  //muid->SetFillColor(kGreen);
  refmuon->SetFillColor(kBlack);//was 40
  jet->SetFillColor(kGreen);
  ele->SetFillColor(kBlue);
  clus->SetFillColor(kYellow);
  //gamma->SetFillColor(kOrange);
  //tau->SetFillColor(kRed);

  //draw bad FEBs
  TH2F *badFEBS = (TH2F *)(clus->Clone("badFEBS"));
  badFEBS->Reset();
  badFEBS->SetMinimum(min);
  badFEBS->SetMaximum(max);
  badFEBS->SetFillColor(6);//pink
  badFEBS->SetStats(0);
  badFEBS->GetXaxis()->SetTitle("eta");
  badFEBS->GetYaxis()->SetTitle("phi");
  badFEBS->GetZaxis()->SetTitle("pt (GeV)");
  badFEBS->GetXaxis()->SetTitleOffset(1.1);
  badFEBS->GetYaxis()->SetTitleOffset(1.3);
  badFEBS->GetZaxis()->SetTitleOffset(1.1);

  addBadFEB(badFEBS,-2.5,-2.4,-2.4,-1.6);
  addBadFEB(badFEBS,-2.0,-1.8,1.6,2.0);
  addBadFEB(badFEBS,-0.8,-0.4,2.6,2.8);
  addBadFEB(badFEBS,1.2,1.4,2.4,2.6);
  addBadFEB(badFEBS,1.4,1.5,0.4,0.6);
  addBadFEB(badFEBS,2.0,2.2,-1.0,-0.8);

  badFEBS->SetTitle( cand_num + TString("Run ")+run+TString(" Event ")+event);
  //badFEBS->SetTitle( TString("Run ")+run+TString(" Event ")+event);
  badFEBS->Draw("lego10 fb");

  clus->Draw("same lego10 fb");
  jet->Draw("same lego10 fb");
  muon->Draw("same lego10 fb");
  //muid->Draw("same lego10 fb");
  ele->Draw("same lego10 fb");
  refmuon->Draw("same lego10 fb");
  //gamma->Draw("same lego10 fb");
  //tau->Draw("same lego10 fb");

  muon->SetStats(0);
  //muid->SetStats(0);
  refmuon->SetStats(0);
  jet->SetStats(0);
  ele->SetStats(0);
  clus->SetStats(0);
  //gamma->SetStats(0);
  //tau->SetStats(0);


  muon->SetTitle(  TString("Run ")+run+TString(" Event ")+event);
  //muid->SetTitle(  TString("Run ")+run+TString(" Event ")+event);
  refmuon->SetTitle(  TString("Run ")+run+TString(" Event ")+event);
  jet->SetTitle(  TString("Run ")+run+TString(" Event ")+event);
  ele->SetTitle(  TString("Run ")+run+TString(" Event ")+event);
  clus->SetTitle(  TString("Run ")+run+TString(" Event ")+event);
  //gamma->SetTitle(  TString("Run ")+run+TString(" Event ")+event);
  //tau->SetTitle(  TString("Run ")+run+TString(" Event ")+event);

  muon->SetMinimum(min);
  //muid->SetMinimum(min);
  refmuon->SetMinimum(min);
  jet->SetMinimum(min);
  ele->SetMinimum(min);
  clus->SetMinimum(min);
  //gamma->SetMinimum(min);
  //tau->SetMinimum(min);

  muon->SetMaximum(max);
  //muid->SetMaximum(max);
  refmuon->SetMaximum(max);
  jet->SetMaximum(max);
  ele->SetMaximum(max);
  clus->SetMaximum(max);
  //gamma->SetMaximum(max);
  //tau->SetMaximum(max);

  muon->GetXaxis()->SetTitle("eta");
  //muid->GetXaxis()->SetTitle("eta");
  refmuon->GetXaxis()->SetTitle("eta");
  jet->GetXaxis()->SetTitle("eta");
  ele->GetXaxis()->SetTitle("eta");
  clus->GetXaxis()->SetTitle("eta");
  //gamma->GetXaxis()->SetTitle("eta");
  //tau->GetXaxis()->SetTitle("eta");

  muon->GetXaxis()->SetTitleOffset(1.1);
  //muid->GetXaxis()->SetTitleOffset(1.1);
  refmuon->GetXaxis()->SetTitleOffset(1.1);
  jet->GetXaxis()->SetTitleOffset(1.1);
  ele->GetXaxis()->SetTitleOffset(1.1);
  clus->GetXaxis()->SetTitleOffset(1.1);
  //gamma->GetXaxis()->SetTitleOffset(1.1);
  //tau->GetXaxis()->SetTitleOffset(1.1);

  muon->GetYaxis()->SetTitle("phi");
  //muid->GetYaxis()->SetTitle("phi");
  refmuon->GetYaxis()->SetTitle("phi");
  jet->GetYaxis()->SetTitle("phi");
  ele->GetYaxis()->SetTitle("phi");
  clus->GetYaxis()->SetTitle("phi");
  //gamma->GetYaxis()->SetTitle("phi");
  //tau->GetYaxis()->SetTitle("phi");

  muon->GetYaxis()->SetTitleOffset(1.3);
  //muid->GetYaxis()->SetTitleOffset(1.3);
  refmuon->GetYaxis()->SetTitleOffset(1.3);
  jet->GetYaxis()->SetTitleOffset(1.3);
  ele->GetYaxis()->SetTitleOffset(1.3);
  clus->GetYaxis()->SetTitleOffset(1.3);
  //gamma->GetYaxis()->SetTitleOffset(1.3);
  //tau->GetYaxis()->SetTitleOffset(1.3);


  muon->GetZaxis()->SetTitle("pt (GeV)");
  //muid->GetZaxis()->SetTitle("pt (GeV)");
  refmuon->GetZaxis()->SetTitle("pt (GeV)");
  jet->GetZaxis()->SetTitle("pt (GeV)");
  ele->GetZaxis()->SetTitle("pt (GeV)");
  clus->GetZaxis()->SetTitle("pt (GeV)");
  //gamma->GetZaxis()->SetTitle("pt (GeV)");
  //tau->GetZaxis()->SetTitle("pt (GeV)");

  muon->GetZaxis()->SetTitleOffset(1.1);
  //muid->GetZaxis()->SetTitleOffset(1.1);
  refmuon->GetZaxis()->SetTitleOffset(1.1);
  jet->GetZaxis()->SetTitleOffset(1.1);
  ele->GetZaxis()->SetTitleOffset(1.1);
  clus->GetZaxis()->SetTitleOffset(1.1);
  //gamma->GetZaxis()->SetTitleOffset(1.1);
  //tau->GetZaxis()->SetTitleOffset(1.1);

  TLegend *t = new TLegend(0.7,0.7,0.95,0.95);
  if (muon->GetEntries()>0) {
    t->AddEntry(muon, "MuonBoy");
  }
  if (refmuon->GetEntries()>0) {
    t->AddEntry(refmuon, "RefMuon");
  }
  if (jet->GetEntries()>0) {
    t->AddEntry(jet, "RefJet");
  }
  if (ele->GetEntries()>0) {
    t->AddEntry(ele, "RefEle");
  }
  if (clus->GetEntries()>0) {
    t->AddEntry(clus, "CellOut");
  }

  t->AddEntry(badFEBS, "Missing FEBs (S2)");

  //t->AddEntry(muid, "MuID");
  //t->AddEntry(gamma, "RefGamma");
  //t->AddEntry(tau, "RefTau");

  t->SetFillColor(0);
  t->Draw();

  c1->SetFillColor(0);

  TPaveText *pt = new TPaveText(-0.9660137,0.6347523,-0.5269166,0.9900241,"br");
  text = pt->AddText(TString("MET_W1 = ") + met_w1 + TString(" GeV"));
  text = pt->AddText(TString("MET_W2 = ") + met_w2 + TString(" GeV"));
  text = pt->AddText(TString("MET_W3 = ") + met_w3 + TString(" GeV"));
  pt->SetFillColor(0);
  pt->Draw();

  //c1->Print(TString("run_")+run+TString("_event_")+event+TString(".gif"));
  //c1->Print(TString("wenu_num_")+cand_num_forfile+TString("_run_")+run+TString("_event_")+event+TString(".gif"));
  c1->Print(TString("wmunu_num_")+cand_num_forfile+TString("_run_")+run+TString("_event_")+event+TString(".gif"));
}
