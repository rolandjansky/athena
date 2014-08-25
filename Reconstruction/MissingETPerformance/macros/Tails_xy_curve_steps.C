/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
///////////////////////////////////////////////////////////////////////////////////
// histos for Etmiss Resol vs Sumet  
// take from the resolution plots in Sumet bins the
// fit parameters, plot and fit them to make the curve 

// Open the root file

//TFile file("../METPerformance_wenu.root");
TFile file("../METPerformance_j4_100k_sumet4.root");
//TFile file("../METPerformance_ztautau.root");
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
int m_entries=500;

int m_numptt;
int m_binpt;
int m_startpt;

//ATTENTION!! set m_numptt, m_binpt, m_startpt
//     according to ResolutionTool_jobOptions.py in MissingETPerformance!!
//set also m_curvasres good for Tails plot....

m_numptt=40;
m_startpt=5;

//wenu,ztautau
//m_binpt=10;
//m_curvasres=5.;
//j4
m_binpt=20.;
m_curvasres=20.;

m_curvasET=m_numptt*m_binpt;

//tails...................
float prova1;
float prova2;
double tail[m_numptt];
double tail2[m_numptt];
double tail3[m_numptt];
double tail4[m_numptt];
double tail5[m_numptt];
double tail6[m_numptt];
double tail7[m_numptt];
double ertail[m_numptt];
double ertail2[m_numptt];
double ertail3[m_numptt];
double ertail4[m_numptt];
double ertail5[m_numptt];
double ertail6[m_numptt];
double ertail7[m_numptt];
double Htail[m_numptt];
double Htail2[m_numptt];
double Htail3[m_numptt];
double Htail4[m_numptt];
double Htail5[m_numptt];
double Htail6[m_numptt];
double Htail7[m_numptt];
for (int ita= 0; ita< m_numptt ; ita++){
tail[ita]=0.;
tail2[ita]=0.;
tail3[ita]=0.;
tail4[ita]=0.;
tail5[ita]=0.;
tail6[ita]=0.;
tail7[ita]=0.;
}
int nbinstart;
int nbinend;
//include underflow/overflow
nbinstart=0;
nbinend=102;
//NO underflow/overflow
//   nbinstart=1;
//   nbinend=101;

//tails...................
  
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

    //tails..................
    prova1=mean[ih]+3.*sigma[ih];
    prova2=mean[ih]-3.*sigma[ih];
