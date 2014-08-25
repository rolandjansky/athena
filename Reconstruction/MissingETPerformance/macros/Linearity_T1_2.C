/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
///////////////////////////////////////////////////////////////////////////////////
// histos for Etmiss Linearity vs true EtMiss 
// take from the resolution plots in EtMiss bins the
// fit parameters, plot and fit them  
//
// Open the root file
TFile file("METPerformance.T1.root");
file->cd( "Linearity/Binned" );
file.ls();

#include "utils.h"
//gROOT->SetStyle("ATLAS");
//gROOT->ForceStyle();
gStyle->SetOptStat(111111);
gStyle->SetOptFit(111111);
gStyle->SetOptFit(0);

int m_fitrangel=2;
float  m_percerr=0.15;
int m_entries=100;

int m_lnumptt;
int m_lbinpt;
int m_lstartpt;
float  m_curvasETmi;  

//curvalinearity---------------------------------
m_lnumptt=60;
m_lstartpt=0.;
m_lbinpt=5.;
//True Linearity (EtMissTruth-EtMissRec)/EtMissTruth

//events with true EtMiss
//       m_curvaslin=0.3;
float m_curvaslin=0.1;

//mytext... events with true etMiss=0 --> Zee
//       m_curvaslin=10.;
//       m_curvaslin=5.;
//----------------------------------------------

int cp=sqrt((float)m_lnumptt)+1;
m_lstartpt=0.;
m_curvasETmi=m_lstartpt+m_lnumptt*m_lbinpt;

//first
char name[m_lnumptt];
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

float xbin;
// book histo curva resol vs SumET 
he_resvset = new TH2F("resvset","resvset",100,0.,1000.,100,0.,20.);

TCanvas *c1 = new TCanvas("c1", "c1",293,139,700,500);
c1->Update();
c1->Divide(cp,cp);
//

int ih=0;
for (int i= 1; i< m_lnumptt ; i++){

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
    //    if ( ih < 4) { m_fitr=15;} 

    if (m_fitrangel == 1) {
      hist->Fit( "gaus","","",-m_fitr,m_fitr );
    }
    //fit on the whole plot range............................
    if (m_fitrangel == 2) {
      hist->Fit( "gaus","","");
    }

    TF1 *fitsf = hist->GetFunction("gaus");
    if (!fitsf) continue;
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
  }
}
c1->Print("many_linear1.ps");

//second
char name2[m_lnumptt];
double con2[m_lnumptt];
double mean2[m_lnumptt];
double sigma2[m_lnumptt];
double con_err2[m_lnumptt];
double mean_err2[m_lnumptt];
double sigma_err2[m_lnumptt];
double ex2[m_lnumptt];
double nxbin2[m_lnumptt];
float expsigmaf2[m_lnumptt]=0.;
int nbinplot2=0;

float xbin;

int ih=0;
for (int i= 1; i< m_lnumptt ; i++){
  c1->cd(i+1);
  sprintf(name,"hl11_%d",i);
  TH1F* hist  =(TH1F*)gDirectory->Get(name);

  int entries = hist->GetEntries();
  sigma2[ih]=-10005.;
  sigma_err2[ih]=1000.;
  if  ( entries> m_entries) {

    xbin = m_lstartpt+m_lbinpt/2.+ i * m_lbinpt;

    //    if( xbin< m_curvasETmi && entries > m_nentries) {

    //choice of m_fitr........................
    float m_fitr=2.5;
    //    if ( ih < 4) { m_fitr=15;} 

    if (m_fitrangel == 1) {
      hist->Fit( "gaus","","",-m_fitr,m_fitr );
    }
    //fit on the whole plot range............................
    if (m_fitrangel == 2) {
      hist->Fit( "gaus","","");
    }

    TF1 *fitsf2 = hist->GetFunction("gaus");
    if (!fitsf2) continue;

    float sigma_first = fitsf2->GetParameter(2);
    float err_first = fitsf2->GetParError(2);
 
    nbinplot2++;
    con2[ih] = fitsf2->GetParameter(0);
    mean2[ih] = fitsf2->GetParameter(1);
    sigma2[ih] = fitsf2->GetParameter(2);
    con_err2[ih] = fitsf2->GetParError(0);
    mean_err2[ih] = fitsf2->GetParError(1);
    sigma_err2[ih] = fitsf2->GetParError(2);
  
    nxbin2[ih]=xbin;
    ex2[ih]=0.;
    he_resvset->Fill(xbin, sigma2[ih]);
    //choice of points.....................
    if(sigma2[ih] > 0.) {
      double rappo= sigma_err2[ih]/sigma2[ih];
      if ( rappo > m_percerr ) {
	sigma2[ih]=-10005.;
	sigma_err2[ih]=1000.;
      }
    } 
    ih++;
  }
}
c1->Print("many_linear1.ps");

