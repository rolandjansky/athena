/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// access pythia common Pyint1
#include "PythiaExo_i/Pyint1.h"

// set pointer to zero at start
Pyint1::PYINT1* Pyint1::s_pyint1 =0;

// Constructor
Pyint1::Pyint1() 
{
  m_dummy = -999;
  m_realdummy = -999.;
}
 
// Destructor
Pyint1::~Pyint1() 
{
}

// access mint in common
int& Pyint1::mint(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenMint()) {
  m_dummy = -999;
  return m_dummy;
  }
  return s_pyint1->mint[n-1];
}

// access iset in common
double& Pyint1::vint(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenVint()) {
  m_realdummy = -999;
  return m_realdummy;
  }
  return s_pyint1->vint[n-1];
}
