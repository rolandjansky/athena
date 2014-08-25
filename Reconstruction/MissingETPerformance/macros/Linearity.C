/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  ///////////////////////////////////////////////////////////////////////////////////
  // histos for Etmiss Linearity vs true EtMiss 
  // take from the resolution plots in EtMiss bins the
  // fit parameters, plot and fit them  
  ///////////////////////////////////////////////////////////////////////////////////
  #include "utils.h"

  // Open the root file
  TFile file("METPerformance.root");
  gDirectory->cd("Linearity/Binned");

  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();
  gStyle->SetOptStat(111111);
  gStyle->SetOptFit(111111);
  gStyle->SetOptFit(0);

  int m_fitrangel=2;
  float  m_percerr=0.15;
  int m_entries=50;

  int m_lnumptt;
  int m_lbinpt;
  int m_lstartpt;
  float  m_curvasETmi;  
  float m_curvaslin;
  //curvalinearity---------------------------------
  m_lnumptt=20;
  m_lstartpt=0.;
  m_lbinpt=5.;
  m_curvaslin=0.3;

  int cp=sqrt((float)m_lnumptt)+1;
  m_lstartpt=0.;
  char name[m_lnumptt];
  m_curvasETmi=m_lstartpt+m_lnumptt*m_lbinpt;
  double con[m_lnumptt];
  double mean[m_lnumptt];
  double sigma[m_lnumptt];
  double con_err[m_lnumptt];
  double mean_err[m_lnumptt];
  double sigma_err[m_lnumptt];
  double ex[m_lnumptt];
  double nxbin[m_lnumptt];
  float expsigmaf[m_lnumptt]=0.;
  int nbinplot=0;
  double conatl[m_lnumptt];
  double meanatl[m_lnumptt];
  double sigmaatl[m_lnumptt];
  double con_erratl[m_lnumptt];
  double mean_erratl[m_lnumptt];
  double sigma_erratl[m_lnumptt];
  double exatl[m_lnumptt];
  double nxbinatl[m_lnumptt];
  int nbinplotatl=0;

  float xbin;
  // book histo curva resol vs SumET 
  he_resvset = new TH2F("resvset","resvset",100,0.,1000.,100,0.,20.);

  TCanvas *c1 = new TCanvas("c1", "c1",293,139,700,500);
  c1.Update();
  c1.Divide(cp,cp);

  int ih=0;
  for (int i= 0; i< m_lnumptt ; i++){
    c1->cd(i+1);
    sprintf(name,"hl_%d",i);
    TH1F* hist  =(TH1F*)gDirectory->Get(name);

    int entries = hist->GetEntries();
    sigma[ih]=-10005.;
    sigma_err[ih]=1000.;
    if  ( entries> m_entries) {

      xbin = m_lstartpt+m_lbinpt/2.+ i * m_lbinpt;

      //choice of m_fitr........................
      float m_fitr=2.5;

      if (m_fitrangel == 1) {
	hist->Fit( "gaus","","",-m_fitr,m_fitr );
      }
      //fit on the whole plot range............................
      if (m_fitrangel == 2) {
	hist->Fit( "gaus","","");
      }

      TF1 *fitsf = hist->GetFunction("gaus");
      float sigma_first = fitsf->GetParameter(2);
      float err_first = fitsf->GetParError(2);
 
      nbinplot++;
      con[ih] = fitsf->GetParameter(0);
      mean[ih] = fitsf->GetParameter(1);
      sigma[ih] = fitsf->GetParameter(2);
      con_err[ih] = fitsf->GetParError(0);
      mean_err[ih] = fitsf->GetParError(1);
      sigma_err[ih] = fitsf->GetParError(2);
  
      nxbin[ih]=xbin;
      ex[ih]=0.;
      he_resvset->Fill(xbin, sigma[ih]);
      //choice of points.....................
      if(sigma[ih] > 0.) {
	double rappo= sigma_err[ih]/sigma[ih];
	if ( rappo > m_percerr ) {
	  sigma[ih]=-10005.;
	  sigma_err[ih]=1000.;
	}
      }
      ih++;
    }//if enough entries in this histogram
  }//end loop over histos
  c1.Print("many_linear1_wenu.ps");

  TCanvas* c2 = new TCanvas("c2"," Resolution vs Sumet from Topo (A0300)",293,139,700,500);
  c2.Update();
  c2->Divide(1,1);
  c2.cd( 1 );

  //Linearity
  TGraphErrors* gr = new TGraphErrors(m_lnumptt,nxbin,mean,ex,mean_err);
  gr->SetTitle();

  gr->SetMarkerColor(1);
  gr->SetMarkerStyle(21);
  gr->Draw("AP"); 
  gr->SetMaximum(m_curvaslin);
  gr->SetMinimum(-m_curvaslin);
  gr->GetYaxis()->SetTitle("Linearity of response");
  gr->GetXaxis()->SetTitle("True E_{T}^{miss}  (GeV)");

  float xxx=0.40;
  myMarkerText(xxx,0.25,1,21,"E_{T}^{miss} refined calibration");
  myText( 0.72,0.85,1,"W#rightarrow e#nu em250");

  TLine b22(0.,0.,105.,0.);
  //blue
  b22->SetLineColor(4);
  //b1->SetLineColor(1);
  b22->SetLineStyle(1); // 1 = linea tratteggiata
  b22->Draw();

  TLine b23(0.,-4.,105.,-4.);
  //blue
  b23->SetLineColor(4);
  b23->SetLineStyle(2); // 1 = linea tratteggiata
  b23->Draw();

  TLine b24(0.,4.,105.,4.);
  //blue
  b24->SetLineColor(4);
  b24->SetLineStyle(2); // 1 = linea tratteggiata
  b24->Draw();

  c2.Update();
  c2.Print("Linear1_wenu.eps");
  c2.Update();
}//end Linearity.C