//third
char name3[m_lnumptt];
double con3[m_lnumptt];
double mean3[m_lnumptt];
double sigma3[m_lnumptt];
double con_err3[m_lnumptt];
double mean_err3[m_lnumptt];
double sigma_err3[m_lnumptt];
double ex3[m_lnumptt];
double nxbin3[m_lnumptt];
float expsigmaf3[m_lnumptt]=0.;
int nbinplot3=0;

float xbin;

int ih=0;
for (int i= 1; i< m_lnumptt ; i++){
  c1->cd(i+1);
  sprintf(name,"hl12_%d",i);
  TH1F* hist  =(TH1F*)gDirectory->Get(name);

  int entries = hist->GetEntries();
  sigma3[ih]=-10005.;
  sigma_err3[ih]=1000.;
  if  ( entries> m_entries) {

    xbin = m_lstartpt+m_lbinpt/2.+ i * m_lbinpt;
    //    if( xbin< m_curvasETmi && entries > m_nentries) {

    //choice of m_fitr........................
    float m_fitr=2.5;
    //    if ( ih < 4) { m_fitr=15;} 

    if (m_fitrangel == 1) {
      hist->Fit( "gaus","","",-m_fitr,m_fitr );
    }
    //fit on the whole plot range............................
    if (m_fitrangel == 2) {
      hist->Fit( "gaus","","");
    }

    TF1 *fitsf3 = hist->GetFunction("gaus");
    if (!fitsf3) continue;
    float sigma_first = fitsf3->GetParameter(2);
    float err_first = fitsf3->GetParError(2);
 
    nbinplot3++;
    con3[ih] = fitsf3->GetParameter(0);
    mean3[ih] = fitsf3->GetParameter(1);
    sigma3[ih] = fitsf3->GetParameter(2);
    con_err3[ih] = fitsf3->GetParError(0);
    mean_err3[ih] = fitsf3->GetParError(1);
    sigma_err3[ih] = fitsf3->GetParError(2);
  
    nxbin3[ih]=xbin;
    ex3[ih]=0.;
    he_resvset->Fill(xbin, sigma3[ih]);
    //choice of points.....................
    if(sigma3[ih] > 0.) {
      double rappo= sigma_err3[ih]/sigma3[ih];
      if ( rappo > m_percerr ) {
	sigma3[ih]=-10005.;
	sigma_err3[ih]=1000.;
      }
    } 
    ih++;
  }
}
c1->Print("many_linear1.ps");

//fourth
char name4[m_lnumptt];
double con4[m_lnumptt];
double mean4[m_lnumptt];
double sigma4[m_lnumptt];
double con_err4[m_lnumptt];
double mean_err4[m_lnumptt];
double sigma_err4[m_lnumptt];
double ex4[m_lnumptt];
double nxbin4[m_lnumptt];
float expsigmaf4[m_lnumptt]=0.;
int nbinplot4=0;

float xbin;

int ih=0;
for (int i= 1; i< m_lnumptt ; i++){
  c1->cd(i+1);
  sprintf(name,"hl13_%d",i);
  TH1F* hist  =(TH1F*)gDirectory->Get(name);

  int entries = hist->GetEntries();
  sigma4[ih]=-10005.;
  sigma_err4[ih]=1000.;
  if  ( entries> m_entries) {

    xbin = m_lstartpt+m_lbinpt/2.+ i * m_lbinpt;
    //    if( xbin< m_curvasETmi && entries > m_nentries) {

    //choice of m_fitr........................
    float m_fitr=2.5;
    //    if ( ih < 4) { m_fitr=15;} 

    if (m_fitrangel == 1) {
      hist->Fit( "gaus","","",-m_fitr,m_fitr );
    }
    //fit on the whole plot range............................
    if (m_fitrangel == 2) {
      hist->Fit( "gaus","","");
    }

    TF1 *fitsf4 = hist->GetFunction("gaus");
    if (!fitsf4) continue;
    float sigma_first = fitsf4->GetParameter(2);
    float err_first = fitsf4->GetParError(2);
 
    nbinplot4++;
    con4[ih] = fitsf4->GetParameter(0);
    mean4[ih] = fitsf4->GetParameter(1);
    sigma4[ih] = fitsf4->GetParameter(2);
    con_err4[ih] = fitsf4->GetParError(0);
    mean_err4[ih] = fitsf4->GetParError(1);
    sigma_err4[ih] = fitsf4->GetParError(2);
  
    nxbin4[ih]=xbin;
    ex4[ih]=0.;
    he_resvset->Fill(xbin, sigma4[ih]);
    //choice of points.....................
    if(sigma4[ih] > 0.) {
      double rappo= sigma_err4[ih]/sigma4[ih];
      if ( rappo > m_percerr ) {
	sigma4[ih]=-10005.;
	sigma_err4[ih]=1000.;
      }
    } 
    ih++;
  }
}
c1->Print("many_linear1.ps");

