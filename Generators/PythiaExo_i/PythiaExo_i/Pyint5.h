/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Pyint5_h
#define Pyint5_h

extern "C" {
  void* pyint5_address_(void);
}
/**
@class      Pyint5.h

@brief      Class definition for Pyint5, which is used
      to modify the Pythia Pyint5 common.

 Environment:
      ATLAS/ATHENA

@author      Ian Hinchliffe LBNL  

 Pythia Common access method
*/

class Pyint5 {
public:
  Pyint5();
  ~Pyint5();
  int& ngenpd();
  int& ngen(int n, int i);
  double& xsec(int n, int i);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int depthNgen() const {return s_depthNgen;}
  int lenNgen() const {return s_lenNgen;}
  int lenXsec() const {return s_lenXsec;}
  int depthXsec() const {return s_depthXsec;}
private: 

  // Lengths of NGENPD and NGEN COMMONS
  static const int s_lenNgen = 501;
  static const int s_depthNgen = 3; 
  static const int s_lenXsec = 501;
  static const int s_depthXsec = 3; 

  struct PYINT5;
  friend struct PYINT5;

  struct PYINT5 {
  int ngenpd;
  int ngen[s_depthNgen][s_lenNgen];
  double xsec[s_depthXsec][s_lenXsec];
  };
  int m_dummy;
  double m_realdummy;
  static PYINT5* s_pyint5;
};

#include "PythiaExo_i/Pyint5.icc"

#endif
