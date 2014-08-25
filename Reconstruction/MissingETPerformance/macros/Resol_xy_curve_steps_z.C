/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
///////////////////////////////////////////////////////////////////////////////////
// histos for Etmiss Resol vs Sumet  
// take from the resolution plots in Sumet bins the
// fit parameters, plot and fit them to make the curve 

// Open the root file

TFile file("../METPerformance_ztautau.root");

file->cd( "Resolution/Binned" );

file.ls();

 #include "utils.h"
gROOT->SetStyle("ATLAS");
gROOT->ForceStyle();
gStyle->SetOptStat(111111);
gStyle->SetOptFit(0);

int m_fitrange=3;
float m_ns=2.;
float  m_percerr=0.15;
int m_entries=100;

int m_numptt;
int m_binpt;
int m_startpt;
m_curvasres=10;

//m_numptt=20;
m_numptt=40;
m_binpt=10;
m_startpt=5;
m_curvasET=m_numptt*m_binpt;
  
//first
char name[m_numptt];
m_curvasET=m_startpt+m_numptt*m_binpt;
double mean[m_numptt];
double sigma[m_numptt];
double mean_err[m_numptt];
double sigma_err[m_numptt];
double ex[m_numptt];
double nxbin[m_numptt];
float expsigmaf[m_numptt]=0.;
int nbinplot=0;
float xbin;


int cp=sqrt((float)m_numptt)+1;
TCanvas *c1 = new TCanvas("c1", "c1",293,139,700,500);
c1.Update();
c1.Divide(cp,cp);
//

int ih=0;
for (int i= 0; i< m_numptt ; i++){
  c1->cd(i+1);
  sprintf(name,"h_%d",i);
  std::cout<<"name" <<name<<std::endl;
  TH1F* hist  =(TH1F*)gDirectory->Get(name);

  int entries = hist->GetEntries();
  std::cout << "entries"<<entries <<std::endl; 
     sigma[ih]=-10005.;
     sigma_err[ih]=1000.;
  xbin = m_startpt+m_binpt/2.+ i * m_binpt;
    nxbin[ih]=xbin;
    ex[ih]=0.;

     if  ( entries> m_entries) {

//fit on the whole plot range............................
       if (m_fitrange == 2) {
     hist->Fit( "gaus","","");
       }
//fit in m_ns sigma (sigma=0.5*sqrt(SumET bin)..........
       if (m_fitrange == 3) {
     expsigma=0.5*sqrt(xbin);
     hist->Fit( "gaus","","",-m_ns*expsigma,m_ns*expsigma );
       }
  TF1 *fitsf = hist->GetFunction("gaus");

    nbinplot++;
    mean[ih] = fitsf->GetParameter(1);
    sigma[ih] = fitsf->GetParameter(2);
    mean_err[ih] = fitsf->GetParError(1);
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
    }
}
c1.Print("many_plot_xy_step1.ps");

//second
char name2[m_numptt];
m_curvasET=m_startpt+m_numptt*m_binpt;
double mean2[m_numptt];
double sigma2[m_numptt];
double mean_err2[m_numptt];
double sigma_err2[m_numptt];
double ex2[m_numptt];
double nxbin2[m_numptt];
float expsigmaf2[m_numptt]=0.;
int nbinplot2=0;
float xbin;


int cp=sqrt((float)m_numptt)+1;
TCanvas *c1 = new TCanvas("c1", "c1",293,139,700,500);
c1.Update();
c1.Divide(cp,cp);
//

