/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <string>

void getMuonEfficiency(string fname="MuonPhysMon.root",string muonColl="Staco",string dname="") 
{  
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0000);

  TFile* f=new TFile(fname.c_str(),"READ");
  
  //Get the run number - it's the top-folder's name in the root file!
  TIter dirItr(gDirectory->GetListOfKeys());
  TKey *key=0;
 
   
  while (key=(TKey*) dirItr()) {
    TDirectory *dir= (TDirectory*)key->ReadObj();
    string root=dir->GetName();

    if (dname!="" && root!=dname) {
      cout<<"Skipping directory: " << root << endl;
      continue;
    }
    else if ( root.find("run") < root.length() ) {
      root = root + "/";
      cout<<root<<endl;
    }
    else {
      root = "";
    }
    string pathBeforeAssociation = root+"Muon/MuonPhysicsMonitoring/Jpsimumu/Efficiencies/";
    string pathAfterAssociation = root+"Muon/MuonPhysicsMonitoring/Jpsimumu/Efficiencies/MuonReconstruction/"+muonColl+"MuonCollection/";

    TDirectory *dirBA= f->GetDirectory(pathBeforeAssociation.c_str());
    TDirectory *dirAA= f->GetDirectory(pathAfterAssociation.c_str());
    /////////////////////////////////////////////////////////////////
    
    //Mass
    TH1F *h_mass_before_OS = (TH1F*)dirBA->Get("m_TagAndProbe_Mass_BeforeProbeAssociation");
    TH1F *h_mass_before_SS =  (TH1F*)dirBA->Get("m_TagAndProbe_Mass_SameSign_BeforeProbeAssociation");
    TH1F *h_mass_before = h_mass_before_OS->Clone(); h_mass_before->Add(h_mass_before_OS,h_mass_before_SS,1,-1);
    TH1F *h_mass_after_OS =  (TH1F*)dirAA->Get("m_TagAndProbe_Mass_AfterProbeAssociation");
    TH1F *h_mass_after_SS =  (TH1F*)dirAA->Get("m_TagAndProbe_Mass_SameSign_AfterProbeAssociation");
    TH1F *h_mass_after = h_mass_after_OS->Clone(); h_mass_after->Add(h_mass_after_OS,h_mass_after_SS,1,-1);
    //Pt
    TH1F *h_pt_before_OS = (TH1F*)dirBA->Get("m_TagAndProbe_Pt_BeforeProbeAssociation_SR");
    TH1F *h_pt_before_SS = (TH1F*)dirBA->Get("m_TagAndProbe_Pt_SameSign_BeforeProbeAssociation_SR");
    TH1F *h_pt_before = h_pt_before_OS->Clone(); h_pt_before->Add(h_pt_before_OS,h_pt_before_SS,1,-1);
    TH1F *h_pt_after = (TH1F*)dirAA->Get("m_TagAndProbe_Pt_AfterProbeAssociation_SR");

    TH1F *h_pt_eff = h_pt_after->Clone();
    h_pt_eff->Divide(h_pt_after,h_pt_before);

    //Eta
    TH1F *h_eta_before_OS = (TH1F*)dirBA->Get("m_TagAndProbe_Eta_BeforeProbeAssociation_SR");
    TH1F *h_eta_before_SS = (TH1F*)dirBA->Get("m_TagAndProbe_Eta_SameSign_BeforeProbeAssociation_SR");
    TH1F *h_eta_before = h_eta_before_OS->Clone(); h_eta_before->Add(h_eta_before_OS,h_eta_before_SS,1,-1);
    TH1F *h_eta_after = (TH1F*)dirAA->Get("m_TagAndProbe_Eta_AfterProbeAssociation_SR");

    TH1F *h_eta_eff = h_eta_after->Clone();
    h_eta_eff->Divide(h_eta_after,h_eta_before);


    //Phi
    TH1F *h_phi_before_OS = (TH1F*)dirBA->Get("m_TagAndProbe_Phi_BeforeProbeAssociation_SR");
    TH1F *h_phi_before_SS = (TH1F*)dirBA->Get("m_TagAndProbe_Phi_SameSign_BeforeProbeAssociation_SR");
    TH1F *h_phi_before = h_phi_before_OS->Clone(); h_phi_before->Add(h_phi_before_OS,h_phi_before_SS,1,-1);
    TH1F *h_phi_after = (TH1F*)dirAA->Get("m_TagAndProbe_Phi_AfterProbeAssociation_SR");

    TH1F *h_phi_eff = h_phi_after->Clone();
    h_phi_eff->Divide(h_phi_after,h_phi_before);

  }
  
  TCanvas *c1 = new TCanvas();TLegend *leg = new TLegend(0.8,0.75,0.95,1); 
  h_mass_before_OS->SetTitle("Tag & probe before association");
  h_mass_before_OS->SetLineColor(kBlue);
  h_mass_before_OS->SetLineWidth(2);
  h_mass_before_OS->Draw();
  h_mass_before_OS->GetYaxis()->SetRangeUser(0,h_mass_before_OS->GetMaximum()*1.1);
  h_mass_before_SS->SetLineColor(kGreen);
  h_mass_before_SS->SetLineWidth(2);
  h_mass_before_SS->SetTitle("Tag & probe before association");
  h_mass_before_SS->Draw("same");
  leg->AddEntry(h_mass_before_OS,"OS","lp");
  leg->AddEntry(h_mass_before_SS,"SS","lp");
  leg->Draw();
  c1->Print("mass_mutrk.eps");

  TCanvas *c2 = new TCanvas();
  h_mass_after_OS->SetTitle("Tag & probe after association");
  h_mass_after_OS->SetLineColor(kBlue);
  h_mass_after_OS->SetLineWidth(2);
  h_mass_after_OS->Draw();
  h_mass_after_OS->GetYaxis()->SetRangeUser(0,h_mass_after_OS->GetMaximum()*1.1);
  h_mass_after_SS->SetLineColor(kGreen);
  h_mass_after_SS->SetLineWidth(2);
  h_mass_after_SS->SetTitle("Tag & probe after association");
  h_mass_after_SS->Draw("same");
  leg->Draw();
  c2->Print("mass_mumu.eps");

  //  h_mass_before->Draw(); 
  //  h_mass_after->SetLineColor(kRed);
  //  h_mass_after->Draw("same"); 

  TCanvas *c3 = new TCanvas();
  c3->Divide(3,1);
  c3->cd(1); h_pt_eff->GetYaxis()->SetRangeUser(0,1.1); h_pt_eff->Draw();
  c3->cd(2); h_eta_eff->GetYaxis()->SetRangeUser(0,1.1);h_eta_eff->Draw();
  c3->cd(3); h_phi_eff->GetYaxis()->SetRangeUser(0,1.1);h_phi_eff->Draw();

  TCanvas *c4 = new TCanvas();
  c4->Divide(3,1);
  c4->cd(1); h_pt_before->Draw();  h_pt_after->SetLineColor(kRed);  h_pt_after->Draw("same"); 
  c4->cd(2); h_eta_before->Draw();  h_eta_after->SetLineColor(kRed);  h_eta_after->Draw("same"); 
  c4->cd(3); h_phi_before->Draw();  h_phi_after->SetLineColor(kRed);  h_phi_after->Draw("same"); 

  TCanvas *c5 = new TCanvas();
  h_mass_before->GetYaxis()->SetRangeUser(0,h_mass_before_OS->GetMaximum()*1.1);
  h_mass_before->SetLineColor(kBlue);
  h_mass_before->SetTitle("");
  h_mass_before->Draw();
  h_mass_after->SetLineColor(kRed);
  h_mass_after->SetTitle("Tag and probe before-after association");
  h_mass_after->Draw("same");


  return;
}
