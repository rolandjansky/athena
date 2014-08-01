/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Pydat2_h
#define Pydat2_h

extern "C" {
  void* pydat2_address_(void);
}
/**
@class      Pydat2.hh

@brief      Class definition for Pydat2, which is used
      to modify the Pythia Pydat2 common.

 Environment:
      CDF Run 2

@author      Chris Green,   Purdue University
      Stan Thompson, Glasgow University    

 Pythia Common access method
*/

class Pydat2 {
public:
  Pydat2();
  ~Pydat2();

  int& kchg(int kc, int i);
  double& pmas(int kc, int i);
  double& parf(int n);
  double& vckm(int i, int j);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int depthKchg() const {return _depthKchg;}
  int lenPmas() const {return _lenPmas;}
  int depthPmas() const {return _depthPmas;}
  int lenParf() const {return _lenParf;}
  int lenVckm() const {return _lenVckm;}
private: 

  // Lengths of array in PYDAT2 common
  static const int _lenPmas   = 500;
  static const int _depthPmas = 4;
  static const int _depthKchg = 4;
  static const int _lenParf   = 2000;
  static const int _lenVckm = 4;

  struct PYDAT2;
  friend struct PYDAT2;
  struct PYDAT2 {
  int kchg[_depthKchg][_lenPmas];
  double pmas[_depthPmas][_lenPmas];
  double parf[_lenParf];
  double vckm[_lenVckm][_lenVckm];
  };
  int _dummy;
  double _realdummy;
  static PYDAT2* _pydat2;
};

#include "PythiaExo_i/Pydat2.icc"

#endif
