/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifndef Ffinout_h
#define Ffinout_h

extern "C" {
  void* ffinout_address_(void);
}
/**
@class  Ffinout.h

@brief      Class definition for ffinout, which is used
      to modify the Tauola FFINOUT common.

@author B. P. Kersevan

 Ffinout Common access method
*/

class Ffinout {
public:
  Ffinout();
  ~Ffinout();
  int& inut();
  int& iout();

  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
 
 private: 

  struct INOUT;
  friend struct INOUT;

  struct INOUT {
    int inut;
    int iout;
  };

  static INOUT* s_ffinout;
};

#include "Tauola_i/Ffinout.icc"

#endif
