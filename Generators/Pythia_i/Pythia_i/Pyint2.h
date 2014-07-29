/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef Pyint2_h
#define Pyint2_h

extern "C" {
  void* pyint2_address_(void);
}
/**

@class      Pyint2.h

@brief      Class definition for Pyint2, which is used
      to modify the Pythia Pyint2 common.

 Environment:
      ATLAS/ATHENA

@author      Ian Hinchliffe LBNL  

 Pythia Common access method
*/

class Pyint2 {
public:
  Pyint2();
  ~Pyint2();
  int& iset(int n);
  int& kfpr(int n, int i);
  double& coef(int n, int i);
  int& icol(int n,int i,int j);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenIset() const {return _lenIset;}
  int depthKfpr() const {return _depthKfpr;}
  int lenKfpr() const {return _lenKfpr;}
  int lenCoef() const {return _lenCoef;}
  int depthCoef() const {return _depthCoef;}
  int lenIcol() const {return _lenIcol;}
  int depthIcol() const {return _depthIcol;}
  int widthIcol() const {return _widthIcol;}
private: 

  // Lengths of ISET and KFPR COMMONS
  static const int _lenIset = 500;
  static const int _lenKfpr = 500;
  static const int _depthKfpr = 2; 
  static const int _lenCoef = 500;
  static const int _depthCoef = 20; 
  static const int _lenIcol = 40;
  static const int _depthIcol = 4; 
  static const int _widthIcol = 2; 

  struct PYINT2;
  friend struct PYINT2;

  struct PYINT2 {
  int iset[_lenIset];
  int kfpr[_depthKfpr][_lenKfpr];
  double coef[_depthCoef][_lenCoef];
  int icol[_widthIcol][_depthIcol][_lenIcol];
  };
  int _dummy;
  double _realdummy;
  static PYINT2* _pyint2;
};

#include "Pythia_i/Pyint2.icc"

#endif
