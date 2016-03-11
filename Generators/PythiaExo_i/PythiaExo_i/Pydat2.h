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
  int depthKchg() const {return s_depthKchg;}
  int lenPmas() const {return s_lenPmas;}
  int depthPmas() const {return s_depthPmas;}
  int lenParf() const {return s_lenParf;}
  int lenVckm() const {return s_lenVckm;}
private: 

  // Lengths of array in PYDAT2 common
  static const int s_lenPmas   = 500;
  static const int s_depthPmas = 4;
  static const int s_depthKchg = 4;
  static const int s_lenParf   = 2000;
  static const int s_lenVckm = 4;

  struct PYDAT2;
  friend struct PYDAT2;
  struct PYDAT2 {
  int kchg[s_depthKchg][s_lenPmas];
  double pmas[s_depthPmas][s_lenPmas];
  double parf[s_lenParf];
  double vckm[s_lenVckm][s_lenVckm];
  };
  int m_dummy;
  double m_realdummy;
  static PYDAT2* s_pydat2;
};

#include "PythiaExo_i/Pydat2.icc"

#endif