//          std::cout  << " ih "<<ih<< "mean[ih] "<<mean[ih] <<
//" sigma[ih] "<<sigma[ih]<< "prova1 "<<prova1 <<std::endl; 
    for (int ibinss= nbinstart; ibinss< nbinend ; ibinss++){ 
//     centro=hist.GetBinCenter(ibinss);
      centro=hist.GetBinLowEdge(ibinss);
      //      std::cout << "centro"<<centro <<std::endl; 
      if(centro > prova1 || centro < prova2) {
	tail[ih]= tail[ih]+hist.GetBinContent(ibinss);
      }
    }
    tail[ih]=tail[ih]*100./entries;
    ertail[ih]=sqrt(tail[ih])*100./entries;
    //          ertail[ih]=0.01;
    std::cout << "tail[ih]"<<tail[ih] << " ertail[ih]"<<ertail[ih] <<std::endl; 
    Htail[ih]= hist.GetBinContent(nbinstart)+hist.GetBinContent(nbinend-1);   
      
    //tails..................

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
  
    //tails..................
    prova1=mean2[ih]+3.*sigma2[ih];
    prova2=mean2[ih]-3.*sigma2[ih];
    //          std::cout  << " ih "<<ih<< "mean2[ih] "<<mean2[ih] <<
    //" sigma2[ih] "<<sigma2[ih]<< "prova1 "<<prova1 <<std::endl; 
    for (int ibinss= nbinstart; ibinss< nbinend ; ibinss++){ 
      centro=hist.GetBinLowEdge(ibinss);
      //          std::cout << "centro"<<centro <<std::endl; 
      if(centro > prova1 || centro < prova2) {
	tail2[ih]= tail2[ih]+hist.GetBinContent(ibinss);
      }
    }
    tail2[ih]=tail2[ih]*100./entries;
    ertail2[ih]=sqrt(tail2[ih])*100./entries;
    //          ertail2[ih]=0.01;
    //          std::cout << "tail2[ih]"<<tail2[ih] << " ertail2[ih]"<<ertail2[ih] <<std::endl; 
    Htail2[ih]= hist.GetBinContent(nbinstart)+hist.GetBinContent(nbinend-1);   
            
    //tails..................

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
  
    //tails..................
    prova1=mean3[ih]+3.*sigma3[ih];
    prova2=mean3[ih]-3.*sigma3[ih];
    for (int ibinss= nbinstart; ibinss< nbinend ; ibinss++){ 
      centro=hist.GetBinLowEdge(ibinss);
      if(centro > prova1 || centro < prova2) {
	tail3[ih]= tail3[ih]+hist.GetBinContent(ibinss);
      }
    }
    tail3[ih]=tail3[ih]*100./entries;
    ertail3[ih]=sqrt(tail3[ih])*100./entries;
    //          ertail3[ih]=0.01;
    //          std::cout << "tail3[ih]"<<tail3[ih] << " ertail3[ih]"<<ertail3[ih] <<std::endl; 
    Htail3[ih]= hist.GetBinContent(nbinstart)+hist.GetBinContent(nbinend-1);   
    //tails..................
            
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
    //tails..................
    prova1=mean4[ih]+3.*sigma4[ih];
    prova2=mean4[ih]-3.*sigma4[ih];
    for (int ibinss= nbinstart; ibinss< nbinend ; ibinss++){ 
      centro=hist.GetBinLowEdge(ibinss);
      if(centro > prova1 || centro < prova2) {
	tail4[ih]= tail4[ih]+hist.GetBinContent(ibinss);
      }
    }
    tail4[ih]=tail4[ih]*100./entries;
    ertail4[ih]=sqrt(tail4[ih])*100./entries;
    //          ertail4[ih]=0.01;
    //          std::cout << "tail4[ih]"<<tail4[ih] << " ertail4[ih]"<<ertail4[ih] <<std::endl; 
    Htail4[ih]= hist.GetBinContent(nbinstart)+hist.GetBinContent(nbinend-1);   
            
    //tails..................
  
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
  
    //tails..................
    prova1=mean5[ih]+3.*sigma5[ih];
    prova2=mean5[ih]-3.*sigma5[ih];
    for (int ibinss= nbinstart; ibinss< nbinend ; ibinss++){ 
      centro=hist.GetBinLowEdge(ibinss);
      if(centro > prova1 || centro < prova2) {
	tail5[ih]= tail5[ih]+hist.GetBinContent(ibinss);
      }
    }
    tail5[ih]=tail5[ih]*100./entries;
    ertail5[ih]=sqrt(tail5[ih])*100./entries;
    //          ertail5[ih]=0.01;
    //          std::cout << "tail5[ih]"<<tail5[ih] << " ertail5[ih]"<<ertail5[ih] <<std::endl; 
    Htail5[ih]= hist.GetBinContent(nbinstart)+hist.GetBinContent(nbinend-1);   
            
    //tails..................

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
    //tails..................
    prova1=mean6[ih]+3.*sigma6[ih];
    prova2=mean6[ih]-3.*sigma6[ih];
    for (int ibinss= nbinstart; ibinss< nbinend ; ibinss++){ 
      centro=hist.GetBinLowEdge(ibinss);
      if(centro > prova1 || centro < prova2) {
	tail6[ih]= tail6[ih]+hist.GetBinContent(ibinss);
      }
    }
    tail6[ih]=tail6[ih]*100./entries;
    ertail6[ih]=sqrt(tail6[ih])*100./entries;
    //          ertail6[ih]=0.01;
    //          std::cout << "tail6[ih]"<<tail6[ih] << " ertail6[ih]"<<ertail6[ih] <<std::endl; 
    Htail6[ih]= hist.GetBinContent(nbinstart)+hist.GetBinContent(nbinend-1);   

    //tails.................. 
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

    //tails..................
    prova1=mean7[ih]+3.*sigma7[ih];
    prova2=mean7[ih]-3.*sigma7[ih];
    std::cout << "mean "<<mean7[ih]<< " sigma "<<sigma7[ih]<<" prova1 "
	      <<prova1<<" prova2 "<<prova2<<std::endl; 
    for (int ibinss= nbinstart; ibinss< nbinend ; ibinss++){ 
      prova7=hist.GetBinContent(ibinss);
      centro=hist.GetBinLowEdge(ibinss);
      std::cout << "ibinss "<<ibinss << " cont "<<prova7<<" lowedge "<<centro<<std::endl; 
      if(centro > prova1 || centro < prova2) {
	std::cout << "ibinssOk "<<ibinss << " cont "<<prova7<<" lowedge "<<centro<<std::endl; 
	tail7[ih]= tail7[ih]+hist.GetBinContent(ibinss);
      }
    }
    tail7[ih]=tail7[ih]*100./entries;
    ertail7[ih]=sqrt(tail7[ih])*100/entries;
    //          ertail7[ih]=0.01;
    std::cout << "tail7[ih]"<<tail7[ih] << " ertail7[ih]"<<ertail7[ih] <<std::endl; 
    Htail7[ih]= hist.GetBinContent(nbinstart)+hist.GetBinContent(nbinend-1);   
    //tails..................

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
//TGraphErrors* gr = new TGraphErrors(nbinplot++,nxbin,sigma,ex,sigma_err);
TGraphErrors* gr = new TGraphErrors(nbinplot++,nxbin,tail,ex,ertail);
gr->SetTitle();

