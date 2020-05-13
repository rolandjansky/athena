/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStandAloneExtraTools/TimeAndAdcFitter.h"
#include "TF1.h"

void TimeAndAdcFitter::doTimeFit(TH1F *h, const int nParams, double *pfit, double *errfit, double *pchi2, int *pndof ) {

  double minTime = 0.;
  double maxTime = 2000.;

  double chi2;
  int ndof;
      
  TF1 *TimeSpectrum = new TF1("TimeSpectrum",
			      "[0]+([1]*(1+[2]*exp(-(x-[4])/[3])))/((1+exp((-x+[4])/[6]))*(1+exp((x-[5])/[7]))) ",
			      minTime,maxTime); 
  TimeSpectrum->SetLineColor(3);
	 
  searchParams(h,&pfit[0],nParams);  

  TimeSpectrum->SetParameters(pfit);
  TimeSpectrum->SetParLimits(0,0.,100.);
  TimeSpectrum->SetParLimits(1,0.,10000.);
  TimeSpectrum->SetParLimits(2,0.,40.);
  TimeSpectrum->SetParLimits(3,50.,400.);
  TimeSpectrum->SetParLimits(4,0.,1500.);
  // 5 parameters fit
  TimeSpectrum->SetParLimits(5,pfit[5],pfit[5]);
  TimeSpectrum->SetParLimits(6,pfit[6],pfit[6]);
  TimeSpectrum->SetParLimits(7,pfit[7],pfit[7]);
  h->Fit("TimeSpectrum","QLB");
  // 6 parameters fit
  TimeSpectrum->SetParLimits(5,500.,2000.);
  TimeSpectrum->SetParLimits(6,pfit[6],pfit[6]);
  TimeSpectrum->SetParLimits(7,pfit[7],pfit[7]);
  h->Fit("TimeSpectrum","QLB");
  // 7 parameters fit
  TimeSpectrum->SetParLimits(6,4.,30.);
  h->Fit("TimeSpectrum","QLB");
  // final 8 parameters fit
  TimeSpectrum->SetParLimits(6,4.,30.);
  TimeSpectrum->SetParLimits(7,4.,30.);
  double xmin = h->GetBinLowEdge(1);
  double xmax = TimeSpectrum->GetParameter(5)+250.;
  h->Fit("TimeSpectrum","QLB","",xmin,xmax);

  for (int i=0; i<nParams; i++){
    pfit[i]    = TimeSpectrum->GetParameter(i);
    errfit[i]  = TimeSpectrum->GetParError(i);
  }
  chi2     = TimeSpectrum->GetChisquare(); // total chi2
  ndof     = TimeSpectrum->GetNDF();        // number of degrees of freedom
	
  *pchi2 = chi2;
  *pndof = ndof;
  return;
}  //end TimeAndAdcFitter::doTimeFit

void TimeAndAdcFitter::doAdcFit(TH1F *h, const int /*nAdcParams*/, double *Adcpfit, double *errAdcpfit, double* /*chi2Adc*/, int* /*ndofAdc*/ ) {

  double adcThreshold = 50.;
  double minAdc = 80.;
  double maxAdc = 300.;

  double m=h->GetMean();
  double r=h->GetRMS();
  double maxval=h->GetMaximum();
  double adcpar[4];
  adcpar[0] = maxval*2.;
  adcpar[1] = m;
  adcpar[2] = r;
  adcpar[3] = r/3.;
  
  TF1 *AdcSpectrum = new TF1("AdcSpectrum"," ([0]*exp((x-[1])/[2]))/ (1.+exp((x-[1])/[3])) ", minAdc,maxAdc );
  AdcSpectrum->SetLineColor(3);
  AdcSpectrum->SetParameters(adcpar);
  double fitMin = m-(3*r);
  double fitMax = m+(3*r);
  if (fitMin<adcThreshold) fitMin = adcThreshold;
  if (fitMax>maxAdc ) fitMax = maxAdc;
  h->Fit("AdcSpectrum","Q"," ",fitMin,fitMax);
  for (int i=0; i<4; i++) {
    Adcpfit[i] = AdcSpectrum->GetParameter(i);
    errAdcpfit[i] = AdcSpectrum->GetParError(i);
  }
  // THE NEW HISTOGRAM HAS BEEN FIT

  return;
}  //end TimeAndAdcFitter::doAdcFit

void  TimeAndAdcFitter::searchParams(TH1 *h, double *p, int /*nParams*/) {
  double sizeX = h->GetBinWidth(1);
  double oldSizeX=sizeX;
  int RebinFactor = (int) (10./sizeX);
  // extract starting values for fit params p[nParams] from the Time Spectrum h
  TH1 *hnew = h->Rebin(RebinFactor,"hnew"); // creates a new histogram hnew
  //merging 5 bins of h1 in one bin
  float minDeriv(9999.); 
  int minDerivBin(0);
  sizeX = hnew->GetBinWidth(1);
  int newbins = hnew->GetNbinsX();
  for(int i=0; i<newbins-1; ++i) {
    if(hnew->GetBinContent(i)-hnew->GetBinContent(i+1)
       <minDeriv) {
      minDeriv = hnew->GetBinContent(i)-hnew->GetBinContent(i+1);
      minDerivBin = i;
    }
  }
  float t0guess = hnew->GetBinCenter(minDerivBin);
  if (minDerivBin<newbins-1) {
    t0guess += (hnew->GetBinCenter(minDerivBin+1)-hnew->GetBinCenter(minDerivBin))/2.;
  }
  //
  // =================== Noise level search ===================================
  //
  float noise(0);
  int numOfBins(10), numOfBinsOffset(3);
  int imin, imax;
  if (minDerivBin>numOfBins+numOfBinsOffset) {
    imin = minDerivBin-numOfBins-numOfBinsOffset;
    imax = minDerivBin-numOfBinsOffset;
  } else {
    imin = 0;
    if (minDerivBin>numOfBinsOffset) {
      imax = minDerivBin-numOfBinsOffset;
    } else {
      imax = minDerivBin;
    }
  }
  int icount(0);
  for (int i=imin; i<=imax; ++i) {
    noise += hnew->GetBinContent(i);
    icount++;
  }
	   
  noise = noise/(float)(icount);
  //
  // =================== Normalization =========================================
  //
  int t0bin = minDerivBin;
  int ix1 = t0bin+(int)(50/sizeX);
  int ix2 = t0bin+(int)(500/sizeX);
  float P1=p[1];
  float P2=p[2];
  float P3=p[3];
  p[0]=noise;
  p[4]=t0guess;
  p[5]=p[4]+700;
  p[1]=20.;
  p[2]=10.; 
  if (0<ix1 && ix1<newbins && 0<ix2 && ix2<newbins ) {
    float a1=hnew->GetBinCenter(ix1);
    float a2=hnew->GetBinCenter(ix2);
    float cont1=hnew->GetBinContent(ix1);
    float cont2=hnew->GetBinContent(ix2);
    if (cont1>0. && cont2>0. ){
      float A1=std::exp(-(a1-t0guess)/P3);
      float A2=std::exp(-(a2-t0guess)/P3);
      // do not forget rebinning!
      P2 = (cont1/cont2-1.)/(A1-cont1/cont2*A2);
      P1 = cont1/(1+P2*A1);
      P1=P1*oldSizeX/sizeX;
      P2=P2*oldSizeX/sizeX;
      p[1]=P1;
      p[2]=P2;
    }
  }
  delete hnew;
  return;
}  //end TimeAndAdcFitter::searchParams
