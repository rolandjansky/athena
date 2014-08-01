/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYTCSM_H
#define PYTCSM_H

extern "C" {
  void* pytcsm_address_(void);
}
/**
@class   Pytcsm.hh

@brief Description:
      Class definition for Pytcsm, which is used
      to modify the Pythia Pytcsm common.

@author      Georgios Stavropoulos, U.C. Berkeley

 Pythia Common access method
*/
class Pytcsm {
public:
  Pytcsm();
  ~Pytcsm();

  int& itcm(int n);
  double& rtcm(int n);

  int lenItcm() const {return _lenItcm;}
  int lenRtcm() const {return _lenRtcm;}

  inline void init();

private: 

  static const int _lenItcm = 100;
  static const int _lenRtcm = 100;

  struct PYTCSM;
  friend struct PYTCSM;

  struct PYTCSM {
    int itcm[_lenItcm];
    double rtcm[_lenRtcm];
  };

  int _dummy;
  double _realdummy;

  static PYTCSM* _pytcsm;
};

#include "PythiaExo_i/Pytcsm.icc"

#endif