//fifth
char name5[m_lnumptt];
double con5[m_lnumptt];
double mean5[m_lnumptt];
double sigma5[m_lnumptt];
double con_err5[m_lnumptt];
double mean_err5[m_lnumptt];
double sigma_err5[m_lnumptt];
double ex5[m_lnumptt];
double nxbin5[m_lnumptt];
float expsigmaf5[m_lnumptt]=0.;
int nbinplot5=0;

float xbin;

int ih=0;
for (int i= 1; i< m_lnumptt ; i++){
  c1->cd(i+1);
  sprintf(name,"hl14_%d",i);
  TH1F* hist  =(TH1F*)gDirectory->Get(name);

  int entries = hist->GetEntries();
  sigma5[ih]=-10005.;
  sigma_err5[ih]=1000.;
  if  ( entries> m_entries) {

    xbin = m_lstartpt+m_lbinpt/2.+ i * m_lbinpt;
    //    if( xbin< m_curvasETmi && entries > m_nentries) {

    //choice of m_fitr........................
    float m_fitr=2.5;
    //    if ( ih < 4) { m_fitr=15;} 

    if (m_fitrangel == 1) {
      hist->Fit( "gaus","","",-m_fitr,m_fitr );
    }
    //fit on the whole plot range............................
    if (m_fitrangel == 2) {
      hist->Fit( "gaus","","");
    }

    TF1 *fitsf5 = hist->GetFunction("gaus");
    if (!fitsf5) continue;
    float sigma_first = fitsf5->GetParameter(2);
    float err_first = fitsf5->GetParError(2);
 
    nbinplot5++;
    con5[ih] = fitsf5->GetParameter(0);
    mean5[ih] = fitsf5->GetParameter(1);
    sigma5[ih] = fitsf5->GetParameter(2);
    con_err5[ih] = fitsf5->GetParError(0);
    mean_err5[ih] = fitsf5->GetParError(1);
    sigma_err5[ih] = fitsf5->GetParError(2);
  
    nxbin5[ih]=xbin;
    ex5[ih]=0.;
    he_resvset->Fill(xbin, sigma5[ih]);
    //choice of points.....................
    if(sigma5[ih] > 0.) {
      double rappo= sigma_err5[ih]/sigma5[ih];
      if ( rappo > m_percerr ) {
	sigma5[ih]=-10005.;
	sigma_err5[ih]=1000.;
      }
    } 
    ih++;
  }
}
c1->Print("many_linear1.ps");

//sixth
char name6[m_lnumptt];
double con6[m_lnumptt];
double mean6[m_lnumptt];
double sigma6[m_lnumptt];
double con_err6[m_lnumptt];
double mean_err6[m_lnumptt];
double sigma_err6[m_lnumptt];
double ex6[m_lnumptt];
double nxbin6[m_lnumptt];
float expsigmaf6[m_lnumptt]=0.;
int nbinplot6=0;

float xbin;

