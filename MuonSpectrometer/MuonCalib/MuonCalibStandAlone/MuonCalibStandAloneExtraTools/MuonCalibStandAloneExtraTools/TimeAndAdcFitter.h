/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SRC_TIMEANDADCFITTER_H
#define SRC_TIMEANDADCFITTER_H

#include "TMath.h"
#include <string.h>
#include <stdio.h>

#include <sstream>
#include <iostream>
#include <fstream> 

#include <TFile.h>
#include <TString.h>
#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "TObjArray.h"


//using namespace std;


class TimeAndAdcFitter {
public:
  TimeAndAdcFitter() : m_verbose(0) {};  
  ~TimeAndAdcFitter() {};
  void doTimeFit(TH1F * h, const int nParams, double * pfit, double * errfit, double* chi2, int* ndof ) ;
  // void doTimeFit(TH1F * h);
  void doAdcFit(TH1F * h, const int nAdcParams, double * Adcpfit, double * errAdcpfit, double* chi2Adc, int* ndofAdc ) ;
  void searchParams(TH1 * h, double * p, int np);
  void setVerbose(int lvl) { m_verbose = lvl; }

private:
  int m_verbose;
  
};

////////////////////////////////////////////////////////////////////
//
// UTILITIES of General use
//
/*
class ToString {
 public:
  template< class T >
    std::string operator()( const T& i )
  {
    std::ostringstream os;
    os << i;
    return os.str();
  }
};
*/
#endif
