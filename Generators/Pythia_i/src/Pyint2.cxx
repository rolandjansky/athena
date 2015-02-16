/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// access pythia common Pyint2
#include "Pythia_i/Pyint2.h"

// set pointer to zero at start
Pyint2::PYINT2* Pyint2::_pyint2 =0;

// Constructor
Pyint2::Pyint2() 
{
  _dummy=-999;
  _realdummy=-999.;
}
 
// Destructor
Pyint2::~Pyint2() 
{
}

// access iset in common
int& Pyint2::iset(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenIset()) {
  _dummy = -999;
  return _dummy;
  }
  return _pyint2->iset[n-1];
}

// access kfpr in common
  int& Pyint2::kfpr(int n, int i) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenKfpr() ||
     i <1 || i >depthKfpr()) {
  _dummy = -999;
  return _dummy;
  }
  return _pyint2->kfpr[i-1][n-1];
}

// access coef in common
double& Pyint2::coef(int n, int i) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenCoef() ||
     i <1 || i > depthCoef()) {
  _realdummy = -999.;
  return _realdummy;
  }
  return _pyint2->coef[i-1][n-1];
}

// access icol in common
int& Pyint2::icol(int n, int i ,int j) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenIcol() ||
     i <1 || i > depthIcol() ||
     j < 1 || j > widthIcol() ) {
  _dummy = -999;
  return _dummy;
  }
  return _pyint2->icol[j-1][i-1][n-1];
}

