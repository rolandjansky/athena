/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Fftaudet_h
#define Fftaudet_h

extern "C" {
  void* fftaudet_address_(void);
}

/**
@class Fftaudet.h

@brief      Needed fot interface of Tauola details to HEPEvt

@author Eric Torrence, August 2009

TAUDET common
      INTEGER NMXTAU
      PARAMETER (NMXTAU=20)
      INTEGER NTAUSAV, EVTNUM
      INTEGER ITNP(NMXTAU), ITJAK(NMXTAU)
      REAL*4  FTPOLX(NMXTAU), FTPOLY(NMXTAU), FTPOLZ(NMXTAU)
      COMMON / TAUDET / ITNP, ITJAK, FTPOLX, FTPOLY, FTPOLZ, NTAUSAV, EVTNUM

*/

class Fftaudet {
public:
  Fftaudet();
  ~Fftaudet();

  int& ntausav();
  int& evtnum();
  int& itnp(int nt);
  int& itjak(int nt);
  float& ftpolx(int nt);
  float& ftpoly(int nt);
  float& ftpolz(int nt);

  inline void init(); // inlined for speed of access (small function)

  int lenTaudet() const {return s_lenTaudet;}

private: 
  // Lengths of array in TAUDET common
  static const int s_lenTaudet = 20 ; // Maximum number of tau decays to record

  struct TAUDET;
  friend struct TAUDET;

  struct TAUDET {
    int itnp[s_lenTaudet];
    int itjak[s_lenTaudet];
    float ftpolx[s_lenTaudet];
    float ftpoly[s_lenTaudet];
    float ftpolz[s_lenTaudet];
    int ntausav;
    int evtnum;
  };

  int m_dummy;
  float m_realdummy;

  static TAUDET* s_fftaudet;
};

#include "Tauola_i/Fftaudet.icc"

#endif
