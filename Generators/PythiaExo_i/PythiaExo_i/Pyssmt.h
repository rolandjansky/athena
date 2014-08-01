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

  static const int _sizeZmix = 4;
  static const int _sizeUmix = 2;
  static const int _sizeVmix = 2;
  static const int _lenSmz = 4;
  static const int _lenSmw = 2;
  static const int _lenSfmix = 4;
  static const int _widthSfmix = 16;

  struct PYSSMT;
  friend struct PYSSMT;

  struct PYSSMT {
    double zmix[_sizeZmix][_sizeZmix];
    double umix[_sizeUmix][_sizeUmix];
    double vmix[_sizeVmix][_sizeVmix];
    double smz[_lenSmz];
    double smw[_lenSmw];
    double sfmix[_lenSfmix][_widthSfmix];
  };

  mutable int _dummy;
  mutable double _realdummy;

  static PYSSMT const * _pyssmt;
};

#include "PythiaExo_i/Pyssmt.icc"

#endif
