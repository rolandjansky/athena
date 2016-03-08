/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UserGamprt_h
#define UserGamprt_h

extern "C" {
  void* usergamprt_address_(void);
}

/**
@class Fftaudet.h

@brief      Needed for interface of Tauola 

@author JanKotanski, August 2011

USERGAMPRT common
      INTEGER UGUSED
      PARAMETER (NMX=30)
      REAL*4  USERGAMPRT(NMX)
      COMMON / USERGAMPRT / UGUSED, USERGAMPRT

*/

class UserGamprt {
public:
  UserGamprt();
  ~UserGamprt();

  int& ugused();
  float& userGamprt(int nt);

  inline void init(); // inlined for speed of access (small function)

  int nmax() const {return s_nmax;}

private: 
  // Lengths of array in USERGAMPRT common
  static const int s_nmax = 30 ; // Length of the array

  struct USERGAMPRT;
  friend struct USERGAMPRT;

  struct USERGAMPRT {
    int ugused;
    float usergamprt[s_nmax];
  };

  static USERGAMPRT* s_usergamprt;

  float m_realdummy;

};

#include "Tauola_i/UserGamprt.icc"

#endif
