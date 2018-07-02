/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef RMSFRAC_H
#define RMSFRAC_H 

#include "TH1D.h"
#include "TF1.h"

namespace generate {

double findMean(TH1D* s, double frac);

double rmsFrac(TH1D* s, double frac, double mean);
double rmsFrac(TH1D* s, double frac);

/// get the fraction of the rms95 value with respect 
/// to the rms95 value of a gaussian
double rms95(TH1D* s, double mean);
double rms95(TH1D* s);


double GetEntries(TH1D* h, int ilow, int ihi);
double GetEntries(TH1D* h);

}

#endif /// RMSFRAC_H 
