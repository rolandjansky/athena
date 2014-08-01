/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Pyint1_h
#define Pyint1_h
extern "C" {
  void* pyint1_address_(void);
}
/**
@class      Pyint1.h

@brief      Class definition for Pyint1, which is used
      to modify the Pythia Pyint1 common.

 Environment:
     ATLAS/ATHENA

@author      Elzbieta Richter-Was, Cracow INP  

 Pythia Common access method
*/

class Pyint1 {
public:
  Pyint1();
  ~Pyint1();
  int& mint(int n);
  double& vint(int n);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenMint() const {return _lenMint;}
  int lenVint() const {return _lenVint;}
private: 

  // Lengths of MINT and VINT COMMONS
  static const int _lenMint = 400;
  static const int _lenVint = 400;
 
  struct PYINT1;
  friend struct PYINT1;

  struct PYINT1 {
  int mint[_lenMint];
  double vint[_lenVint];
  };        
  int _dummy;
  double _realdummy;
  static PYINT1* _pyint1;
};
#include "PythiaExo_i/Pyint1.icc"
#endif