gr->SetMarkerColor(1);
gr->SetMarkerStyle(21);
//gr->Draw("ALP"); 
gr->Draw("AP"); 
gr->SetMaximum(m_curvasres);
gr->SetMinimum(0);
//gr->GetYaxis()->SetTitle("Resolution (GeV)");
gr->GetYaxis()->SetTitle("Tails % (GeV)");
gr->GetXaxis()->SetTitle("#Sigma E_{T} (GeV)");

//myText( 0.60,0.25,1,"minimum bias ");
//myText( 0.60,0.85,1,"Z#rightarrow #tau#tau");
//myText( 0.60,0.85,1,"W#rightarrow e#nu");
myText( 0.60,0.85,1,"j4");

//TF1 *f1 = new TF1("f1","[0]*sqrt(x)",0, m_curvasET);
//f1->SetLineColor(1);
//gr->Fit("f1","R");

//TLegend *leg = new TLegend(0.55, 0.75, 0.85, 0.95);
//TLegend *leg = new TLegend(0.65, 0.25, 0.90, 0.50);
TLegend *leg = new TLegend(0.70, 0.65, 0.90, 0.95);
char message[50];
sprintf(message,"1 Refined ");
leg->AddEntry(gr,message,"pl");


//second
TGraphErrors* gr = new TGraphErrors(nbinplot2++,nxbin,tail2,ex2,ertail2);

gr->SetMarkerColor(2);
gr->SetMarkerStyle(21);
gr->Draw("P"); 

sprintf(message,"2 Topo");
leg->AddEntry(gr,message,"pl");

//third
TGraphErrors* gr = new TGraphErrors(nbinplot3++,nxbin,tail3,ex3,ertail3);

gr->SetMarkerColor(3);
gr->SetMarkerStyle(21);
gr->Draw("P"); 

sprintf(message,"3 TopoCorr" );
leg->AddEntry(gr,message,"pl");

//fourth
TGraphErrors* gr = new TGraphErrors(nbinplot4++,nxbin,tail4,ex4,ertail4);

gr->SetMarkerColor(4);
gr->SetMarkerStyle(21);
gr->Draw("P"); 

sprintf(message,"4 Final" );
leg->AddEntry(gr,message,"pl");

//fifth
TGraphErrors* gr = new TGraphErrors(nbinplot5++,nxbin,tail5,ex5,ertail5);

gr->SetMarkerColor(5);
gr->SetMarkerStyle(21);
gr->Draw("P"); 

sprintf(message,"5 LocHad" );
leg->AddEntry(gr,message,"pl");

//sixth
TGraphErrors* gr = new TGraphErrors(nbinplot6++,nxbin,tail6,ex6,ertail6);

gr->SetMarkerColor(6);
gr->SetMarkerStyle(21);
gr->Draw("P"); 

sprintf(message,"6 Base" );
leg->AddEntry(gr,message,"pl");

//seventh
TGraphErrors* gr = new TGraphErrors(nbinplot7++,nxbin,tail7,ex7,ertail7);

gr->SetMarkerColor(7);
gr->SetMarkerStyle(21);
gr->Draw("P"); 

sprintf(message,"7 Calib" );
leg->AddEntry(gr,message,"pl");

c2.Update();
leg->Draw();
//end Graph

//  ATLAS_LABEL(0.8,0.2);

//c2.Print("Tails_wenu_xy_steps.eps");
//c2.Print("Tails_minbias_100_xy_steps.eps");
//c2.Print("Tails_minbias_100_useTruth0_xy_steps.eps");
//c2.Print("Tails_j4_steps.eps");
c2.Print("Tails_ztautau_steps.eps");
}
////***************************************

}
