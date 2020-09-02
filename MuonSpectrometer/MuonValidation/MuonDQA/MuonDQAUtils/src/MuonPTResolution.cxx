/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TF1.h"
#include "TH1F.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include "MuonDQAUtils/MuonPTResolution.h"

double getContentInRange(TH1F *hist, double centralX, double halfWidth)
{
  double ContentInRange=0;
  for (int i=0; i<=hist->GetNbinsX(); i++)
    {
      if (	( (hist->GetBinCenter(i) + hist->GetBinWidth(i)/2.) <	centralX+halfWidth) &&
		(  (hist->GetBinCenter(i) - hist->GetBinWidth(i)/2.) >	centralX-halfWidth ))
	{
	  ContentInRange=ContentInRange+ hist->GetBinContent(i); // If Bin is completly in range, add the full bin
	}
    }
  int borderbin=hist->FindBin(centralX-halfWidth);

  double	ContentInLeftBorderBin = hist->GetBinContent(borderbin)*
    ((hist->GetBinCenter(borderbin) + hist->GetBinWidth(borderbin)/2.) -(centralX-halfWidth))/hist->GetBinWidth(borderbin);

  borderbin=hist->FindBin(centralX+halfWidth);
  double	ContentInRightBorderBin = hist->GetBinContent(borderbin)*((centralX+halfWidth) - (hist->GetBinCenter(borderbin) -
												  hist->GetBinWidth(borderbin)/2.))/hist->GetBinWidth(borderbin);

  return ContentInRange+ContentInRightBorderBin+ContentInLeftBorderBin;
}



void getMuonPTResolution(	TH1F *hist,
				double &PTResMean,
				double &ErrMean,
				double &PTSigma,
				double &ErrSigma,
				double &TailContent)
{
  double empty = 0.;
  getMuonPTResolution(hist, 3, PTResMean, ErrMean, PTSigma, ErrSigma, TailContent,empty,empty);
}



void	getMuonPTResolution(	TH1F *hist,
				int n_max_runs,
				double &PTResMean,
				double &ErrMean,
				double &PTSigma,
				double &ErrSigma,
				double &TailFraction,
				double PercentageOfEntriesInWidth,
				double &HalfWidth)
{
  PTResMean	= -1.0;
  ErrMean		= -1.0;
  PTSigma		= -1.0;
  ErrSigma	= -1.0;
  TailFraction	= -1.0;
  HalfWidth	= -1.0;

  if (n_max_runs<=0) return;
  Double_t par[8];
  Double_t mean=0.0, sigma=0.0;
  Double_t old_sigma=0.0;
  PTResMean        = 0.0;

  TF1        gaussian("g1","gaus");
  gaussian.SetRange(-300,300);


  for (int i=1; i<=n_max_runs ;i++)
    {
      hist->Fit(&gaussian,"R");
      gaussian.GetParameters(&par[0]);
      mean	= par[1];
      sigma	= std::abs(par[2]);
      gaussian.SetRange(mean-sigma*2.,mean+sigma*2.);

      // Check if converged, if yes then quit
      if (std::abs(1.0-old_sigma/sigma)<0.0001) break;
      old_sigma=sigma;
    }


  PTResMean = mean;
  PTSigma = std::abs(sigma);
  ErrMean = (gaussian.GetParError(1));
  ErrSigma = std::abs((gaussian.GetParError(2)));
  int i=0;
  double TailContent=0;

  while ( (hist->GetBinCenter(i)-hist->GetBinWidth(i)/2.) < mean-sigma*2. )
    {
      TailContent=TailContent+hist->GetBinContent(i);
      i++;
    }
  i=hist->GetNbinsX();

  while ( (hist->GetBinCenter(i)+hist->GetBinWidth(i)/2.) > mean+sigma*2. )
    {
      TailContent=TailContent+hist->GetBinContent(i);
      i--;
    }

  TailFraction = ((double)(TailContent))/((double)hist->GetEntries());

  if (PercentageOfEntriesInWidth>1) PercentageOfEntriesInWidth=PercentageOfEntriesInWidth/100;

  double NumberOfEntriesInWidth = PercentageOfEntriesInWidth * 1.0 * hist->GetEntries();
  HalfWidth=0.;
  double StepLength=0.1;

  while (StepLength>sigma/1000.)
    {
      while ( getContentInRange(hist,mean,HalfWidth) < NumberOfEntriesInWidth )
	{
	  HalfWidth=HalfWidth+StepLength;
	}
      HalfWidth=HalfWidth-StepLength;

      StepLength=StepLength/10.;
    }
}


