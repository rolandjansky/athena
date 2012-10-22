/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author M. Bellomo <massimiliano.bellomo@pv.infn,it>
//
// DO_method.C
//
// ROOT macro to fit Z to mumu invariant mass reconstructed
// with StandAlone/Vertex/Combined ATLAS MuonSpectrometer
// reconstruction mode and extract trigger efficiency from data
// This technique is explained in detail in
// http://www.pv.infn.it/~bellomo/archivio/phd_thesis/ATLAS_muHLT_lumi_ZWbosons_at_LHC.pdf

#include <iostream>
#include <string.h>
#include "TROOT.h"
#include "TF1.h"
#include "TH1F.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TFile.h"

using namespace std;

///////////////////////
// Double Gaussian
// mean    --> par[0]
// sigma 1 --> par[1]
// sigma 2 --> par[2]
//
Double_t DoubleGaus(Double_t x, Double_t mean, Double_t sigma1, Double_t sigma2)
{
  return TMath::Gaus(x,mean,sigma1) + TMath::Gaus(x,mean,sigma2);
}

///////////////////////
// Triple Gaussian
// mean    --> par[0]
// sigma 1 --> par[1]
// sigma 2 --> par[2]
// sigma 3 --> par[3]
//
Double_t TripleGaus(Double_t x, Double_t mean, Double_t sigma1, Double_t sigma2, Double_t sigma3)
{
  return TMath::Gaus(x,mean,sigma1) + TMath::Gaus(x,mean,sigma2)  + TMath::Gaus(x,mean,sigma3);
}

//////////////////////////////////////
// Breit-Wigner Gaussian convolution
//
Double_t BreitGaus(Double_t *x, Double_t *par) 
{
  //printf("BreitGaus input parameter %f %f %f \n",par[0],par[1],par[2]);
  //Fit parameters:
  //par[0]=Width (scale) Breit-Wigner
  //par[1]=Most Probable (MP, location) Breit mean
  //par[2]=Width (sigma) of convoluted Gaussian function
  //
  //
  //In the Landau distribution (represented by the CERNLIB approximation),
  //the maximum is located at x=-0.22278298 with the location parameter=0.
  //This shift is corrected within this function, so that the actual
  //maximum is identical to the MP parameter.
 
  // Numeric constants
  Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
  //Double_t twoPi = 6.2831853071795;//2Pi
 
  // Control constants
  Double_t np = 100.0;      // number of convolution steps
  Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas
 
  // Variables
  Double_t xx;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow,xupp;
  Double_t step;
  Double_t i;
  
  // Range of convolution integral
  xlow = x[0] - sc * par[2];
  xupp = x[0] + sc * par[2];
 
  step = (xupp-xlow) / np;
 
  // Convolution integral of Breit and Gaussian by sum
  for(i=1.0; i<=np/2; i++) {
    xx = xlow + (i-.5) * step;
    fland = TMath::BreitWigner(xx,par[1],par[0]);
    sum += fland * TMath::Gaus(x[0],xx,par[2]);
 
    xx = xupp - (i-.5) * step;
    fland = TMath::BreitWigner(xx,par[1],par[0]);
    sum += fland * TMath::Gaus(x[0],xx,par[2]);
  } 
  return (step * sum * invsq2pi / par[2]);
}

//////////////////////////////////////
// Breit-Wigner DoubleGaussian convolution
//
Double_t BreitDoubleGaus(Double_t *x, Double_t *par) 
{
  //printf("BreitDoubleGaus input parameter %f %f %f $f\n",par[0],par[1],par[2],par[3]);
  //Fit parameters:
  //par[0]=Width (scale) Breit-Wigner
  //par[1]=Most Probable (MP, location) Breit mean
  //par[2]=Width 1(sigma) of convoluted Double Gaussian function
  //par[3]=Width 2(sigma) of convoluted Double Gaussian function
  //
  //
  //In the Landau distribution (represented by the CERNLIB approximation),
  //the maximum is located at x=-0.22278298 with the location parameter=0.
  //This shift is corrected within this function, so that the actual
  //maximum is identical to the MP parameter.
 
  // Numeric constants
  Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
  //Double_t twoPi = 6.2831853071795;//2Pi
 
  // Control constants
  Double_t np = 100.0;      // number of convolution steps
  Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas
 
  // Variables
  Double_t xx;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow,xupp;
  Double_t step;
  Double_t i;
  
  Double_t total_width = sqrt(par[2]*par[2] + par[3]*par[3]);

  // Range of convolution integral
  xlow = x[0] - sc * total_width;
  xupp = x[0] + sc * total_width;
 
  step = (xupp-xlow) / np;
 
  // Convolution integral of Breit and Gaussian by sum
  for(i=1.0; i<=np/2; i++) {
    xx = xlow + (i-.5) * step;
    fland = TMath::BreitWigner(xx,par[1],par[0]);
    sum += fland * DoubleGaus(x[0],xx,par[2],par[3]);
 
    xx = xupp - (i-.5) * step;
    fland = TMath::BreitWigner(xx,par[1],par[0]);
    sum += fland * DoubleGaus(x[0],xx,par[2],par[3]);
  } 
  return (step * sum * invsq2pi / total_width);
}

