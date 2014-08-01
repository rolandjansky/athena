/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef Pydat1_h
#define Pydat1_h

extern "C" {
  void* pydat1_address_(void);
}

/**
@class      Pydat1.hh

@brief      Class definition for Pydat1, which is used
      to modify the Pythia Pydat1 common.

 Environment:
      CDF Run 2

@author      Chris Green,   Purdue University
      Stan Thompson, Glasgow University    

 Pythia Common access method
*/
class Pydat1 {
public:
  Pydat1();
  ~Pydat1();
  int& mstu(int n);
  double& paru(int n);
  int& mstj(int n);
  double& parj(int n);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenMstu() const {return _lenMstu;}
  int lenParu() const {return _lenParu;}
  int lenMstj() const {return _lenMstj;}
  int lenParj() const {return _lenParj;}
private: 

  // Lengths of MSTU and PARU COMMONS
  static const int _lenMstu = 200;
  static const int _lenParu = 200;
  static const int _lenMstj = 200;
  static const int _lenParj = 200;

  struct PYDAT1;
  friend struct PYDAT1;

  struct PYDAT1 {
  int mstu[_lenMstu];
  double paru[_lenParu];
  int mstj[_lenMstj];
  double parj[_lenParj];
  };
  int _dummy;
  double _realdummy;
  static PYDAT1* _pydat1;
};

#include "PythiaExo_i/Pydat1.icc"

#endif
