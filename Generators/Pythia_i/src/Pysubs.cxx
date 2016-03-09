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
Pysubs::PYSUBS* Pysubs::s_pysubs =0;

// Constructor
Pysubs::Pysubs() 
{
  m_dummy=-999;
  m_realdummy=-999.;
}

// Destructor
Pysubs::~Pysubs() 
{
}

// access msel in common
int& Pysubs::msel() {
  init(); // check COMMON is initialized
  return s_pysubs->msel;
}

// access msub in common
int& Pysubs::msub(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenMsub()) {
  m_dummy = -999;
  return m_dummy;
  }
  return s_pysubs->msub[n-1];
}

// access kfin in common
int& Pysubs::kfin(int iside, int jflav) {
  init(); // check COMMON is initialized
  int half_lenKfin = lenKfin()/2;
  if(std::abs(jflav) > half_lenKfin ||
     iside < 1 || iside > depthKfin())
 {
  m_dummy = -999;
  return m_dummy;
  }
  return s_pysubs->kfin[jflav+half_lenKfin][iside-1];
}

// access ckin in common
double& Pysubs::ckin(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenCkin()) {
  m_realdummy = -999.;
  return m_realdummy;
  }
  return s_pysubs->ckin[n-1];
}

