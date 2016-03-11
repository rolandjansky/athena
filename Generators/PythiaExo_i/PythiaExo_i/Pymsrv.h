/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//--------------------------------------------------------------------------
// File and Version Information:
//      Pymsrv.hh
//
// Description:
//      Class definition for Pymsrv, which is used
//      to modify the Pythia Pymsrv common.
//
//
// Author List:
//      Claus Horn
//
//------------------------------------------------------------------------
// Pythia Common access method
#ifndef PYMSRV_H
#define PYMSRV_H

extern "C" {
  void* pymsrv_address_(void);
}

class Pymsrv {
public:
  Pymsrv();
  ~Pymsrv();

  double& rvlam(int n);
  double& rvlamp(int n);
  double& rvlamb(int n);

  int lenRVlam() const {return 27;}
  int lenRVlamp() const {return 27;}
  int lenRVlamb() const {return 27;}

  inline void init();

private: 

  struct PYMSRV;
  friend struct PYMSRV;

  struct PYMSRV {
    double rvlam[3][3][3];
    double rvlamp[3][3][3];
    double rvlamb[3][3][3];
  };

  double m_dummy;
  double m_realdummy;

  static PYMSRV* s_pymsrv;
};

#include "Pythia_i/Pymsrv.icc"

#endif
