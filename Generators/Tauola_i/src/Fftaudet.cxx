//--------------------------------------------------------------------------
// File and Version Information:
//      Fftaudet.cxx
//
// Description:
//      Needed for interface of Tauola details to HEPevt
//
// Author List:
// Eric Torrence, August 2009
// Taken from Atlas_HEPEVT
//
// Copyright Information:
//
//------------------------------------------------------------------------
#include "Tauola_i/Fftaudet.h"

// set pointer to zero at start
Fftaudet::TAUDET* Fftaudet::_fftaudet =0;

// Constructor
Fftaudet::Fftaudet() 
{
  _dummy = 0;
  _realdummy = 0.;
}

// Destructor
Fftaudet::~Fftaudet() 
{
}


int& Fftaudet::ntausav() {
  init(); // check COMMON is initialized
  
  return _fftaudet->ntausav;
}

int& Fftaudet::evtnum() {
  init(); // check COMMON is initialized
  
  return _fftaudet->evtnum;
}

int& Fftaudet::itnp(int nt) {
  init(); // check COMMON is initialized
  
  if( nt < 1 || nt > lenTaudet()) 
    {
      _dummy = -999;
      return _dummy;
    }

  return _fftaudet->itnp[nt-1];
}

int& Fftaudet::itjak(int nt) {
  init(); // check COMMON is initialized
  
  if( nt < 1 || nt > lenTaudet()) 
    {
      _dummy = -999;
      return _dummy;
    }

  return _fftaudet->itjak[nt-1];
}

float& Fftaudet::ftpolx(int nt) {
  init(); // check COMMON is initialized
  
  if( nt < 1 || nt > lenTaudet()) 
    {
      _realdummy = -999;
      return _realdummy;
    }

  return _fftaudet->ftpolx[nt-1];
}

float& Fftaudet::ftpoly(int nt) {
  init(); // check COMMON is initialized
  
  if( nt < 1 || nt > lenTaudet()) 
    {
      _realdummy = -999;
      return _realdummy;
    }

  return _fftaudet->ftpoly[nt-1];
}

float& Fftaudet::ftpolz(int nt) {
  init(); // check COMMON is initialized
  
  if( nt < 1 || nt > lenTaudet()) 
    {
      _realdummy = -999;
      return _realdummy;
    }

  return _fftaudet->ftpolz[nt-1];
}

