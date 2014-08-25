/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  #include "utils.h"
  // Open the root file
  TFile file("METPerformance.zmumu.root");

  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();
  gStyle->SetOptStat(111111);
  gStyle->SetOptFit(0);

  int m_fitrange=2;
  float m_ns=2.;
  float m_percerr=0.15;
  int m_entries=100;
  int m_numptt=100;
  int m_binpt=3.5;
  float m_startpt=0.;
  float m_curvaslin=10.;
  int numMETCalculations = 7;

  string dirname[numMETCalculations];

  dirname[0] = "ZMuMu/Resolutions/RefFinal/Data_Res_Perp_HadActivity";
  dirname[1] = "ZMuMu/Resolutions/EmScale/Data_Res_Perp_HadActivity";
  dirname[2] = "ZMuMu/Resolutions/GlobCalib/Data_Res_Perp_HadActivity";
  dirname[3] = "ZMuMu/Resolutions/Final/Data_Res_Perp_HadActivity";
  dirname[4] = "ZMuMu/Resolutions/FinalLocHad/Data_Res_Perp_HadActivity";
  dirname[5] = "ZMuMu/Resolutions/BaseEm/Data_Res_Perp_HadActivity";
  dirname[6] = "ZMuMu/Resolutions/BaseGlobCalib/Data_Res_Perp_HadActivity";

  char name[m_numptt];
  double sigma[m_numptt];
  double sigma_err[m_numptt];
  float expsigmaf[m_numptt];
  float xbin;

  double mean[numMETCalculations][m_numptt];
  double mean_err[numMETCalculations][m_numptt];
  double ex[m_numptt];
  double nxbin[numMETCalculations][m_numptt];
  int nbinplot[numMETCalculations]={0};

  int cp=sqrt((float)m_numptt)+1;
  TCanvas *c1[numMETCalculations];
  for (int i =0; i < numMETCalculations; ++i) {
    TString temp;
    if (i==0) temp = "c1_0";
    if (i==1) temp = "c1_1";
    if (i==2) temp = "c1_2";
    if (i==3) temp = "c1_3";
    if (i==4) temp = "c1_4";
    if (i==5) temp = "c1_5";
    if (i==6) temp = "c1_6";
    if (i==7) temp = "c1_7";
    if (i==8) temp = "c1_8";
    if (i==9) temp = "c1_9";

    c1[i] = new TCanvas(temp,temp,293,139,700,500);
    c1[i]->Update();
    c1[i]->Divide(cp,cp);
  }

  int ih=0;
  //loop over MET calculations
  for (int icalc = 0; icalc < numMETCalculations; ++icalc) {

    file->cd( dirname[icalc].c_str() );

    //reset ih
    ih = 0;

    for (int i= 0; i< m_numptt ; i++){
      c1[icalc]->cd(i+1);
      sprintf(name,"ResolutionInBin_%d",i);
      TH1F* hist  =(TH1F*)gDirectory->Get(name);

      int entries = hist->GetEntries();
      sigma[ih]=-10005.;
      sigma_err[ih]=1000.;
      xbin = m_startpt+m_binpt/2.+ i * m_binpt;
      nxbin[icalc][ih]=xbin;
      ex[ih]=0.;

      if  ( entries> m_entries) {
	//fit on the whole plot range............................
	if (m_fitrange == 2) {
	  hist->Fit( "gaus","","");
	}
	//fit in m_ns sigma (sigma=0.5*sqrt(SumET bin)..........
	if (m_fitrange == 3) {
	  float expsigma=0.5*sqrt(xbin);
	  hist->Fit( "gaus","","",-m_ns*expsigma,m_ns*expsigma );
	}
	TF1 *fitsf = hist->GetFunction("gaus");

	nbinplot[icalc]++;
	mean[icalc][ih] = fitsf->GetParameter(1);
	sigma[ih] = fitsf->GetParameter(2);
	mean_err[icalc][ih] = fitsf->GetParError(1);
	sigma_err[ih] = fitsf->GetParError(2);
	expsigmaf[ih] =m_ns*expsigma;
  
	//choice of points.....................
	if(sigma[ih] > 0.) {
	  double rappo= sigma_err[ih]/sigma[ih];
	  if ( rappo > m_percerr ) {
	    sigma[ih]=-10005.;
	    sigma_err[ih]=1000.;
	  }
	}
	ih++;
      }//require min number of entries
    }
  }//loop over calculations

  //c1.Print("ZMuMu_many_plot_xy.ps");
  //c12.Print("ZMuMu_many_plot_xy2.ps");

  TCanvas* c2 = new TCanvas("c2"," Linearity along Perp vs. Z Hadronic Activity",293,139,700,500);
  c2.Update();
  c2->Divide(1,1);
  c2.cd( 1 );
  gStyle->SetLineColor( 2 );
  float xxx=0.25;
  float yyy=0.9;
  float yyy2=0.85;
  float yyy3=0.8;
  float yyy4=0.75;
  float yyy5=0.7;
  float yyy6=0.65;
  float yyy7=0.6;

  TGraphErrors* gr = new TGraphErrors(nbinplot[0],nxbin[0],mean[0],ex,mean_err[0]);
  gr->SetTitle();
  gr->SetMarkerColor(1);
  gr->SetMarkerSize(0.9);
  gr->SetMarkerStyle(21);
  gr->Draw("AP"); 

  gr->SetMaximum(m_curvaslin);
  gr->SetMinimum(-m_curvaslin);
  gr->GetYaxis()->SetTitle("Linearity(Data_Res_Perp_HadActivity)");
  gr->GetXaxis()->SetTitle("HadActivity");
  myText( 0.72,0.85,1,"Z#rightarrow #mu#mu");
  myMarkerText(xxx,yyy,1,21,"1 Refined");

  TGraphErrors* gr = new TGraphErrors(nbinplot[1],nxbin[1],mean[1],ex,mean_err[1]);
  gr->SetTitle();
  gr->SetMarkerColor(2);
  gr->SetMarkerSize(0.9);
  gr->SetMarkerStyle(21);
  gr->Draw("P"); 
  gr->SetMaximum(m_curvaslin);
  gr->SetMinimum(-m_curvaslin);
  myMarkerText(xxx,yyy2,2,21,"2 Topo");

  TGraphErrors* gr = new TGraphErrors(nbinplot[2],nxbin[2],mean[2],ex,mean_err[2]);
  gr->SetTitle();
  gr->SetMarkerColor(3);
  gr->SetMarkerSize(0.9);
  gr->SetMarkerStyle(21);
  gr->Draw("P"); 
  gr->SetMaximum(m_curvaslin);
  gr->SetMinimum(-m_curvaslin);
  myMarkerText(xxx,yyy3,3,21,"3 TopoCorr");

  TGraphErrors* gr = new TGraphErrors(nbinplot[3],nxbin[3],mean[3],ex,mean_err[3]);
  gr->SetTitle();
  gr->SetMarkerColor(4);
  gr->SetMarkerSize(0.9);
  gr->SetMarkerStyle(21);
  gr->Draw("P"); 
  gr->SetMaximum(m_curvaslin);
  gr->SetMinimum(-m_curvaslin);
  myMarkerText(xxx,yyy4,4,21,"4 Final");

  TGraphErrors* gr = new TGraphErrors(nbinplot[4],nxbin[4],mean[4],ex,mean_err[4]);
  gr->SetTitle();
  gr->SetMarkerColor(5);
  gr->SetMarkerSize(0.9);
  gr->SetMarkerStyle(21);
  gr->Draw("P"); 
  gr->SetMaximum(m_curvaslin);
  gr->SetMinimum(-m_curvaslin);
  myMarkerText(xxx,yyy5,5,21,"5 LocHad");

  TGraphErrors* gr = new TGraphErrors(nbinplot[5],nxbin[5],mean[5],ex,mean_err[5]);
  gr->SetTitle();
  gr->SetMarkerColor(6);
  gr->SetMarkerSize(0.9);
  gr->SetMarkerStyle(21);
  gr->Draw("P"); 
  gr->SetMaximum(m_curvaslin);
  gr->SetMinimum(-m_curvaslin);
  myMarkerText(xxx,yyy6,6,21,"6 Base");

  TGraphErrors* gr = new TGraphErrors(nbinplot[6],nxbin[6],mean[6],ex,mean_err[6]);
  gr->SetTitle();
  gr->SetMarkerColor(7);
  gr->SetMarkerSize(0.9);
  gr->SetMarkerStyle(21);
  gr->Draw("P"); 
  gr->SetMaximum(m_curvaslin);
  gr->SetMinimum(-m_curvaslin);
  myMarkerText(xxx,yyy7,7,21,"7 Calib");

  c2.Print("ZMuMu_Lin_Data_Res_ParPerp2_HadActivity.eps");

}//end 
