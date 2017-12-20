/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"

#include "TFile.h"
#include "TRandom1.h"
#include <iostream>

//=============================================
//======= TFCS1DFunction =========
//=============================================


double TFCS1DFunction::get_maxdev(TH1* h_input1, TH1* h_approx1)
{

  TH1D* h_input =(TH1D*)h_input1->Clone("h_input");
  TH1D* h_approx=(TH1D*)h_approx1->Clone("h_approx");

  double maxdev=0.0;

  //normalize the histos to the same area:
  double integral_input=h_input->Integral();
  double integral_approx=0.0;
  for(int b=1;b<=h_input->GetNbinsX();b++)
    integral_approx+=h_approx->GetBinContent(h_approx->FindBin(h_input->GetBinCenter(b)));
  h_approx->Scale(integral_input/integral_approx);

  double ymax=h_approx->GetBinContent(h_approx->GetNbinsX())-h_approx->GetBinContent(h_approx->GetMinimumBin());
  for(int i=1;i<=h_input->GetNbinsX();i++)
  {
    double val=fabs(h_approx->GetBinContent(h_approx->FindBin(h_input->GetBinCenter(i)))-h_input->GetBinContent(i))/ymax;
    if(val>maxdev) maxdev=val;
  }

  delete h_input;
  delete h_approx;

  return maxdev*100.0;

}

double TFCS1DFunction::rnd_to_fct(double)
{
  return 0;
}
