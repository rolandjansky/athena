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

  int lenTaudet() const {return _lenTaudet;}

private: 
  // Lengths of array in TAUDET common
  static const int _lenTaudet = 20 ; // Maximum number of tau decays to record

  struct TAUDET;
  friend struct TAUDET;

  struct TAUDET {
    int itnp[_lenTaudet];
    int itjak[_lenTaudet];
    float ftpolx[_lenTaudet];
    float ftpoly[_lenTaudet];
    float ftpolz[_lenTaudet];
    int ntausav;
    int evtnum;
  };

  int _dummy;
  float _realdummy;

  static TAUDET* _fftaudet;
};

#include "Tauola_i/Fftaudet.icc"

#endif
