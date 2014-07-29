/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// access pythia common Pyint1
#include "Pythia_i/Pyint1.h"

// set pointer to zero at start
Pyint1::PYINT1* Pyint1::_pyint1 =0;

// Constructor
Pyint1::Pyint1() 
{
  _dummy=-999;
  _realdummy=-999.;
}
 
// Destructor
Pyint1::~Pyint1() 
{
}

// access mint in common
int& Pyint1::mint(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenMint()) {
  _dummy = -999;
  return _dummy;
  }
  return _pyint1->mint[n-1];
}

// access iset in common
double& Pyint1::vint(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenVint()) {
  _realdummy = -999;
  return _realdummy;
  }
  return _pyint1->vint[n-1];
}
