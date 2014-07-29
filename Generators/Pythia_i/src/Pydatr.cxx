/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// access pythia common Pydatr
#include "Pythia_i/Pydatr.h"

// set pointer to zero at start
Pydatr::PYDATR* Pydatr::_pydatr =0;

// Constructor
Pydatr::Pydatr() 
{
  _dummy=-999;
  _realdummy=-999.;
}

// Destructor
Pydatr::~Pydatr() 
{
}

// access mrpy in common
int& Pydatr::mrpy(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenMrpy()) {
  _dummy = -999;
  return _dummy;
  }
  return _pydatr->mrpy[n-1];
}

// access rrpy in common
double& Pydatr::rrpy(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenRrpy()) {
  _realdummy = -999.;
  return _realdummy;
  }
  return _pydatr->rrpy[n-1];
}