//////////////////////////////////////
// Breit-Wigner TripleGaussian convolution
//
Double_t BreitTripleGaus(Double_t *x, Double_t *par) 
{
  //printf("BreitDoubleGaus input parameter %f %f %f $f\n",par[0],par[1],par[2],par[3]);
  //Fit parameters:
  //par[0]=Width (scale) Breit-Wigner
  //par[1]=Most Probable (MP, location) Breit mean
  //par[2]=Width 1(sigma) of convoluted Double Gaussian function
  //par[3]=Width 2(sigma) of convoluted Double Gaussian function
  //
  //
  //In the Landau distribution (represented by the CERNLIB approximation),
  //the maximum is located at x=-0.22278298 with the location parameter=0.
  //This shift is corrected within this function, so that the actual
  //maximum is identical to the MP parameter.
 
  // Numeric constants
  Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
  //Double_t twoPi = 6.2831853071795;//2Pi
 
  // Control constants
  Double_t np = 100.0;      // number of convolution steps
  Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas
 
  // Variables
  Double_t xx;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow,xupp;
  Double_t step;
  Double_t i;
  
  Double_t total_width = sqrt(par[2]*par[2] + par[3]*par[3] + par[4]*par[4]);

  // Range of convolution integral
  xlow = x[0] - sc * total_width;
  xupp = x[0] + sc * total_width;
 
  step = (xupp-xlow) / np;
 
  // Convolution integral of Breit and Gaussian by sum
  for(i=1.0; i<=np/2; i++) {
    xx = xlow + (i-.5) * step;
    fland = TMath::BreitWigner(xx,par[1],par[0]);
    sum += fland * TripleGaus(x[0],xx,par[2],par[3],par[4]);
 
    xx = xupp - (i-.5) * step;
    fland = TMath::BreitWigner(xx,par[1],par[0]);
    sum += fland * TripleGaus(x[0],xx,par[2],par[3],par[4]);
  } 
  return (step * sum * invsq2pi / total_width);
}

///////////////////////////
// exponential background
//
Double_t exponential(Double_t *x, Double_t *par) 
{
  //printf("exp input parameter %f %f \n",par[0],par[1]);
  Double_t xlow = 30000.;
  Double_t xmax = 120000.;
  Double_t N = (TMath::Exp(par[0] + par[1]*xmax) - TMath::Exp(par[0] + par[1]*xlow))/par[1];
  if(N == 0) 
    return 0;
  else
    return (1/N)*TMath::Exp(par[0] + par[1]*x[0]);
}

/////////////////////////////////////////////////////////
// fit function = Breit-wigner x Gaussian + exponential
//
Double_t fitFunctionSB(Double_t *x, Double_t *par) 
{
  //printf("fitFunction input parameter %f %f\n",par[0],par[1]);
  //return par[0]*(breitgausfun(x,&par[3]));
  //return par[0]*(exponential(x,&par[1]));
  return par[0]*exponential(x,&par[2]) + par[1]*BreitGaus(x,&par[4]);
}

/////////////////////////////////////////////////////////
// fit function = Breit-wigner x DoubleGaussian + exponential
//
Double_t fitFunctionSB2(Double_t *x, Double_t *par) 
{
  return par[0]*exponential(x,&par[2]) + par[1]*BreitDoubleGaus(x,&par[4]);
}

/////////////////////////////////////////////////////////
// fit function = Breit-wigner x Gaussian
//
Double_t fitFunctionS(Double_t *x, Double_t *par) {return par[0]*(BreitGaus(x,&par[1]));}

