/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PYSSMT_H
#define PYSSMT_H

extern "C" {
  void* pyssmt_address_(void);
}
/**

@class      Pyssmt.hh

@brief      Class definition for Pyssmt, which is used
      to modify the Pythia Pyssmt common.

 Environment:
      CDF Run 2


@author      Chris Green,   Purdue University
      Stan Thompson, Glasgow University    

 Pythia Common access method
*/
class Pyssmt {
public:
  Pyssmt();
  ~Pyssmt();

  const double& zmix(int i, int j) const;
  const double& umix(int i, int j) const;
  const double& vmix(int i, int j) const;
  const double& smz(int i) const;
  const double& smw(int i) const;
  const double& sfmix(int i, int j) const;

  inline void init();

private: 

  static const int s_sizeZmix = 4;
  static const int s_sizeUmix = 2;
  static const int s_sizeVmix = 2;
  static const int s_lenSmz = 4;
  static const int s_lenSmw = 2;
  static const int s_lenSfmix = 4;
  static const int s_widthSfmix = 16;

  struct PYSSMT;
  friend struct PYSSMT;

  struct PYSSMT {
    double zmix[s_sizeZmix][s_sizeZmix];
    double umix[s_sizeUmix][s_sizeUmix];
    double vmix[s_sizeVmix][s_sizeVmix];
    double smz[s_lenSmz];
    double smw[s_lenSmw];
    double sfmix[s_lenSfmix][s_widthSfmix];
  };

  mutable int m_dummy;
  mutable double m_realdummy;

  static PYSSMT const * s_pyssmt;
};

#include "PythiaExo_i/Pyssmt.icc"

#endif
