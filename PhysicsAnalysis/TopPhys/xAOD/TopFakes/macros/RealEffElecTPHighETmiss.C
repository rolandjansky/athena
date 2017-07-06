/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGraph.h"
#include "TString.h"
#include "AtlasUtils.C"
#include "AtlasStyle.C"
#include "AtlasLabels.C"
#include <vector>
#include <string>
#include <cmath>

/////////////////////////////////////////////////////////////////////
// CREATED : 01/02/2016
// AUTHOR : Frederic Derue (LPNHE Paris)
// PURPOSE : ROOT macro to plot real efficiencies obtained both from 
//           tag-and-probe and high-MET/MTW methods vs different variables
//           It reads as input the default efficiencies of TopFakes package
// MODIFICATION : 
//
/////////////////////////////////////////////////////////////////////

void RealEffElecTPHighETmiss()
{
  // set the variable one want to compare
  //int var = 0;  // pt
  //int var = 1;  // eta
  //int var = 2;  // dR
  //int var = 3;  // jetpt
  //int var = 4;  // dphi
  //int var = 5;  // met
  //int var = 6;  // mtw
  //int var = 7;  // njet
  //int var = 8;  // nbjet

  SetAtlasStyle(); 
  gROOT->Reset();
  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetOptTitle(0);
  
  // define the class 
  MyEff my(1);
  // uncomment if you want to do all plots in one go
  for (int var=0;var<=8;var++)
    my.execute(var);
  
  return;
}

class MyEff {
  
public:
  
  MyEff(int choice) {}
  
  // ========================================
  void execute(int var) 
  {
    TGraph* plot0; 
    TH1F* plot[2][10];
    TString myname="";
    TString name[100];
    TString mynamehist = "";
    TFile *f[2];

    // read the input files
    // T&P
    myname = "../data/R_el_Default.root";    
    f[0] = TFile::Open(myname,"old");
    // High ETmiss
    myname = "../data/R_el_Default_CRreal.root";    
    f[1] = TFile::Open(myname,"old");

    // loop on files
    for (int ifile=0;ifile<=1;ifile++) {
      // loop on jet bins
      for (int ijet=0;ijet<=1;ijet++) {
	// check which variables to use
	if (var==0) mynamehist = "eff_pt";
	if (var==1) mynamehist = "eff_eta";
	if (var==2) mynamehist = "eff_dR";
	if (var==3) mynamehist = "eff_jetpt";
	if (var==4) mynamehist = "eff_dPhi";
	if (var==5) mynamehist = "eff_met";
	if (var==6) mynamehist = "eff_mtw";
	if (var==7) mynamehist = "eff_njet";
	if (var==8) mynamehist = "eff_nbtag";
	
	if (var!=7) {
	  if (ijet==0) mynamehist  += "_1j";
	  if (ifile==0)
	    if (ijet==1) mynamehist  += "_ge2j";
	  if (ifile==1)
	    if (ijet==1) mynamehist  += "";
	} else {
	  if (ijet==0) mynamehist  += "";
	  if (ijet==1) mynamehist  += "";
	}

	std::cout << " ijet = " << ijet 
		  << " namehist = " << mynamehist << std::endl;
	plot[ifile][ijet] = (TH1F*)f[ifile]->Get(mynamehist);
	std::cout << " ifile = " << ifile << " ijet = " << ijet 
		  << " plot = " << plot[ifile][ijet] << std::endl;
      }
    }

    // ===============================================
    // set the TAxis ranges
    m_nlines = 2;
    m_eff[0] = 0.75;
    m_eff[1] = 1.1;
    if (var==0) {m_var[0] = 25.;  m_var[1] = 150.;}  // pt
    if (var==1) {m_var[0] = 0.;   m_var[1] = 2.3;}   // eta
    if (var==2) {m_var[0] = 0.;   m_var[1] = 6.;}    // dR
    if (var==3) {m_var[0] = 25.;  m_var[1] = 500.;}  // jetpt
    if (var==4) {m_var[0] = 0.;   m_var[1] = 3.2;}   // dphi
    if (var==5) {m_var[0] = 0.;   m_var[1] = 1000.;} // met
    if (var==6) {m_var[0] = 0.;   m_var[1] = 1000.;} // mtw
    if (var==7) {m_var[0] = 0.;   m_var[1] = 6.;}    // njet
    if (var==8) {m_var[0] = 0.;   m_var[1] = 3.;}    // nbjet
    plot0 = new TGraph(m_nlines,m_var,m_eff);

    // plot the TGraph
    plotGraph(var,plot0,plot);
  }
  
