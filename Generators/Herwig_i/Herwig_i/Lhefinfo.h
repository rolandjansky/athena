/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LHEFINFO_h
#define LHEFINFO_h

extern "C" {
  void* lhefinfo_address_();
}

/**
@class      Lhefinfo.hh

@brief      Class definition for Lhefinfo common block.

@author     Sebastian Piec 
*/

class Lhefinfo {
public:
  Lhefinfo();
  ~Lhefinfo();
  
  inline void init(); // inlined for speed of access (small function)

  int& id1();
  int& id2();
  double& x1();
  double& x2();
  double& scalePdf();
  double& xPdf1();
  double& xPdf2();

private: 
  struct LHEFINFO;
  friend struct LHEFINFO;

  struct LHEFINFO 
  {
    int     id1, id2;
    double  x1, x2;
    double  scalePdf, xPdf1, xPdf2;
  };

  static LHEFINFO* _lhefinfo;
};

#include "Herwig_i/Lhefinfo.icc"

#endif

