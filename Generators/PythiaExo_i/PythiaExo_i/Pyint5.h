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
  int depthNgen() const {return _depthNgen;}
  int lenNgen() const {return _lenNgen;}
  int lenXsec() const {return _lenXsec;}
  int depthXsec() const {return _depthXsec;}
private: 

  // Lengths of NGENPD and NGEN COMMONS
  static const int _lenNgen = 501;
  static const int _depthNgen = 3; 
  static const int _lenXsec = 501;
  static const int _depthXsec = 3; 

  struct PYINT5;
  friend struct PYINT5;

  struct PYINT5 {
  int ngenpd;
  int ngen[_depthNgen][_lenNgen];
  double xsec[_depthXsec][_lenXsec];
  };
  int _dummy;
  double _realdummy;
  static PYINT5* _pyint5;
};

#include "PythiaExo_i/Pyint5.icc"

#endif