  // ============================================== 
  void plotGraph(int var,TGraph* plot0,TH1F* plot[2][10])
  {
    // set the TCanvas
    c1 = new TCanvas("c1"," ",0.,0.,800,600);
    c1->SetFillColor(42);
    c1->SetFillColor(10);
    
    // set the TAxis
    plot0->SetFillColor(19);
    plot0->SetLineColor(0);
    plot0->SetLineWidth(4);
    plot0->SetMarkerColor(0);
    plot0->SetMarkerStyle(20);
    //plot0->GetXaxis()->CenterLabels(true);
    if (var==0) plot0->GetXaxis()->SetTitle("E_{T}(elec) [GeV]");
    if (var==1) plot0->GetXaxis()->SetTitle("|#eta(elec)|");
    if (var==2) plot0->GetXaxis()->SetTitle("#DeltaR (elec, nearest jet)");
    if (var==3) plot0->GetXaxis()->SetTitle("p_{T}(leading jet) [GeV]");
    if (var==4) plot0->GetXaxis()->SetTitle("#Delta#phi (elec,E_{T}^{miss})");
    if (var==5) plot0->GetXaxis()->SetTitle("E_{T}^{miss} [GeV]");
    if (var==6) plot0->GetXaxis()->SetTitle("m_{T}(elec,E_{T}^{miss}) [GeV]");
    if (var==7) plot0->GetXaxis()->SetTitle("n_{jet}");
    if (var==8) plot0->GetXaxis()->SetTitle("n_{b-jet}");

    plot0->GetYaxis()->SetTitle("Efficiency");
    plot0->Draw("AP");
    
    // ================================================================
    // plot the TGraph
    myplot(plot[0][0],1,20); 
    if (var!=7) {
      myplot(plot[0][1],2,21); 
    }
    myplot(plot[1][0],3,24); 

    c1->Update();
    c1->GetFrame()->SetFillColor(0);
    c1->GetFrame()->SetBorderSize(0);
    c1->Modified();
    // plot the TLegend
    plotLegend(var,plot);       
    c1->RedrawAxis();

    // set the output names
    TString suffix = "";
    if (var==0) suffix = "_vsEt";
    if (var==1) suffix = "_vsEta";
    if (var==2) suffix = "_vsdR";
    if (var==3) suffix = "_vsPtLeadingJet";
    if (var==4) suffix = "_vsdPhi";
    if (var==5) suffix = "_vsMET";
    if (var==6) suffix = "_vsMTW";
    if (var==7) suffix = "_vsnjet";
    if (var==8) suffix = "_vsnbjet";
    TString format = "";
    for (int i=0;i<3;i++) {
      if (i==0) format = ".eps";
      if (i==1) format = ".pdf";
      if (i==2) format = ".png";

      c1->SaveAs("fig/effrealTPHighETmiss_elec"+suffix+format);
    }
  }
  
  // ============================================== 
  void myplot(TH1F* plot,int mkcolor, int mktype, float size=1.5)
  {
    plot->SetFillColor(19);
    plot->SetLineColor(1);
    plot->SetLineWidth(1.5);
    plot->SetMarkerColor(mkcolor);
    plot->SetMarkerStyle(mktype);
    //plot->SetTitle("rel 16");
    plot->SetMarkerSize(size);
    plot->Draw("P sames");
  }

  // ============================================== 
  void plotLegend(int var,TH1F* plot[2][10])
  {
    float xlabel = 0.2; 
    float ylabel = 0.88;
    // set the ATLAS label
    ATLASLabel(xlabel,ylabel,"Work in Progress");
    // set energy and luminosity (adapt to your own case)
    myText(xlabel+0.1,ylabel-0.06,1,"e+jets, #sqrt{s}=13 TeV, 3.2 fb^{-1}",0.045);

    // ============================================================      
    // set the TLegend (adapt to your own team)
    float ymin = 0.6;
    float ymax = 0.8;
    leg = new TLegend(0.2,ymin,0.3,ymax);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.04);  
    leg->AddEntry(plot[0][0],"#epsilon_{r} tag-and-probe 1 jet","ep");
    leg->AddEntry(plot[0][1],"#epsilon_{r} tag-and-probe #geq2 jets","ep");
    leg->AddEntry(plot[1][0],"#epsilon_{r} #epsilon_{r} high E_{T}^{miss} #geq1 jet","ep");
    if (var!=7) leg->Draw();
  }

private:
  
  Float_t m_var[300];
  Float_t m_errvar[300];
  Float_t m_eff[300];
  Float_t m_erreff[300];
  
  Int_t m_nlines;
};