/////////////////////////////////////////////////////////
// fit function = Breit-wigner x DoubleGaussian 
//
Double_t fitFunctionS2(Double_t *x, Double_t *par) {return par[0]*(BreitDoubleGaus(x,&par[1]));}

/////////////////////////////////////////////////////////
// fit function = Breit-wigner x TripleGaussian
//
Double_t fitFunctionS3(Double_t *x, Double_t *par) {return par[0]*(BreitTripleGaus(x,&par[1]));}

/////////////////////////////////////////////////////////
// Invariant mass spectrum fitter
//
void fitSpectrum(Bool_t signal_only, TH1F *h, Double_t *yields)
{

  TF1* fitFCN;
  Double_t* par;
  Double_t parS[6];
  Double_t parSB[9];

  ///// SIGNAL ONLY
  // Breit-Wigner x Gaussian convolution
  parS[0] = 100000.;    // S 
  parS[1] = 2500;
  parS[2] = 90000.;   
  parS[3]=  100.;  // sigma 1
  parS[4]=  2000.;  // sigma 2
  parS[5]=  1000.;  // sigma 3

  ///// SIGNAL + BACKGROUND
  // Exponential
  parSB[0] = 10000.;    // B 
  parSB[1] = 10000.;   // S 
  parSB[2] = 1.;
  parSB[3] = -0.00001;
  // Breit-Wigner x Gaussian convolution
  parSB[4] = 2500.;
  parSB[5] = 90000.;   
  parSB[6]=  100.;  // sigma 1
  parSB[7]=  2000.; // sigma 2 
  parSB[8]=  2000.; // sigma 3
  
  Double_t bin_width = h->GetBinWidth(1);
  Int_t index_mean, index_gamma, index_sigma1, index_sigma2, index_sigma3, index_signal;

  if(signal_only) {
    index_mean = 2;
    index_gamma = 1;
    index_sigma1 = 3;
    index_sigma2 = 4; 
    index_sigma3 = 5; 
    index_signal = 0;
    //fitFCN = new TF1("fitFCN",fitFunctionS, 30000, 120000 ,4);
    fitFCN = new TF1("fitFCN",fitFunctionS2, 30000, 120000 ,5);
    //fitFCN = new TF1("fitFCN",fitFunctionS3, 30000, 120000 ,6);
    par = parS;
    printf("input parameter %f %f %f %f %f %f\n",par[0],par[1],par[2],par[3],par[4],par[5]);
  }else{
    index_mean = 5;
    index_gamma = 4;
    index_sigma1 = 6;
    index_sigma2 = 7;
    index_sigma3 = 8;
    index_signal = 1;
    fitFCN = new TF1("fitFCN",fitFunctionSB2, 30000, 120000 ,8);
    par = parSB;
    printf("input parameter %f %f %f %f %f %f %f %f %f\n",par[0],par[1],par[2],par[3],par[4],par[5],par[6],par[7],par[8]);
  }
   // set initial parameters
  fitFCN->SetParameters(par);	

  // add simulat exp background
  TF1 *bkg = new TF1("bkg",exponential,0,120000,2);
  bkg->SetParameters(1.234,-0.00009657);
  if(!signal_only) h->FillRandom("bkg",50000);
  
  // set histogrma range and fit first time
  h->SetAxisRange(60000,120000,"X");
  h->Fit(fitFCN,"VN","",60000.,120000.);	

  // get fit parameters
  Double_t *ParFit;
  ParFit = fitFCN->GetParameters();
  
  // refit using previous paramaneters and restricted fit range in mean +/- 3*signal_width
  fitFCN->SetParameters(ParFit);	

  // build fit range from data
  Double_t signal_width = sqrt(ParFit[index_gamma]*ParFit[index_gamma] 
			       + ParFit[index_sigma1]*ParFit[index_sigma1]
			       + ParFit[index_sigma2]*ParFit[index_sigma2]);
			       //+ ParFit[index_sigma3]*ParFit[index_sigma3]);

  Double_t Lfit = ParFit[index_mean] - 5*signal_width;
  Double_t Rfit = ParFit[index_mean] + 5*signal_width;

  // print 
  cout << "fit with:" << endl;
  cout << "- mean = " << ParFit[index_mean] << endl;
  cout << "- fit range = " << Lfit << " , " << Rfit << endl;

  // refit
  fitFCN->SetLineColor(2);
  h->Fit(fitFCN,"V","pe1",Lfit,Rfit);

  // get final parameters (yields)
  ParFit = fitFCN->GetParameters();
  yields[0] = ParFit[index_signal]/bin_width;
  yields[1] = ParFit[0]/bin_width;

}

