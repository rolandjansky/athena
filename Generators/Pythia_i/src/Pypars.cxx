/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// access pythia common Pypars
#include "Pythia_i/Pypars.h"

// set pointer to zero at start
Pypars::PYPARS* Pypars::_pypars =0;

// Constructor
Pypars::Pypars() 
{
  _dummy=-999;
  _realdummy=-999.;
}

// Destructor
Pypars::~Pypars() 
{
}

// access mstp in common
int& Pypars::mstp(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenMstp()) {
  _dummy = -999;
  return _dummy;
  }
  return _pypars->mstp[n-1];
}

// access parp in common
double& Pypars::parp(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenParp()) {
  _realdummy = -999.;
  return _realdummy;
  }
  return _pypars->parp[n-1];
}

// access msti in common
int& Pypars::msti(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenMsti()) {
  _dummy = -999;
  return _dummy;
  }
  return _pypars->msti[n-1];
}

// access pari in common
double& Pypars::pari(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenPari()) {
  _realdummy = -999.;
  return _realdummy;
  }
  return _pypars->pari[n-1];
}