int ih=0;
for (int i= 0; i< m_numptt ; i++){
  c1->cd(i+1);
  sprintf(name,"h2_%d",i);
  std::cout<<"name" <<name<<std::endl;
  TH1F* hist  =(TH1F*)gDirectory->Get(name);

  int entries = hist->GetEntries();
  std::cout << "entries"<<entries <<std::endl; 
     sigma2[ih]=-10005.;
     sigma_err2[ih]=1000.;
  xbin = m_startpt+m_binpt/2.+ i * m_binpt;
    nxbin2[ih]=xbin;
    ex2[ih]=0.;

     if  ( entries> m_entries) {

//fit on the whole plot range............................
       if (m_fitrange == 2) {
     hist->Fit( "gaus","","");
       }
//fit in m_ns sigma (sigma=0.5*sqrt(SumET bin)..........
       if (m_fitrange == 3) {
     expsigma=0.5*sqrt(xbin);
     hist->Fit( "gaus","","",-m_ns*expsigma,m_ns*expsigma );
       }
  TF1 *fitsf = hist->GetFunction("gaus");

    nbinplot2++;
    mean2[ih] = fitsf->GetParameter(1);
    sigma2[ih] = fitsf->GetParameter(2);
    mean_err2[ih] = fitsf->GetParError(1);
    sigma_err2[ih] = fitsf->GetParError(2);
    expsigmaf2[ih] =m_ns*expsigma;
  
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
c1.Print("many_plot_xy_step2.ps");

//third
char name3[m_numptt];
m_curvasET=m_startpt+m_numptt*m_binpt;
double mean3[m_numptt];
double sigma3[m_numptt];
double mean_err3[m_numptt];
double sigma_err3[m_numptt];
double ex3[m_numptt];
double nxbin3[m_numptt];
float expsigmaf3[m_numptt]=0.;
int nbinplot3=0;
float xbin;


int cp=sqrt((float)m_numptt)+1;
TCanvas *c1 = new TCanvas("c1", "c1",293,139,700,500);
c1.Update();
c1.Divide(cp,cp);
//

int ih=0;
for (int i= 0; i< m_numptt ; i++){
  c1->cd(i+1);
  sprintf(name,"h3_%d",i);
  std::cout<<"name" <<name<<std::endl;
  TH1F* hist  =(TH1F*)gDirectory->Get(name);

  int entries = hist->GetEntries();
  std::cout << "entries"<<entries <<std::endl; 
     sigma3[ih]=-10005.;
     sigma_err3[ih]=1000.;
  xbin = m_startpt+m_binpt/2.+ i * m_binpt;
    nxbin3[ih]=xbin;
    ex3[ih]=0.;

     if  ( entries> m_entries) {

//fit on the whole plot range............................
       if (m_fitrange == 2) {
     hist->Fit( "gaus","","");
       }
//fit in m_ns sigma (sigma=0.5*sqrt(SumET bin)..........
       if (m_fitrange == 3) {
     expsigma=0.5*sqrt(xbin);
     hist->Fit( "gaus","","",-m_ns*expsigma,m_ns*expsigma );
       }
  TF1 *fitsf = hist->GetFunction("gaus");

    nbinplot3++;
    mean3[ih] = fitsf->GetParameter(1);
    sigma3[ih] = fitsf->GetParameter(2);
    mean_err3[ih] = fitsf->GetParError(1);
    sigma_err3[ih] = fitsf->GetParError(2);
    expsigmaf3[ih] =m_ns*expsigma;
  
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
c1.Print("many_plot_xy_step3.ps");

//fourth
char name4[m_numptt];
m_curvasET=m_startpt+m_numptt*m_binpt;
double mean4[m_numptt];
double sigma4[m_numptt];
double mean_err4[m_numptt];
double sigma_err4[m_numptt];
double ex4[m_numptt];
double nxbin4[m_numptt];
float expsigmaf4[m_numptt]=0.;
int nbinplot4=0;
float xbin;


int cp=sqrt((float)m_numptt)+1;
TCanvas *c1 = new TCanvas("c1", "c1",293,139,700,500);
c1.Update();
c1.Divide(cp,cp);
//

int ih=0;
for (int i= 0; i< m_numptt ; i++){
  c1->cd(i+1);
  sprintf(name,"h4_%d",i);
  std::cout<<"name" <<name<<std::endl;
  TH1F* hist  =(TH1F*)gDirectory->Get(name);

  int entries = hist->GetEntries();
  std::cout << "entries"<<entries <<std::endl; 
     sigma4[ih]=-10005.;
     sigma_err4[ih]=1000.;
  xbin = m_startpt+m_binpt/2.+ i * m_binpt;
    nxbin4[ih]=xbin;
    ex4[ih]=0.;

     if  ( entries> m_entries) {

//fit on the whole plot range............................
       if (m_fitrange == 2) {
     hist->Fit( "gaus","","");
       }
//fit in m_ns sigma (sigma=0.5*sqrt(SumET bin)..........
       if (m_fitrange == 3) {
     expsigma=0.5*sqrt(xbin);
     hist->Fit( "gaus","","",-m_ns*expsigma,m_ns*expsigma );
       }
  TF1 *fitsf = hist->GetFunction("gaus");

    nbinplot4++;
    mean4[ih] = fitsf->GetParameter(1);
    sigma4[ih] = fitsf->GetParameter(2);
    mean_err4[ih] = fitsf->GetParError(1);
    sigma_err4[ih] = fitsf->GetParError(2);
    expsigmaf4[ih] =m_ns*expsigma;
  
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
c1.Print("many_plot_xy_step4.ps");

//fifth
char name5[m_numptt];
m_curvasET=m_startpt+m_numptt*m_binpt;
double mean5[m_numptt];
double sigma5[m_numptt];
double mean_err5[m_numptt];
double sigma_err5[m_numptt];
double ex5[m_numptt];
double nxbin5[m_numptt];
float expsigmaf5[m_numptt]=0.;
int nbinplot5=0;
float xbin;


int cp=sqrt((float)m_numptt)+1;
TCanvas *c1 = new TCanvas("c1", "c1",293,139,700,500);
c1.Update();
c1.Divide(cp,cp);
//

int ih=0;
for (int i= 0; i< m_numptt ; i++){
  c1->cd(i+1);
  sprintf(name,"h5_%d",i);
  std::cout<<"name" <<name<<std::endl;
  TH1F* hist  =(TH1F*)gDirectory->Get(name);

  int entries = hist->GetEntries();
  std::cout << "entries"<<entries <<std::endl; 
     sigma5[ih]=-10005.;
     sigma_err5[ih]=1000.;
  xbin = m_startpt+m_binpt/2.+ i * m_binpt;
    nxbin5[ih]=xbin;
    ex5[ih]=0.;

     if  ( entries> m_entries) {

//fit on the whole plot range............................
       if (m_fitrange == 2) {
     hist->Fit( "gaus","","");
       }
//fit in m_ns sigma (sigma=0.5*sqrt(SumET bin)..........
       if (m_fitrange == 3) {
     expsigma=0.5*sqrt(xbin);
     hist->Fit( "gaus","","",-m_ns*expsigma,m_ns*expsigma );
       }
  TF1 *fitsf = hist->GetFunction("gaus");

    nbinplot5++;
    mean5[ih] = fitsf->GetParameter(1);
    sigma5[ih] = fitsf->GetParameter(2);
    mean_err5[ih] = fitsf->GetParError(1);
    sigma_err5[ih] = fitsf->GetParError(2);
    expsigmaf5[ih] =m_ns*expsigma;
  
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
c1.Print("many_plot_xy_step5.ps");

//sixth
char name6[m_numptt];
m_curvasET=m_startpt+m_numptt*m_binpt;
double mean6[m_numptt];
double sigma6[m_numptt];
double mean_err6[m_numptt];
double sigma_err6[m_numptt];
double ex6[m_numptt];
double nxbin6[m_numptt];
float expsigmaf6[m_numptt]=0.;
int nbinplot6=0;
float xbin;


int cp=sqrt((float)m_numptt)+1;
TCanvas *c1 = new TCanvas("c1", "c1",293,139,700,500);
c1.Update();
c1.Divide(cp,cp);
//

int ih=0;
for (int i= 0; i< m_numptt ; i++){
  c1->cd(i+1);
  sprintf(name,"h6_%d",i);
  std::cout<<"name" <<name<<std::endl;
  TH1F* hist  =(TH1F*)gDirectory->Get(name);

  int entries = hist->GetEntries();
  std::cout << "entries"<<entries <<std::endl; 
     sigma6[ih]=-10005.;
     sigma_err6[ih]=1000.;
  xbin = m_startpt+m_binpt/2.+ i * m_binpt;
    nxbin6[ih]=xbin;
    ex6[ih]=0.;

     if  ( entries> m_entries) {

//fit on the whole plot range............................
       if (m_fitrange == 2) {
     hist->Fit( "gaus","","");
       }
//fit in m_ns sigma (sigma=0.5*sqrt(SumET bin)..........
       if (m_fitrange == 3) {
     expsigma=0.5*sqrt(xbin);
     hist->Fit( "gaus","","",-m_ns*expsigma,m_ns*expsigma );
       }
  TF1 *fitsf = hist->GetFunction("gaus");

    nbinplot6++;
    mean6[ih] = fitsf->GetParameter(1);
    sigma6[ih] = fitsf->GetParameter(2);
    mean_err6[ih] = fitsf->GetParError(1);
    sigma_err6[ih] = fitsf->GetParError(2);
    expsigmaf6[ih] =m_ns*expsigma;
  
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
c1.Print("many_plot_xy_step6.ps");

//seventh
char name7[m_numptt];
m_curvasET=m_startpt+m_numptt*m_binpt;
double mean7[m_numptt];
double sigma7[m_numptt];
double mean_err7[m_numptt];
double sigma_err7[m_numptt];
double ex7[m_numptt];
double nxbin7[m_numptt];
float expsigmaf7[m_numptt]=0.;
int nbinplot7=0;
float xbin;


int cp=sqrt((float)m_numptt)+1;
TCanvas *c1 = new TCanvas("c1", "c1",293,139,700,500);
c1.Update();
c1.Divide(cp,cp);
//

int ih=0;
for (int i= 0; i< m_numptt ; i++){
  c1->cd(i+1);
  sprintf(name,"h7_%d",i);
  std::cout<<"name" <<name<<std::endl;
  TH1F* hist  =(TH1F*)gDirectory->Get(name);

  int entries = hist->GetEntries();
  std::cout << "entries"<<entries <<std::endl; 
     sigma7[ih]=-10005.;
     sigma_err7[ih]=1000.;
  xbin = m_startpt+m_binpt/2.+ i * m_binpt;
    nxbin7[ih]=xbin;
    ex7[ih]=0.;

     if  ( entries> m_entries) {

//fit on the whole plot range............................
       if (m_fitrange == 2) {
     hist->Fit( "gaus","","");
       }
//fit in m_ns sigma (sigma=0.5*sqrt(SumET bin)..........
       if (m_fitrange == 3) {
     expsigma=0.5*sqrt(xbin);
     hist->Fit( "gaus","","",-m_ns*expsigma,m_ns*expsigma );
       }
  TF1 *fitsf = hist->GetFunction("gaus");

    nbinplot7++;
    mean7[ih] = fitsf->GetParameter(1);
    sigma7[ih] = fitsf->GetParameter(2);
    mean_err7[ih] = fitsf->GetParError(1);
    sigma_err7[ih] = fitsf->GetParError(2);
    expsigmaf7[ih] =m_ns*expsigma;
  
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
c1.Print("many_plot_xy_step7.ps");

TCanvas* c2 = new TCanvas("c2"," Resolution vs Sumet from Topo (A0300)",293,139,700,500);
c2.Update();
c2->Divide(1,1);
c2.cd( 1 );

gStyle->SetLineColor( 2 );

//first
TGraphErrors* gr = new TGraphErrors(nbinplot++,nxbin,sigma,ex,sigma_err);
gr->SetTitle();

gr->SetMarkerColor(1);
gr->SetMarkerStyle(21);
//gr->Draw("ALP"); 
gr->Draw("AP"); 
gr->SetMaximum(m_curvasres);
gr->SetMinimum(0);
//gr->GetYaxis()->SetTitle("Resolution (GeV)");
gr->GetYaxis()->SetTitle("ExMiss,EyMiss (GeV)");
gr->GetXaxis()->SetTitle("#Sigma E_{T}  (GeV)");

//myText( 0.60,0.25,1,"minimum bias ");
myText( 0.20,0.85,1,"Z#rightarrow #tau#tau");

TF1 *f1 = new TF1("f1","[0]*sqrt(x)",0, m_curvasET);
f1->SetLineColor(1);
gr->Fit("f1","R");

//TLegend *leg = new TLegend(0.55, 0.75, 0.85, 0.95);
TLegend *leg = new TLegend(0.65, 0.25, 0.90, 0.50);
char message[50];
sprintf(message,"1 Refined: fit %3.2f #sqrt{Sumet} " , f1->GetParameter(0) );
leg->AddEntry(gr,message,"pl");

//second
TGraphErrors* gr = new TGraphErrors(nbinplot2++,nxbin,sigma2,ex2,sigma_err2);

gr->SetMarkerColor(2);
gr->SetMarkerStyle(21);
gr->Draw("P"); 

TF1 *f2 = new TF1("f2","[0]*sqrt(x)",0, m_curvasET);
f2->SetLineColor(2);
gr->Fit("f2","R");

sprintf(message,"2 Topo: fit %3.2f #sqrt{Sumet} " , f2->GetParameter(0) );
leg->AddEntry(gr,message,"pl");

//third
TGraphErrors* gr = new TGraphErrors(nbinplot3++,nxbin,sigma3,ex3,sigma_err3);

gr->SetMarkerColor(3);
gr->SetMarkerStyle(21);
gr->Draw("P"); 

TF1 *f3 = new TF1("f3","[0]*sqrt(x)",0, m_curvasET);
f3->SetLineColor(3);
gr->Fit("f3","R");

sprintf(message,"3 TopoCorr: fit %3.2f #sqrt{Sumet} " , f3->GetParameter(0) );
leg->AddEntry(gr,message,"pl");

//fourth
TGraphErrors* gr = new TGraphErrors(nbinplot4++,nxbin,sigma4,ex4,sigma_err4);

gr->SetMarkerColor(4);
gr->SetMarkerStyle(21);
gr->Draw("P"); 

TF1 *f4 = new TF1("f4","[0]*sqrt(x)",0, m_curvasET);
f4->SetLineColor(4);
gr->Fit("f4","R");

sprintf(message,"4 Final: fit %3.2f #sqrt{Sumet} " , f4->GetParameter(0) );
leg->AddEntry(gr,message,"pl");

//fifth
TGraphErrors* gr = new TGraphErrors(nbinplot5++,nxbin,sigma5,ex5,sigma_err5);

gr->SetMarkerColor(5);
gr->SetMarkerStyle(21);
gr->Draw("P"); 

TF1 *f5 = new TF1("f5","[0]*sqrt(x)",0, m_curvasET);
f5->SetLineColor(5);
gr->Fit("f5","R");

sprintf(message,"5 LocHad: fit %3.2f #sqrt{Sumet} " , f5->GetParameter(0) );
leg->AddEntry(gr,message,"pl");

//sixth
TGraphErrors* gr = new TGraphErrors(nbinplot6++,nxbin,sigma6,ex6,sigma_err6);

gr->SetMarkerColor(6);
gr->SetMarkerStyle(21);
gr->Draw("P"); 

TF1 *f6 = new TF1("f6","[0]*sqrt(x)",0, m_curvasET);
f6->SetLineColor(6);
//gr->Fit("f6","R");

sprintf(message,"6 Base: fit %3.2f #sqrt{Sumet} " , f6->GetParameter(0) );
leg->AddEntry(gr,message,"pl");

//seventh
TGraphErrors* gr = new TGraphErrors(nbinplot7++,nxbin,sigma7,ex7,sigma_err7);

gr->SetMarkerColor(7);
gr->SetMarkerStyle(21);
gr->Draw("P"); 

TF1 *f7 = new TF1("f7","[0]*sqrt(x)",0, m_curvasET);
f7->SetLineColor(7);
//gr->Fit("f7","R");

sprintf(message,"7 Calib: fit %3.2f #sqrt{Sumet} " , f7->GetParameter(0) );
leg->AddEntry(gr,message,"pl");

c2.Update();
leg->Draw();
//end Graph

//  ATLAS_LABEL(0.8,0.2);

//c2.Print("curva_wenu_xy_steps.eps");
//c2.Print("curva_minbias_100_xy_steps.eps");
//c2.Print("curva_minbias_100_useTruth0_xy_steps.eps");
c2.Print("curva_ztautau_steps.eps");
}
//***************************************

}