/////////////////////////////////////////////////////////
// Main program
//
int main(int argc, char* argv[])
{
  
  if(argc<2 || !strncmp(argv[1],"-h",2)) {
    cout << "Usage : DO_method <rootfile> OPT" << endl;
    cout << " OPT    value       variable    default" << endl;
    cout << "        true/false  signal_only true" << endl; 
    exit(1);
  }
  
  Bool_t signal_only=true;
  
  if(argc>2  && !strncmp(argv[2],"false",5)) {
    cout << "Background analysis active " << endl;
    signal_only=false;
  }
    
  //Double_t XSec_Zmumu = 1497;
  //Double_t XSec_Ztautau = 77;
  //Double_t XSec_BBmumu = 6525;
  //Double_t XSec_Wmunu = 11946;
  
  TFile *file_Zmumu = new TFile(argv[1]);

  TH1F *Cut_Efficiency_Zmumu = (TH1F*)file_Zmumu->Get("ZmumuAnalysis/Cut_Efficiency");

  //

  TH1F *DO_MuonSpec_DiMuonMassN1_Zmumu_MS = (TH1F*)file_Zmumu->Get("InsituPerformance/DO_Histograms/DO_MuonSpec_DiMuonMassN1_Trigger_Level_3_MS");
  TH1F *DO_MuonSpec_DiMuonMassN2_Zmumu_MS = (TH1F*)file_Zmumu->Get("InsituPerformance/DO_Histograms/DO_MuonSpec_DiMuonMassN2_Trigger_Level_3_MS");

  TH1F *DO_MuonSpec_DiMuonMassN1_Zmumu_MV = (TH1F*)file_Zmumu->Get("InsituPerformance/DO_Histograms/DO_MuonSpec_DiMuonMassN1_Trigger_Level_3_MV");
  TH1F *DO_MuonSpec_DiMuonMassN2_Zmumu_MV = (TH1F*)file_Zmumu->Get("InsituPerformance/DO_Histograms/DO_MuonSpec_DiMuonMassN2_Trigger_Level_3_MV");

  TH1F *DO_MuonSpec_DiMuonMassN1_Zmumu_CB = (TH1F*)file_Zmumu->Get("InsituPerformance/DO_Histograms/DO_MuonSpec_DiMuonMassN1_Trigger_Level_3_CB");
  TH1F *DO_MuonSpec_DiMuonMassN2_Zmumu_CB = (TH1F*)file_Zmumu->Get("InsituPerformance/DO_Histograms/DO_MuonSpec_DiMuonMassN2_Trigger_Level_3_CB"); 
 
  Double_t MuonCombN1_S =  DO_MuonSpec_DiMuonMassN1_Zmumu_MS->Integral();
  Double_t MuonCombN2_S =  DO_MuonSpec_DiMuonMassN2_Zmumu_MS->Integral();
  Double_t TriggerEff_MuonComb = 2*MuonCombN2_S/(MuonCombN1_S+MuonCombN2_S);
  Double_t dTriggerEff_MuonComb = sqrt(((2-TriggerEff_MuonComb)*(1-TriggerEff_MuonComb)*0.5)/Cut_Efficiency_Zmumu->GetBinContent(1));

  cout << "MS Trigger Efficiency = " << TriggerEff_MuonComb << " +/- " << dTriggerEff_MuonComb << endl;

  MuonCombN1_S =  DO_MuonSpec_DiMuonMassN1_Zmumu_MV->Integral(67,84);
  MuonCombN2_S =  DO_MuonSpec_DiMuonMassN2_Zmumu_MV->Integral(67,84);
  TriggerEff_MuonComb = 2*MuonCombN2_S/(MuonCombN1_S+MuonCombN2_S);
  dTriggerEff_MuonComb = sqrt(((2-TriggerEff_MuonComb)*(1-TriggerEff_MuonComb)*0.5)/Cut_Efficiency_Zmumu->GetBinContent(1));

  cout << "MV Trigger Efficiency = " << TriggerEff_MuonComb << " +/- " << dTriggerEff_MuonComb << endl;

  MuonCombN1_S =  DO_MuonSpec_DiMuonMassN1_Zmumu_CB->Integral(67,84);
  MuonCombN2_S =  DO_MuonSpec_DiMuonMassN2_Zmumu_CB->Integral(67,84);
  TriggerEff_MuonComb = 2*MuonCombN2_S/(MuonCombN1_S+MuonCombN2_S);
  dTriggerEff_MuonComb = sqrt(((2-TriggerEff_MuonComb)*(1-TriggerEff_MuonComb)*0.5)/Cut_Efficiency_Zmumu->GetBinContent(1));

  cout << "CB Trigger Efficiency = " << TriggerEff_MuonComb << " +/- " << dTriggerEff_MuonComb << endl;

  TCanvas *cdo = new TCanvas("cdo","DoubleObject Results MS",400,800);
  cdo->Divide(1,2);  
 
  cout << "**********************************************" << endl;
  cout << "*--------------- DO method ------------------*" << endl;
  cout << "* fitting control sample                     *" << endl;
  cout << "* MuonSpectrometer standalone reconstruction *" << endl;
  cout << "**********************************************" << endl; 
 
  cdo->cd(1);
  Double_t yields[2];
  fitSpectrum(signal_only, DO_MuonSpec_DiMuonMassN1_Zmumu_MS, yields);
  Double_t MuonSpecN1MS_S = yields[0];
  Double_t MuonSpecN1MS_B = yields[1];  
 

  cout << "**********************************************" << endl;
  cout << "*--------------- DO method ------------------*" << endl;
  cout << "* fitting diagnostic sample                  *" << endl;
  cout << "* MuonSpectrometer standalone reconstruction *" << endl;
  cout << "**********************************************" << endl; 

  cdo->cd(2);
  fitSpectrum(signal_only, DO_MuonSpec_DiMuonMassN2_Zmumu_MS, yields);
  Double_t MuonSpecN2MS_S = yields[0];
  Double_t MuonSpecN2MS_B = yields[1];  

  //
  //
  // 

  TCanvas *cdo1 = new TCanvas("cdo1","DoubleObject Results MV",400,800);
  cdo1->Divide(1,2);  
 
  cout << "**********************************************" << endl;
  cout << "*--------------- DO method ------------------*" << endl;
  cout << "* fitting control sample                     *" << endl;
  cout << "* MuonSpectrometer vertex reconstruction     *" << endl;
  cout << "**********************************************" << endl; 
 
  cdo1->cd(1);
  fitSpectrum(signal_only, DO_MuonSpec_DiMuonMassN1_Zmumu_MV, yields);
  Double_t MuonSpecN1MV_S = yields[0];
  Double_t MuonSpecN1MV_B = yields[1];  
 

  cout << "**********************************************" << endl;
  cout << "*--------------- DO method ------------------*" << endl;
  cout << "* fitting diagnostic sample                  *" << endl;
  cout << "* MuonSpectrometer vertex reconstruction     *" << endl;
  cout << "**********************************************" << endl; 

  cdo1->cd(2);
  fitSpectrum(signal_only, DO_MuonSpec_DiMuonMassN2_Zmumu_MV, yields);
  Double_t MuonSpecN2MV_S = yields[0];
  Double_t MuonSpecN2MV_B = yields[1];  

  //
  //
  //

  TCanvas *cdo2 = new TCanvas("cdo2","DoubleObject Results CB",400,800);
  cdo2->Divide(1,2);  
 
  cout << "**********************************************" << endl;
  cout << "*--------------- DO method ------------------*" << endl;
  cout << "* fitting control sample                     *" << endl;
  cout << "* MuonSpectrometer combined reconstruction   *" << endl;
  cout << "**********************************************" << endl; 
 
  cdo2->cd(1);
  fitSpectrum(signal_only, DO_MuonSpec_DiMuonMassN1_Zmumu_CB, yields);
  Double_t MuonSpecN1CB_S = yields[0];
  Double_t MuonSpecN1CB_B = yields[1];  
 

  cout << "**********************************************" << endl;
  cout << "*--------------- DO method ------------------*" << endl;
  cout << "* fitting diagnostic sample                  *" << endl;
  cout << "* MuonSpectrometer combined reconstruction *" << endl;
  cout << "**********************************************" << endl; 

  cdo2->cd(2);
  fitSpectrum(signal_only, DO_MuonSpec_DiMuonMassN2_Zmumu_CB, yields);
  Double_t MuonSpecN2CB_S = yields[0];
  Double_t MuonSpecN2CB_B = yields[1];  

  //
  //
  // save fit to root file

  TFile *out = new TFile("DO_method.root","RECREATE");
  cdo->Write();
  cdo1->Write();
  cdo2->Write();
  out->Close();

  cout << "**********************************************" << endl;
  cout << "*--------------- DO method ------------------*" << endl;
  cout << "* fitting diagnostic sample                  *" << endl;
  cout << "* MuonSpectrometer standalone reconstruction *" << endl;
  cout << "**********************************************" << endl; 

  cout << "Signal yields N1 = " << MuonSpecN1MS_S << endl;
  if(!signal_only) cout << "Backg. yields B1 = " << MuonSpecN1MS_B << endl;

  cout << "Signal yields N2 = " << MuonSpecN2MS_S << endl;
  if(!signal_only) cout << "Backg. yields B2 = " << MuonSpecN2MS_B << endl;

  cout << "**********************************************" << endl;
  cout << "*--------------- DO method ------------------*" << endl;
  cout << "* fitting diagnostic sample                  *" << endl;
  cout << "* MuonSpectrometer vertex reconstruction     *" << endl;
  cout << "**********************************************" << endl; 

  cout << "Signal yields N1 = " << MuonSpecN1MV_S << endl;
  if(!signal_only) cout << "Backg. yields B1 = " << MuonSpecN1MV_B << endl;

  cout << "Signal yields N2 = " << MuonSpecN2MV_S << endl;
  if(!signal_only) cout << "Backg. yields B2 = " << MuonSpecN2MV_B << endl;

  cout << "**********************************************" << endl;
  cout << "*--------------- DO method ------------------*" << endl;
  cout << "* fitting diagnostic sample                  *" << endl;
  cout << "* MuonSpectrometer combined reconstruction *" << endl;
  cout << "**********************************************" << endl; 

  cout << "Signal yields N1 = " << MuonSpecN1CB_S << endl;
  if(!signal_only) cout << "Backg. yields B1 = " << MuonSpecN1CB_B << endl;

  cout << "Signal yields N2 = " << MuonSpecN2CB_S << endl;
  if(!signal_only) cout << "Backg. yields B2 = " << MuonSpecN2CB_B << endl;

  cout << "**********************************************" << endl;
  cout << "*--------------- DO method ------------------*" << endl;
  cout << "*                 Results                    *" << endl;
  cout << "**********************************************" << endl; 

  Double_t TriggerEff_MuonSpecMS = 2*MuonSpecN2MS_S/(MuonSpecN1MS_S+MuonSpecN2MS_S);
  Double_t dTriggerEff_MuonSpecMS = sqrt(((2-TriggerEff_MuonSpecMS)*(1-TriggerEff_MuonSpecMS)*0.5)/Cut_Efficiency_Zmumu->GetBinContent(1));

  Double_t TriggerEff_MuonSpecMV = 2*MuonSpecN2MV_S/(MuonSpecN1MV_S+MuonSpecN2MV_S);
  Double_t dTriggerEff_MuonSpecMV = sqrt(((2-TriggerEff_MuonSpecMV)*(1-TriggerEff_MuonSpecMV)*0.5)/Cut_Efficiency_Zmumu->GetBinContent(1));

  Double_t TriggerEff_MuonSpecCB = 2*MuonSpecN2CB_S/(MuonSpecN1CB_S+MuonSpecN2CB_S);
  Double_t dTriggerEff_MuonSpecCB = sqrt(((2-TriggerEff_MuonSpecCB)*(1-TriggerEff_MuonSpecCB)*0.5)/Cut_Efficiency_Zmumu->GetBinContent(1));

  cout << "Trigger efficiency MS = " << TriggerEff_MuonSpecMS << " +/- " << dTriggerEff_MuonSpecMS << endl;
  cout << "Trigger efficiency MV = " << TriggerEff_MuonSpecMV << " +/- " << dTriggerEff_MuonSpecMV << endl;
  cout << "Trigger efficiency CB = " << TriggerEff_MuonSpecCB << " +/- " << dTriggerEff_MuonSpecCB << endl;

  return 0;
}
