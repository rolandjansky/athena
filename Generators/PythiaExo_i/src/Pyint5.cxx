/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// access pythia common Pyint5
#include "PythiaExo_i/Pyint5.h"

// set pointer to zero at start
Pyint5::PYINT5* Pyint5::_pyint5 =0;

// Constructor
Pyint5::Pyint5() 
{
  _dummy = -999;
  _realdummy = -999.;
}
 
// Destructor
Pyint5::~Pyint5() 
{
}

// access ngenpd in common
int& Pyint5::ngenpd() {
  init(); // check COMMON is initialized
  return _pyint5->ngenpd;
}

// access ngen in common
  int& Pyint5::ngen(int n, int i) {
  init(); // check COMMON is initialized
  if(n < 0 || n > lenNgen() ||
     i <1 || i >depthNgen()) {
  _dummy = -999;
  return _dummy;
  }
//    return _pyint5->ngen[i][n-1];
  return _pyint5->ngen[i-1][n];
}

// access xsec in common
double& Pyint5::xsec(int n, int i) {
  init(); // check COMMON is initialized
  if(n < 0 || n > lenXsec() ||
     i <1 || i > depthXsec()) {
  _realdummy = -999.;
  return _realdummy;
  }
//    return _pyint5->xsec[i][n-1];
  return _pyint5->xsec[i-1][n];
}



