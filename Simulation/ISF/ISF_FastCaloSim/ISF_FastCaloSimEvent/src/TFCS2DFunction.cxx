/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCS2DFunction.h"
#include "TH2.h"
#include <iostream>

//=============================================
//======= TFCS2DFunction =========
//=============================================

void TFCS2DFunction::rnd_to_fct(float value[],const float rnd[]) const
{
  rnd_to_fct(value[0],value[1],rnd[0],rnd[1]);
}

//================================================================================================================================

double TFCS2DFunction::CheckAndIntegrate2DHistogram(const TH2* hist, std::vector<double>& integral_vec, int& first, int& last) 
{
  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbins = nbinsx*nbinsy;

  float integral=0.0;
  float hint = hist->Integral();
  integral_vec.resize(nbins);


  for (int ix=1; ix<=nbinsx; ix++) {
    for (int iy=1; iy<=nbinsy; iy++) {
      int globalbin = (ix-1)*nbinsy + iy - 1;   
	    float binval = hist->GetBinContent(ix,iy);
      if(binval<0) {
        //Can't work if a bin is negative, forcing bins to 0 in this case
        double fraction=binval/hint;
        if(TMath::Abs(fraction)>1e-5) {
          std::cout<<"WARNING: bin content is negative in histogram "<<hist->GetName()<<" : "<<hist->GetTitle()<<" binval="<<binval<<" "<<fraction*100<<"% of integral="<<hist->Integral()<<". Forcing bin to 0."<<std::endl;
        }  
        binval=0;
      }
      integral+=binval;
      integral_vec[globalbin]=integral;
    }
  }

  for(first=0; first<nbins; first++) if(integral_vec[first]!=0) break;
  for(last=nbins-1; last>0; last--) if(integral_vec[last]!=integral) break;
  last++;
  
  if(integral<=0) {
    std::cout<<"ERROR: histogram "<<hist->GetName()<<" : "<<hist->GetTitle()<<" integral="<<integral<<" is <=0"<<std::endl;
  }
  
  return integral;
}

