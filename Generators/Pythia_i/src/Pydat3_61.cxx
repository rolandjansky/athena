/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// access pythia common Pydat3
#include "Pythia_i/Pydat3.h"

#ifdef DEFECT_OLD_STDC_HEADERS
extern "C" {
#include <stdlib.h>
}
#else
#include <cstdlib>
#endif

// set pointer to zero at start
Pydat3::PYDAT3* Pydat3::_pydat3 =0;

// Constructor
Pydat3::Pydat3() 
  : 
  _dummy(-999)
  , _realdummy(-999.0) 
{
}

// Destructor
Pydat3::~Pydat3() 
{
}

// access mdcy in common
int& Pydat3::mdcy(int kc, int i) {
  init(); // check COMMON is initialized
  if( kc < 1 || kc > lenMdcy() ||
      i  < 1 || i  > depthMdcy())
 {
  _dummy = -999;
  return _dummy;
  }
  return _pydat3->mdcy[i-1][kc-1];
}
// access mdme in common
int& Pydat3::mdme(int idc, int i) {
  init(); // check COMMON is initialized
  if( idc < 1 || idc > lenMdme() ||
      i  < 1 || i  > depthMdme())
 {
  _dummy = -999;
  return _dummy;
  }
  return _pydat3->mdme[i-1][idc-1];
}
// access brat in common
double& Pydat3::brat(int idc) {
  init(); // check COMMON is initialized
  if(idc < 1 || idc > lenBrat()) {
  _realdummy = -999.;
  return _realdummy;
  }
  return _pydat3->brat[idc-1];

}
// access kfdp in common
int& Pydat3::kfdp(int idc, int kf) {
  init(); // check COMMON is initialized
  if( idc < 1 || idc > lenKfdp() ||
      kf  < 1 || kf  > depthKfdp())
 {
  _dummy = -999;
  return _dummy;
  }
  return _pydat3->kfdp[kf-1][idc-1];
}

