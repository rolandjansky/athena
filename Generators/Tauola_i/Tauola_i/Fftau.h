/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef Fftau_h
#define Fftau_h

extern "C" {
  void* fftau_address_(void);
}

/**
@class     Fftau.h

@brief      Class definition for fftau, which is used
      to modify the Tauola FFTAU common.

@author            B. P. Kersevan

 Fftau Common access method
*/

class Fftau {
public:
  Fftau();
  ~Fftau();
  int& ifpol();
  int& ifradcor();
  int& ifdmod();
  int& ifjak1();
  int& ifjak2();
  float& ffphx();

  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
 
private: 

  struct FFTAU;
  friend struct FFTAU;

  struct FFTAU {
    int ifpol;
    int ifradcor;
    int ifdmod;
    int ifjak1;
    int ifjak2;
    float ffphx;
  };

  static FFTAU* s_fftau;
};

#include "Tauola_i/Fftau.icc"

#endif