int ih=0;
for (int i= 1; i< m_lnumptt ; i++){
  c1->cd(i+1);
  sprintf(name,"hl15_%d",i);
  TH1F* hist  =(TH1F*)gDirectory->Get(name);

  int entries = hist->GetEntries();
  sigma6[ih]=-10005.;
  sigma_err6[ih]=1000.;
  if  ( entries> m_entries) {

    xbin = m_lstartpt+m_lbinpt/2.+ i * m_lbinpt;
    //    if( xbin< m_curvasETmi && entries > m_nentries) {

    //choice of m_fitr........................
    float m_fitr=2.5;
    //    if ( ih < 4) { m_fitr=15;} 

    if (m_fitrangel == 1) {
      hist->Fit( "gaus","","",-m_fitr,m_fitr );
    }
    //fit on the whole plot range............................
    if (m_fitrangel == 2) {
      hist->Fit( "gaus","","");
    }

    TF1 *fitsf6 = hist->GetFunction("gaus");
    if (!fitsf6) continue;
    float sigma_first = fitsf6->GetParameter(2);
    float err_first = fitsf6->GetParError(2);
 
    nbinplot6++;
    con6[ih] = fitsf6->GetParameter(0);
    mean6[ih] = fitsf6->GetParameter(1);
    sigma6[ih] = fitsf6->GetParameter(2);
    con_err6[ih] = fitsf6->GetParError(0);
    mean_err6[ih] = fitsf6->GetParError(1);
    sigma_err6[ih] = fitsf6->GetParError(2);
  
    nxbin6[ih]=xbin;
    ex6[ih]=0.;
    he_resvset->Fill(xbin, sigma6[ih]);
    //choice of points.....................
    if(sigma6[ih] > 0.) {
      double rappo= sigma_err6[ih]/sigma6[ih];
      if ( rappo > m_percerr ) {
	sigma6[ih]=-10005.;
	sigma_err6[ih]=1000.;
      }
    } 
    ih++;
  }
}
c1->Print("many_linear1.ps");








//seventh
char name7[m_lnumptt];
double con7[m_lnumptt];
double mean7[m_lnumptt];
double sigma7[m_lnumptt];
double con_err7[m_lnumptt];
double mean_err7[m_lnumptt];
double sigma_err7[m_lnumptt];
double ex7[m_lnumptt];
double nxbin7[m_lnumptt];
float expsigmaf7[m_lnumptt]=0.;
int nbinplot7=0;

float xbin;

int ih=0;
for (int i= 1; i< m_lnumptt ; i++){
  c1->cd(i+1);
  sprintf(name,"hl16_%d",i);
  TH1F* hist  =(TH1F*)gDirectory->Get(name);

  int entries = hist->GetEntries();
  sigma7[ih]=-10005.;
  sigma_err7[ih]=1000.;
  if  ( entries> m_entries) {

    xbin = m_lstartpt+m_lbinpt/2.+ i * m_lbinpt;
    //    if( xbin< m_curvasETmi && entries > m_nentries) {

    //choice of m_fitr........................
    float m_fitr=2.5;
    //    if ( ih < 4) { m_fitr=15;} 

    if (m_fitrangel == 1) {
      hist->Fit( "gaus","","",-m_fitr,m_fitr );
    }
    //fit on the whole plot range............................
    if (m_fitrangel == 2) {
      hist->Fit( "gaus","","");
    }

    TF1 *fitsf7 = hist->GetFunction("gaus");
    if (!fitsf7) continue;
    float sigma_first = fitsf7->GetParameter(2);
    float err_first = fitsf7->GetParError(2);
 
    nbinplot7++;
    con7[ih] = fitsf7->GetParameter(0);
    mean7[ih] = fitsf7->GetParameter(1);
    sigma7[ih] = fitsf7->GetParameter(2);
    con_err7[ih] = fitsf7->GetParError(0);
    mean_err7[ih] = fitsf7->GetParError(1);
    sigma_err7[ih] = fitsf7->GetParError(2);
  
    nxbin7[ih]=xbin;
    ex7[ih]=0.;
    he_resvset->Fill(xbin, sigma7[ih]);
    //choice of points.....................
    if(sigma7[ih] > 0.) {
      double rappo= sigma_err7[ih]/sigma7[ih];
      if ( rappo > m_percerr ) {
	sigma7[ih]=-10005.;
	sigma_err7[ih]=1000.;
      }
    } 
    ih++;
  }
}
c1->Print("many_linear1.ps");

TCanvas* c2 = new TCanvas("c2","Linearity",293,139,700,500);
c2->SetFillColor(0);
c2->Update();
//c2->Divide(1,1);
//c2->cd( 1 );

//Linearity
TGraphErrors* gr = new TGraphErrors(nbinplot,nxbin,mean,ex,mean_err);

gr->SetTitle();

