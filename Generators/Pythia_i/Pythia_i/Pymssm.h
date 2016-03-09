/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PYMSSM_H
#define PYMSSM_H

extern "C" {
  void* pymssm_address_(void);
}
/**
@class      Pymssm.hh

@brief      Class definition for Pymssm, which is used
      to modify the Pythia Pymssm common.

 Environment:
      CDF Run 2

@author      Chris Green,   Purdue University
      Stan Thompson, Glasgow University    

 Pythia Common access method
*/
class Pymssm {
public:
  Pymssm();
  ~Pymssm();

  int& imss(int n);
  double& rmss(int n);

  int lenImss() const {return s_lenImss;}
  int lenRmss() const {return s_lenRmss;}

  inline void init();

private: 

  static const int s_lenImss = 100;
  static const int s_lenRmss = 100;

  struct PYMSSM;
  friend struct PYMSSM;

  struct PYMSSM {
    int imss[s_lenImss];
    double rmss[s_lenRmss];
  };

  int m_dummy;
  double m_realdummy;

  static PYMSSM* s_pymssm;
};

#include "Pythia_i/Pymssm.icc"

#endif
