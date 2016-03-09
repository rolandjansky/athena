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

  int lenItcm() const {return s_lenItcm;}
  int lenRtcm() const {return s_lenRtcm;}

  inline void init();

private: 

  static const int s_lenItcm = 100;
  static const int s_lenRtcm = 100;

  struct PYTCSM;
  friend struct PYTCSM;

  struct PYTCSM {
    int itcm[s_lenItcm];
    double rtcm[s_lenRtcm];
  };

  int m_dummy;
  double m_realdummy;

  static PYTCSM* s_pytcsm;
};

#include "Pythia_i/Pytcsm.icc"

#endif