gr->SetFillColor(0);
gr->SetMarkerColor(1);
gr->SetMarkerStyle(21);
//gr->Draw("ALP"); 
gr->Draw("AP"); 
gr->SetMaximum(m_curvaslin);
gr->SetMinimum(-m_curvaslin);
gr->GetYaxis()->SetTitle("Linearity of response");
gr->GetXaxis()->SetTitle("True E_{T}^{miss}  (GeV)");

//myText( 0.72,0.85,1,"W#rightarrow e#nu");
//myText( 0.72,0.85,1,"Z#rightarrow ee");
myText( 0.14,0.85,1,"T1 Sample");

//TLegend *leg = new TLegend(0.55, 0.75, 0.85, 0.95);
TLegend *leg = new TLegend(0.5, 0.13, 0.88, 0.37);
char message[50];
sprintf(message,"1 Refined Default" );
leg->AddEntry(gr,message,"pl");

//second
TGraphErrors* gr = new TGraphErrors(nbinplot2,nxbin,mean2,ex2,mean_err2);

gr->SetMarkerColor(2);
gr->SetMarkerStyle(21);
gr->Draw("P"); 
gr->SetMaximum(m_curvaslin);
gr->SetMinimum(-m_curvaslin);

sprintf(message,"2 LCW_pt20" );
leg->AddEntry(gr,message,"pl");

//third
TGraphErrors* gr = new TGraphErrors(nbinplot3,nxbin,mean3,ex3,mean_err3);

gr->SetMarkerColor(3);
gr->SetMarkerStyle(21);
gr->Draw("P"); 
gr->SetMaximum(m_curvaslin);
gr->SetMinimum(-m_curvaslin);

sprintf(message,"3 GCW_pt20");
leg->AddEntry(gr,message,"pl");

//fourth
TGraphErrors* gr = new TGraphErrors(nbinplot4,nxbin,mean4,ex4,mean_err4);

gr->SetMarkerColor(4);
gr->SetMarkerStyle(21);
gr->Draw("P"); 
gr->SetMaximum(m_curvaslin);
gr->SetMinimum(-m_curvaslin);

sprintf(message,"4 LCW_NI_pt20_noSoftJets_eflow");
leg->AddEntry(gr,message,"pl");

//fifth
TGraphErrors* gr = new TGraphErrors(nbinplot5,nxbin,mean5,ex5,mean_err5);

gr->SetMarkerColor(5);
gr->SetMarkerStyle(21);
gr->Draw("P"); 
gr->SetMaximum(m_curvaslin);
gr->SetMinimum(-m_curvaslin);

sprintf(message,"5 GCW_NI_pt20_Muid"  );
leg->AddEntry(gr,message,"pl");

//sixth
TGraphErrors* gr = new TGraphErrors(nbinplot6,nxbin,mean6,ex6,mean_err6);

gr->SetMarkerColor(6);
gr->SetMarkerStyle(21);
gr->Draw("P"); 
gr->SetMaximum(m_curvaslin);
gr->SetMinimum(-m_curvaslin);

sprintf(message,"6 LCW_NI_pt20_Muid_eflow"  );
leg->AddEntry(gr,message,"pl");

//seventh
TGraphErrors* gr = new TGraphErrors(nbinplot7,nxbin,mean7,ex7,mean_err7);

gr->SetMarkerColor(7);
gr->SetMarkerStyle(21);
gr->Draw("P"); 
gr->SetMaximum(m_curvaslin);
gr->SetMinimum(-m_curvaslin);

sprintf(message,"7 emjes_cellout_em" );
leg->AddEntry(gr,message,"pl");

c2->Update();
leg->SetFillColor(0);
leg->Draw();
//end Graph

TLine b22(0.,0.,260.,0.);
//blue
b22->SetLineColor(4);
//b1->SetLineColor(1);
b22->SetLineStyle(1); // 1 = linea tratteggiata
b22->Draw();

/*
TLine b23(0.,-4.,105.,-4.);
//blue
b23->SetLineColor(4);
//b1->SetLineColor(1);
b23->SetLineStyle(2); // 1 = linea tratteggiata
b23->Draw();

TLine b24(0.,4.,105.,4.);
//blue
b24->SetLineColor(4);
//b1->SetLineColor(1);
b24->SetLineStyle(2); // 1 = linea tratteggiata
b24->Draw();
*/

c2->Update();
c2->SetFillColor(0);

c2->Print("Linearity_T1_2.eps");
c2->Print("Linearity_T1_2.gif");
c2->Update();
}
