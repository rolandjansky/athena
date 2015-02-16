/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// access Pythia common Pysubs
#include "Pythia_i/Pysubs.h"

//#ifdef DEFECT_OLD_STDC_HEADERS
//extern "C" {
//#include <stdlib.h>
//}
//#else
#include <cstdlib>
//#endif

// set pointer to zero at start
Pysubs::PYSUBS* Pysubs::_pysubs =0;

// Constructor
Pysubs::Pysubs() 
{
  _dummy=-999;
  _realdummy=-999.;
}

// Destructor
Pysubs::~Pysubs() 
{
}

// access msel in common
int& Pysubs::msel() {
  init(); // check COMMON is initialized
  return _pysubs->msel;
}

// access msub in common
int& Pysubs::msub(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenMsub()) {
  _dummy = -999;
  return _dummy;
  }
  return _pysubs->msub[n-1];
}

// access kfin in common
int& Pysubs::kfin(int iside, int jflav) {
  init(); // check COMMON is initialized
  int half_lenKfin = lenKfin()/2;
  if(std::abs(jflav) > half_lenKfin ||
     iside < 1 || iside > depthKfin())
 {
  _dummy = -999;
  return _dummy;
  }
  return _pysubs->kfin[jflav+half_lenKfin][iside-1];
}

// access ckin in common
double& Pysubs::ckin(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenCkin()) {
  _realdummy = -999.;
  return _realdummy;
  }
  return _pysubs->ckin[n-1];
}

