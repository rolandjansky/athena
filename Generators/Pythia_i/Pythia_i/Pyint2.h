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
  int lenIset() const {return s_lenIset;}
  int depthKfpr() const {return s_depthKfpr;}
  int lenKfpr() const {return s_lenKfpr;}
  int lenCoef() const {return s_lenCoef;}
  int depthCoef() const {return s_depthCoef;}
  int lenIcol() const {return s_lenIcol;}
  int depthIcol() const {return s_depthIcol;}
  int widthIcol() const {return s_widthIcol;}
private: 

  // Lengths of ISET and KFPR COMMONS
  static const int s_lenIset = 500;
  static const int s_lenKfpr = 500;
  static const int s_depthKfpr = 2; 
  static const int s_lenCoef = 500;
  static const int s_depthCoef = 20; 
  static const int s_lenIcol = 40;
  static const int s_depthIcol = 4; 
  static const int s_widthIcol = 2; 

  struct PYINT2;
  friend struct PYINT2;

  struct PYINT2 {
  int iset[s_lenIset];
  int kfpr[s_depthKfpr][s_lenKfpr];
  double coef[s_depthCoef][s_lenCoef];
  int icol[s_widthIcol][s_depthIcol][s_lenIcol];
  };
  int m_dummy;
  double m_realdummy;
  static PYINT2* s_pyint2;
};

#include "Pythia_i/Pyint2.icc"

#endif
