/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifndef Ffpholun_h
#define Ffpholun_h

extern "C" {
  void* ffpholun_address_(void);
}

/**
@class    Ffpholun.h

@brief      Class definition for ffpholun, which is used
      to modify the Tauola FFPHOLUN common.

@author         B. P. Kersevan

 Ffpholun Common access method
*/

class Ffpholun {
public:
  Ffpholun();
  ~Ffpholun();
  int& phlun();

  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
 
 private: 

  struct PHOLUN;
  friend struct PHOLUN;

  struct PHOLUN {
    int phlun;
  };

  static PHOLUN* s_ffpholun;
};

#include "Photos_i/Ffpholun.icc"

#endif
