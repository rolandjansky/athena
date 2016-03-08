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
Fftaudet::TAUDET* Fftaudet::s_fftaudet =0;

// Constructor
Fftaudet::Fftaudet() 
{
  m_dummy = 0;
  m_realdummy = 0.;
}

// Destructor
Fftaudet::~Fftaudet() 
{
}


int& Fftaudet::ntausav() {
  init(); // check COMMON is initialized
  
  return s_fftaudet->ntausav;
}

int& Fftaudet::evtnum() {
  init(); // check COMMON is initialized
  
  return s_fftaudet->evtnum;
}

int& Fftaudet::itnp(int nt) {
  init(); // check COMMON is initialized
  
  if( nt < 1 || nt > lenTaudet()) 
    {
      m_dummy = -999;
      return m_dummy;
    }

  return s_fftaudet->itnp[nt-1];
}

int& Fftaudet::itjak(int nt) {
  init(); // check COMMON is initialized
  
  if( nt < 1 || nt > lenTaudet()) 
    {
      m_dummy = -999;
      return m_dummy;
    }

  return s_fftaudet->itjak[nt-1];
}

float& Fftaudet::ftpolx(int nt) {
  init(); // check COMMON is initialized
  
  if( nt < 1 || nt > lenTaudet()) 
    {
      m_realdummy = -999;
      return m_realdummy;
    }

  return s_fftaudet->ftpolx[nt-1];
}

float& Fftaudet::ftpoly(int nt) {
  init(); // check COMMON is initialized
  
  if( nt < 1 || nt > lenTaudet()) 
    {
      m_realdummy = -999;
      return m_realdummy;
    }

  return s_fftaudet->ftpoly[nt-1];
}

float& Fftaudet::ftpolz(int nt) {
  init(); // check COMMON is initialized
  
  if( nt < 1 || nt > lenTaudet()) 
    {
      m_realdummy = -999;
      return m_realdummy;
    }

  return s_fftaudet->ftpolz[nt-1];
}

