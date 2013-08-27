/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File: $Id: pulls.cc,v 1.5 2008-07-23 20:51:23 adye Exp $
#include <string>

#include "TROOT.h"
#include "TString.h"
#include "TTree.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TCut.h"

using std::string;

void pulls (TTree *n1, TTree *n2=0, TTree *n3=0, const string plotfile= "",
            const TCut data_cut="", double match=-1)
{
  TStyle plotStyle ("PlotStyle", "My Plot Style");
  plotStyle.cd();

  TCanvas *cpulls = new TCanvas("test","title",1);
  const int s_bins     = 50;
  const double s_pullMax = 6.0;
  TCut truth_cut ("track_truth_prob>"+TString::Format("%g",(match>=0?match:0.8)));
  //  TCut cuta("abs(1/track_qoverpt)>1000");
//  TCut cuta("track_truth_prob>0.8 && abs(1./track_truth_qoverpt)>3 && abs(track_truth_pdgid)==13");
  TCut cuta(data_cut && truth_cut && "abs(1/track_truth_qoverpt)>3");
  TCut cutb(data_cut);

  TH1F *pull1 = new TH1F("track_phi_pull",    "track phi pull",        s_bins,-s_pullMax, s_pullMax);
  TH1F *pull2 = new TH1F("track_qoverpt_pull","track q/pt pull",       s_bins,-s_pullMax, s_pullMax);
  TH1F *pull3 = new TH1F("track_theta_pull",  "track cot(theta) pull", s_bins,-s_pullMax, s_pullMax);
  TH1F *pull4 = new TH1F("track_d0_pull",     "track d0 pull",         s_bins,-s_pullMax, s_pullMax);
  TH1F *pull5 = new TH1F("track_z0_pull",     "track z0 pull",         s_bins,-s_pullMax, s_pullMax);

  TH1F *pull12 = new TH1F("track_phi_pull2",    "track phi pull",        s_bins,-s_pullMax, s_pullMax);
  TH1F *pull22 = new TH1F("track_qoverpt_pull2","track q/pt pull",       s_bins,-s_pullMax, s_pullMax);
  //  TH1F *pull32 = new TH1F("track_theta_pull2",  "track cot(theta) pull", s_bins,-s_pullMax, s_pullMax);
  TH1F *pull42 = new TH1F("track_d0_pull2",     "track d0 pull",         s_bins,-s_pullMax, s_pullMax);
  TH1F *pull52 = new TH1F("track_z0_pull2",     "track z0 pull",         s_bins,-s_pullMax, s_pullMax);

  TH1F *pull13 = new TH1F("track_phi_pull3",    "track phi pull",        s_bins,-s_pullMax, s_pullMax);
  TH1F *pull23 = new TH1F("track_qoverpt_pull3","track q/pt pull",       s_bins,-s_pullMax, s_pullMax);
  TH1F *pull33 = new TH1F("track_theta_pull3",  "track cot(theta) pull", s_bins,-s_pullMax, s_pullMax);
  TH1F *pull43 = new TH1F("track_d0_pull3",     "track d0 pull",         s_bins,-s_pullMax, s_pullMax);
  TH1F *pull53 = new TH1F("track_z0_pull3",     "track z0 pull",         s_bins,-s_pullMax, s_pullMax);

  if (n1) {
    n1->Project("track_phi_pull",    "(track_phi-track_truth_phi)/track_error_phi",cuta);
    n1->Project("track_qoverpt_pull","(track_qoverpt-track_truth_qoverpt)/track_error_qoverpt",cuta);
    n1->Project("track_theta_pull",  "(track_theta-track_truth_theta)/track_error_theta",cuta);
    n1->Project("track_d0_pull",     "(track_d0-track_truth_d0)/track_error_d0",cuta);
    n1->Project("track_z0_pull",     "(track_z0-track_truth_z0)/track_error_z0",cuta);
  }


  if (n2) { 
    n2->Project("track_phi_pull2",    "(track_phi-track_truth_phi)/track_error_phi",cutb);
    n2->Project("track_qoverpt_pull2","(track_qoverpt-track_truth_qoverpt)/track_error_qoverpt",cutb);
    n2->Project("track_theta_pull2",  "(track_theta-track_truth_theta)/track_error_theta",cutb);
    n2->Project("track_d0_pull2",     "(track_d0-track_truth_d0)/track_error_d0",cutb);
    n2->Project("track_z0_pull2",     "(track_z0-track_truth_z0)/track_error_z0",cutb);
  }

  if (n3) {
    n3->Project("track_phi_pull3",    "(track_phi-track_truth_phi)/track_error_phi",cutb);
    n3->Project("track_qoverpt_pull3","(track_qoverpt-track_truth_qoverpt)/track_error_qoverpt",cutb);
    n3->Project("track_theta_pull3",  "(track_theta-track_truth_theta)/track_error_theta",cutb);
    n3->Project("track_d0_pull3",     "(track_d0-track_truth_d0)/track_error_d0",cutb);
    n3->Project("track_z0_pull3",     "(track_z0-track_truth_z0)/track_error_z0",cutb);
  }

  // draw pulls
  cpulls->Divide(2,3);
 
  // pull12.SetLineType(2);
  
  plotStyle.SetOptFit();
  if (n1) {
    cpulls->cd(1); pull1->Draw();  pull1->Fit("gaus"); 
    cpulls->cd(2); pull2->Draw();    pull2->Fit("gaus");
    cpulls->cd(3); pull3->Draw();  pull3->Fit("gaus");
    cpulls->cd(4); pull4->Draw();  pull4->Fit("gaus");
    cpulls->cd(5); pull5->Draw();   pull5->Fit("gaus");
    cpulls->cd(6);
    if (plotfile != "") cpulls->Print(plotfile.c_str());
  }

  if (!n2 && !n3)
    {
     
    
  
    
    
    
    }
  if (n2) {
    cpulls->cd(1); pull12->Draw("same");
    cpulls->cd(2); pull22->Draw("same");
    cpulls->cd(3); pull33->Draw("same");
    cpulls->cd(4); pull42->Draw("same");
    cpulls->cd(5); pull52->Draw("same");
  }
  if (n3) {
    cpulls->cd(1); pull13->Draw("same");
    cpulls->cd(2); pull23->Draw("same");
    cpulls->cd(3); pull33->Draw("same");
    cpulls->cd(4); pull43->Draw("same");
    cpulls->cd(5); pull53->Draw("same");
  }


  gROOT->SetStyle("Default");
  delete cpulls;
}
