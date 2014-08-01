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

  int lenImss() const {return _lenImss;}
  int lenRmss() const {return _lenRmss;}

  inline void init();

private: 

  static const int _lenImss = 100;
  static const int _lenRmss = 100;

  struct PYMSSM;
  friend struct PYMSSM;

  struct PYMSSM {
    int imss[_lenImss];
    double rmss[_lenRmss];
  };

  int _dummy;
  double _realdummy;

  static PYMSSM* _pymssm;
};

#include "PythiaExo_i/Pymssm.icc"

#endif
